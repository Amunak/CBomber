#include <cstdlib>
#include <curses.h>
#include <panel.h>
#include <csignal>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

#include "CGame.h"
#include "CCoord.h"
#include "CNetwork.h"

using namespace std;

CWorld CGame::m_world = CWorld();
CGame::panel_type CGame::errpan = CGame::panel_type(NULL, NULL);
CGame::panel_type CGame::debugpan = CGame::panel_type(NULL, NULL);
CGame::multiplayer_mode CGame::m_mp_mode = CGame::multiplayer_mode::OFFLINE;
string CGame::m_serverAddr = "ipv6-localhost";
deque<string> CGame::m_debugWindowContent;

CWorld& CGame::GetWorld() {
    return m_world;
}

CCoord::screen_coord_type CGame::minScreenSize() {
    CCoord::screen_coord_type scr = CCoord::MaxScreenCoords();
    scr.first += CWorld::DRAWOFFSET_LEFT;
    scr.second += CWorld::DRAWOFFSET_TOP;
    return scr;
}

void CGame::Draw() {
    CGame::m_world.Draw();
    refresh();
}

int CGame::InitGame(const CGame::multiplayer_mode & mpMode, unsigned int seed) {
    CGame::m_mp_mode = mpMode;
    if (seed == 0) {
        struct timeval time;
        gettimeofday(&time, NULL);
        seed = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    }
    srand(seed); // seed random number generator

    return seed;
}

void CGame::InitCurses() {
    initscr();

    // Basic "settings"
    cbreak();
    noecho();
    curs_set(0);
    timeout((1000 / CGame::FPS) / (m_mp_mode == multiplayer_mode::CLIENT ? 2 : 1)); //getch() timeout based on target FPS
    start_color();
    keypad(stdscr, TRUE);

    // Color pairs definition
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);

    CGame::InitErrorWindow();
    CGame::InitDebugWindow();
    signal(SIGWINCH, CGame::HandleResize);

    CGame::PrintDebug("ncurses initialised");
}

void CGame::EndCurses() {
    endwin();
}

CGame::multiplayer_mode CGame::GetMPGameMode() {
    return CGame::m_mp_mode;
}

CGame::game_state CGame::GetGameState() {
    switch (CGame::m_mp_mode) {
        case CGame::multiplayer_mode::OFFLINE:
            if (CGame::GetWorld().GetPlayer() == NULL)
                return CGame::game_state::HALTED;
            if (CGame::GetWorld().GetPlayer()->IsAlive()) {
                CWorld::eliving_pointer e = NULL;
                for (auto i = CGame::GetWorld().GetEntities().begin(); i != CGame::GetWorld().GetEntities().end(); ++i)
                    if ((e = dynamic_pointer_cast<CLivingEntity>(*i)) && *i != CGame::GetWorld().GetPlayer() && e->IsAlive())
                        return CGame::game_state::ONGOING;
                return CGame::game_state::WIN_LOCAL;
            } else
                return CGame::game_state::WIN_OTHER;
            break;
        case CGame::multiplayer_mode::SERVER:
            if (CGame::GetWorld().GetPlayer() == NULL || CGame::GetWorld().GetNetworkedEntity() == NULL)
                return CGame::game_state::HALTED;
            if (CGame::GetWorld().GetPlayer()->IsAlive() && CGame::GetWorld().GetNetworkedEntity()->IsAlive())
                return CGame::game_state::ONGOING;
            else if (CGame::GetWorld().GetPlayer()->IsAlive())
                return CGame::game_state::WIN_LOCAL;
            else
                return CGame::game_state::WIN_OTHER;
            break;
        default:
            return CGame::game_state::HALTED;
            break;
    }
}

void CGame::InitErrorWindow() {
    CGame::errpan.second = newwin(3, 32, 0, 0);

    CGame::errpan.first = new_panel(CGame::errpan.second);
    hide_panel(CGame::errpan.first);
    update_panels();
}

void CGame::InitDebugWindow() {
    int h, w;
    getmaxyx(stdscr, h, w);
    wattron(CGame::debugpan.second, COLOR_PAIR(2));
    CGame::debugpan.second = newwin(h, 48, 0, w - 48);

    CGame::debugpan.first = new_panel(CGame::debugpan.second);
    wclear(CGame::debugpan.second);
    show_panel(CGame::debugpan.first);
    waddstr(CGame::debugpan.second, "Debug:");
    waddstr(CGame::debugpan.second, " off");
    if (!CGame::DEBUG)
        hide_panel(CGame::debugpan.first);
    update_panels();
    doupdate();
    refresh();
}

void CGame::PrintDebug(const string & str) {
    if (CGame::DEBUG) {
        int h, w;
        getmaxyx(CGame::debugpan.second, h, w);

        wattron(CGame::debugpan.second, COLOR_PAIR(2));
        m_debugWindowContent.push_front(str);
        if (m_debugWindowContent.size() > (unsigned) (h - 2))
            m_debugWindowContent.resize(h - 2);

        wclear(CGame::debugpan.second);
        wbkgd(CGame::debugpan.second, COLOR_PAIR(2));
        int j = 0;
        for (auto i = m_debugWindowContent.rbegin(); i != m_debugWindowContent.rend(); ++i) {
            wmove(CGame::debugpan.second, ++j, 1);
            waddnstr(CGame::debugpan.second, i->c_str(), w - 2);
        }
        box(CGame::debugpan.second, 0, 0);
        wmove(CGame::debugpan.second, 0, 4);
        waddstr(CGame::debugpan.second, "Debug output");

        wrefresh(CGame::debugpan.second);
        wattroff(CGame::debugpan.second, COLOR_PAIR(2));
        update_panels();
        doupdate();
        refresh();
    }
}

void CGame::HandleResize(int) {
    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT) {
        CNetwork::SendAction('h');
        CNetwork::CloseConnection();
        return;
    }
    usleep(100000);
    int h, w;
    endwin();
    refresh();
    wclear(CGame::errpan.second);
    wclear(stdscr);
    getmaxyx(stdscr, h, w);
    resizeterm(h, w);

    if (h < CGame::minScreenSize().second || w < CGame::minScreenSize().first) {
        // Draw error graphics
        wmove(CGame::errpan.second, 0, 0);
        wattron(CGame::errpan.second, COLOR_PAIR(1));
        for (int i = 0; i < 3; i++) {
            wmove(CGame::errpan.second, i, 0);
            for (int j = 0; j < 32; j++)
                waddch(CGame::errpan.second, ACS_CKBOARD);
        }
        wmove(CGame::errpan.second, 1, 2);
        waddstr(CGame::errpan.second, "Terminal window is too small");

        show_panel(CGame::errpan.first);
    } else {
        hide_panel(CGame::errpan.first);
        if (CGame::GetMPGameMode() != CGame::multiplayer_mode::CLIENT)
            CGame::Draw();
    }

    move_panel(CGame::debugpan.first, 0, w - 48);
    CGame::PrintDebug("Resized");

    update_panels();
    doupdate();
    refresh();
}

void CGame::SetNetworkAddress(const string & addr) {
    m_serverAddr = addr;
}

void CGame::PrintEndScreen(bool winner) {
    wclear(stdscr);
    wmove(stdscr, 1, 1);
    waddstr(stdscr, "You have ");
    if (winner)
        waddstr(stdscr, "won");
    else
        waddstr(stdscr, "lost");
    waddstr(stdscr, " the match!");
    wrefresh(stdscr);

    sleep(3);
}

void CGame::StartGameLoop() {
    if (CGame::m_mp_mode == CGame::multiplayer_mode::CLIENT)
        CNetwork::InitClient(m_serverAddr.c_str());
    else if (CGame::m_mp_mode == CGame::multiplayer_mode::SERVER)
        CNetwork::InitServer(m_serverAddr.c_str());
    if (CGame::DEBUG) {
        CCoord::screen_coord_type c1 = CCoord(CCoord::WIDTH - 1, CCoord::HEIGHT - 1).ScreenCoord();
        CCoord::screen_coord_type c2 = CCoord(0, 0).ScreenCoord();
        CGame::PrintDebug("Map screen size: " + to_string(c1.first - c2.first) + ", " + to_string(c1.second - c2.second));
    }
    int lastKey = 0;
    int frameSkip = 0;
    bool quit = false;
    while (1) {
        struct timeval time;
        gettimeofday(&time, NULL);
        int totalTime = (time.tv_sec * 1000000) + time.tv_usec;
        int key = getch();
        gettimeofday(&time, NULL);
        totalTime = (((time.tv_sec * 1000000) + time.tv_usec) - totalTime);

        if (frameSkip < CGame::FPS / 2 && lastKey == key && totalTime < (1000000 / CGame::FPS) / 10) {
            frameSkip++;
            lastKey = key;
            continue;
        }
        frameSkip = 0;
        lastKey = key;

        if (key != ERR) {
            totalTime = (1000000 / CGame::FPS) - totalTime;
            if (totalTime > 0)
                usleep(totalTime);

            CWorld::ebomber_pointer p = CGame::GetWorld().GetPlayer();
            switch (key) {
                case KEY_RESIZE:
                case KEY_MOUSE:
                    usleep(100000);
                    continue;
                    break;
                case 'p':
                    // pause menu
                    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT) {
                        quit = CNetwork::SendAction('h');
                        CNetwork::CloseConnection();
                        //CGame::PrintEndScreen(false);
                        //quit = true;
                    } else
                        CGame::PrintEndScreen(false);
                    quit = true;
                    break;
                case KEY_UP:
                case 'w':
                case 'k':
                    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT)
                        quit = CNetwork::SendAction('a');
                    else
                        p->Move(CCoord::direction::UP);
                    break;
                case KEY_DOWN:
                case 's':
                case 'j':
                    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT)
                        quit = CNetwork::SendAction('b');
                    else
                        p->Move(CCoord::direction::DOWN);
                    break;
                case KEY_LEFT:
                case 'a':
                case 'h':
                    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT)
                        quit = CNetwork::SendAction('c');
                    else
                        p->Move(CCoord::direction::LEFT);
                    break;
                case KEY_RIGHT:
                case 'd':
                case 'l':
                    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT)
                        quit = CNetwork::SendAction('d');
                    else
                        p->Move(CCoord::direction::RIGHT);
                    break;
                case ' ':
                case 'e':
                case 'o':
                    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT)
                        quit = CNetwork::SendAction('e');
                    else
                        p->PlaceBomb();
                    break;
                case '0':
                case 'm':
                case 'q':
                case 'i':
                    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT)
                        quit = CNetwork::SendAction('f');
                    else
                        p->DetonateOwnedBombs();
                    break;
                default:
                    PrintDebug("Key not recognized");
                    break;
            }
        } else if (CGame::GetMPGameMode() == CGame::multiplayer_mode::CLIENT) {
            quit = CNetwork::SendAction('g');
        }

        if (quit) {
            return;
        }

        // client already did its job; this is only server/offline work
        if (CGame::GetMPGameMode() != CGame::multiplayer_mode::CLIENT) {
            CGame::GetWorld().Tick();
            CGame::Draw();
            /**
             * @todo the client gets a one tick delay here; it should be fixed
             */
            if (CGame::GetMPGameMode() == CGame::multiplayer_mode::SERVER)
                CNetwork::ServeClient();

            if (CGame::GetGameState() != CGame::game_state::ONGOING)
                break;

            int h, w;
            getmaxyx(stdscr, h, w);
            while (h < CGame::minScreenSize().second || w < CGame::minScreenSize().first) {
                CGame::PrintDebug("Game paused; resize window");
                sleep(1);
                getmaxyx(stdscr, h, w);
            }
        }
    }

    // non-client mode endscreen
    if (CGame::GetMPGameMode() != CGame::multiplayer_mode::CLIENT) {
        CGame::PrintEndScreen(CGame::GetGameState() == CGame::game_state::WIN_LOCAL);
    }

}
