/**
 * @description Bomberman-like ncurses game
 * @author Jiří Barouš (aka Amunak) [<a href="http://amunak.net">homepage</a>]
 */

/*!
 * \mainpage CBomber
 *
 * \section i_links Important links
 * <ul>
 *   <li><a href="inherits.html">Class hierarchy graph</a>
 *   <li><a href="class_c_game.html">CGame game class</a>
 * </ul>
 * 
 * \section a_info Additional information
 * Network code (methods CNetwork::openSrvSocket and CNetwork::openCliSocket)
 * created by Ladislav Vagner. Source: https://edux.fit.cvut.cz/courses/BI-PA2/_media/networking-2014.tgz
 */

#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <menu.h>

#include "CGame.h"
#include "CNetwork.h"

using namespace std;

/**
 * Prints the usage info to stderr
 */
void usage(const char* prgName) {
    cerr << "USAGE: " << prgName << "[OPTION] [MODE [ADDRESS]]" << endl;
    cerr << endl;
    cerr << "DESCRIPTION: " << endl;
    cerr << "\t          A bomberman-like ncurses game." << endl;
    cerr << endl;
    cerr << "OPTIONS:" << endl;
    cerr << "\t-h,       display this help and exit" << endl;
    cerr << "\t--help" << endl;
    //cerr << "\t-m, --map[=MAP_FILE]\tload a map from MAP_FILE or use the map selection menu" << endl;
    cerr << endl;
    cerr << "MODE:" << endl;
    cerr << "\tOFFLINE - this is the default value; provides play versus AI" << endl;
    cerr << "\tSERVER  - starts the game in server mode waiting for connection" << endl;
    cerr << "\tCLIENT  - starts the game in client mode; connects to server" << endl;
    cerr << endl;
    cerr << "ADDRESS:" << endl;
    cerr << "\t          specifies a server to connect to (or server bind" << endl;
    cerr << "\t          \taddress) in online play" << endl;
    cerr << "\t          defaults to ip6-localhost" << endl;
    cerr << "\t          can be any valid hostname or address" << endl;
}

/**
 * Processess parameters, presents map selection menu, initializes environment and starts the game
 *
 * @param argc argument count
 * @param argv argument list
 * @return error code
 */
int main(int argc, char** argv) {

    /**
     * The directory used to search for maps
     */
    const string mapDirectory = "examples/";

    if (argc > 4) {
        cerr << argv[0] << ": too many arguments" << endl;
        cerr << argv[0] << ": Try '" << argv[0] << " --help' for more information." << endl;
        return 1;
    } else if (argc == 1) {
        CGame::InitGame();
    } else if (argc > 1 && (string(argv[1]) == "-h" || string(argv[1]) == "--help")) {
        usage(argv[0]);
        return 0;
    } else {
        if (string(argv[1]) == "offline" || string(argv[1]) == "OFFLINE")
            CGame::InitGame();
        else if (string(argv[1]) == "client" || string(argv[1]) == "CLIENT")
            CGame::InitGame(CGame::multiplayer_mode::CLIENT);
        else if (string(argv[1]) == "server" || string(argv[1]) == "SERVER")
            CGame::InitGame(CGame::multiplayer_mode::SERVER);
        else {
            cerr << argv[0] << ": invalid argument(s)" << endl;
            cerr << argv[0] << ": Try '" << argv[0] << " --help' for more information." << endl;
            return 2;
        }
    }

    if (argc < 3)
        CGame::SetNetworkAddress("ip6-localhost");
    else
        CGame::SetNetworkAddress(string(argv[2]));

    CGame::InitCurses();

    if (CGame::GetMPGameMode() != CGame::multiplayer_mode::CLIENT) {
        vector<string> mapmenu;
        mapmenu.push_back("Classic");
        mapmenu.push_back("Empty");
        DIR * dir;
        struct dirent *dirent;

        if ((dir = opendir(mapDirectory.c_str())) != NULL) {
            while ((dirent = readdir(dir)) != NULL) {
                string dname(dirent->d_name);
                if (dname.find(".bmap\0") != string::npos)
                    mapmenu.push_back(string(dirent->d_name));
            }
            closedir(dir);
        } else {
            CGame::EndCurses();
            cerr << "maps directory could not be read from" << endl;
            return 3;
        }

        ITEM **my_items;
        int c;
        MENU *my_menu;
        int n_choices, i;
        int cur_sel = 0;

        n_choices = mapmenu.size();
        my_items = (ITEM **) calloc(n_choices + 1, sizeof (ITEM *));

        for (i = 0; i < n_choices; ++i)
            my_items[i] = new_item(mapmenu[i].c_str(), "");
        my_items[n_choices] = (ITEM *) NULL;

        my_menu = new_menu((ITEM **) my_items);
        mvprintw(LINES - 5, 1, "Welcome to CBomber (see-bomber)");
        mvprintw(LINES - 3, 1, "Please select a map with arrow keys and press space/e/p to start the game.");
        mvprintw(LINES - 2, 1, "Control the game with wasd/lkhj/arrow keys; place bombs with o/e/space; p to exit");
        mvprintw(LINES - 1, 1, "Bonuses: F: flame, S: speed, B: bombs, H: health, RD: remote detonator (use with i/q/0)");
        move(1, 2);
        post_menu(my_menu);
        refresh();

        while ((c = getch()) != ' ' && c != 'e' && c != 'o') {
            switch (c) {
                case KEY_DOWN:
                    if (menu_driver(my_menu, REQ_DOWN_ITEM) == E_OK)
                        cur_sel++;
                    break;
                case KEY_UP:
                    if (menu_driver(my_menu, REQ_UP_ITEM) == E_OK)
                        cur_sel--;
                    break;
            }
        }

        for (i = 0; i < n_choices; ++i)
            free_item(my_items[i]);
        free_menu(my_menu);

        if (cur_sel == 0)
            CGame::GetWorld().LoadRetroWorld();
        else if (cur_sel == 1) {
            CGame::GetWorld().LoadPickupArenaWorld();
        } else {
            CGame::GetWorld().LoadFromFile(mapDirectory + mapmenu.at(cur_sel));
        }
        clear();
        CGame::Draw();
    }

    try {
        CGame::StartGameLoop();

        CGame::EndCurses();
    } catch (CNetworkException e) {

        CGame::EndCurses();
        cout << "Network exception." << endl;
    }

    return 0;
}

