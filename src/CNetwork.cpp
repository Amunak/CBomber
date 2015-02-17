#include "CNetwork.h"
#include "CGame.h"

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>

using namespace std;

int CNetwork::m_fileDescriptor = -1;
int CNetwork::m_dataFileDescriptor = -1;
int CNetwork::m_skippedDT = 0;

CNetwork::CNetwork() {
}

void CNetwork::InitServer(const char* name) {
    if ((m_fileDescriptor = CNetwork::openSrvSocket(name, CNetwork::PORT)) == -1)
        throw CNetworkException();

    struct sockaddr remote;
    socklen_t remoteLen = sizeof ( remote);
    wclear(stdscr);
    wmove(stdscr, 1, 1);
    waddstr(stdscr, "Waiting for client to connect...");
    wrefresh(stdscr);

    CNetwork::m_dataFileDescriptor = accept(m_fileDescriptor, &remote, &remoteLen);
    clear();
}

void CNetwork::InitClient(const char* name) {
    if ((m_fileDescriptor = CNetwork::openCliSocket(name, CNetwork::PORT)) == -1)
        throw CNetworkException();
    signal(SIGPIPE, SIG_IGN);
}

void CNetwork::CloseConnection() {
    close(m_fileDescriptor);
    if (CNetwork::m_dataFileDescriptor>-1)
        close(CNetwork::m_dataFileDescriptor);
}

void CNetwork::ServeClient() {
    CNetwork::m_skippedDT++;
    if (CNetwork::m_skippedDT > 100) {
        // client DC; we win
        CGame::GetWorld().GetNetworkedEntity()->SetHealth(0);
        return;
    }

    char receiveBuffer[1];
    CGame::PrintDebug("Receiving data");
    while (1) {
        fd_set fd;
        FD_ZERO(&fd);
        FD_SET(m_dataFileDescriptor, &fd);
        struct timeval tv = {0, 50};

        int res = select(CNetwork::m_dataFileDescriptor + 1, &fd, NULL, NULL, &tv);
        // no data?
        if (res == 0) {
            break;
        } else if (res < 0) {
            return;
        }
        if (FD_ISSET(m_dataFileDescriptor, &fd)) {
            read(CNetwork::m_dataFileDescriptor, receiveBuffer, sizeof (receiveBuffer));

            switch (receiveBuffer[0]) {
                case 'a':
                    CGame::GetWorld().GetNetworkedEntity()->Move(CCoord::direction::UP);
                    break;
                case 'b':
                    CGame::GetWorld().GetNetworkedEntity()->Move(CCoord::direction::DOWN);
                    break;
                case 'c':
                    CGame::GetWorld().GetNetworkedEntity()->Move(CCoord::direction::LEFT);
                    break;
                case 'd':
                    CGame::GetWorld().GetNetworkedEntity()->Move(CCoord::direction::RIGHT);
                    break;
                case 'e':
                    CGame::GetWorld().GetNetworkedEntity()->PlaceBomb();
                    break;
                case 'f':
                    CGame::GetWorld().GetNetworkedEntity()->DetonateOwnedBombs();
                    break;
                case 'g':
                    // client did nothing; just send him updated screen
                    break;
                case 'h':
                    // client quit; we win
                    CGame::GetWorld().GetNetworkedEntity()->SetHealth(0);
                    return;
                    break;
            }
        } else
            break;
    }

    // assemble data for client and send them
    char sendBuffer[CNetwork::BUFFER_SIZE];
    unsigned int bufptr = 0;
    sendBuffer[bufptr++] = static_cast<char> (CGame::GetGameState());
    sendBuffer[bufptr++] = CGame::GetWorld().GetNetworkedEntity()->GetHealth();
    sendBuffer[bufptr++] = CGame::GetWorld().GetNetworkedEntity()->GetBombs();
    sendBuffer[bufptr++] = CGame::GetWorld().GetNetworkedEntity()->GetFlameSize();
    for (int i = 0; i < CCoord::HEIGHT * CTile::HEIGHT; i++) {
        chtype chstr[CCoord::WIDTH * CTile::WIDTH + 2];
        wmove(stdscr, i + CWorld::DRAWOFFSET_TOP, CWorld::DRAWOFFSET_LEFT);
        winchnstr(stdscr, chstr, CCoord::WIDTH * CTile::WIDTH);
        for (int j = 0; j < CCoord::WIDTH * CTile::WIDTH + 1; j++) {
            if (chstr[j] == ACS_CKBOARD)
                sendBuffer[bufptr] = 'a';
            else if (chstr[j] == ACS_ULCORNER)
                sendBuffer[bufptr] = 'b';
            else if (chstr[j] == ACS_URCORNER)
                sendBuffer[bufptr] = 'c';
            else if (chstr[j] == ACS_LLCORNER)
                sendBuffer[bufptr] = 'd';
            else if (chstr[j] == ACS_LRCORNER)
                sendBuffer[bufptr] = 'e';
            else if (chstr[j] == ACS_HLINE)
                sendBuffer[bufptr] = 'f';
            else
                sendBuffer[bufptr] = chstr[j] & A_CHARTEXT;

            bufptr++;
            if (bufptr > CNetwork::BUFFER_SIZE - 2)
                CGame::PrintDebug("DAMN " + to_string(bufptr) + " : " + to_string(i) + ", " + to_string(j));
        }
    }
    sendBuffer[bufptr++] = '\0';
    //CGame::PrintDebug("bufptr is at " + to_string(bufptr));
    //usleep(50000); //this fixes stuff. that is sad

    if (write(CNetwork::m_dataFileDescriptor, sendBuffer, bufptr) < 0)
        return;
    CNetwork::m_skippedDT = 0;

}

bool CNetwork::SendAction(const char action) {
    char sendBuffer[1];
    sendBuffer[0] = action;
    int e = write(m_fileDescriptor, sendBuffer, sizeof (sendBuffer));
    if (e < 0) {
        // network error; we call that a win
        CGame::PrintEndScreen(true);
        return true;
    }
    char receiveBuffer[CNetwork::BUFFER_SIZE];

    read(m_fileDescriptor, receiveBuffer, sizeof (receiveBuffer));
    /*if (l < CNetwork::BUFFER_SIZE / 2 + 1) {
        CGame::PrintDebug("RECEIVE failed with " + to_string(l));
        return true;
    }*/
    unsigned int bufptr = 0;
    if (receiveBuffer[bufptr] == static_cast<char> (CGame::game_state::ONGOING)) {
        bufptr++;
        wmove(stdscr, 0, 0);
        waddstr(stdscr, string("Health: " + to_string((unsigned int) receiveBuffer[bufptr++])).c_str());
        waddstr(stdscr, string(" Bombs: " + to_string((unsigned int) receiveBuffer[bufptr++])).c_str());
        waddstr(stdscr, string(" Flame: " + to_string((unsigned int) receiveBuffer[bufptr++])).c_str());

        for (int i = 0; i < CCoord::HEIGHT * CTile::HEIGHT; i++) {
            for (int j = 0; j < CCoord::WIDTH * CTile::WIDTH + 1; j++) {
                wmove(stdscr, i + CWorld::DRAWOFFSET_TOP, j + CWorld::DRAWOFFSET_LEFT);

                if (receiveBuffer[bufptr] == 'a')
                    waddch(stdscr, ACS_CKBOARD);
                else if (receiveBuffer[bufptr] == 'b')
                    waddch(stdscr, ACS_ULCORNER);
                else if (receiveBuffer[bufptr] == 'c')
                    waddch(stdscr, ACS_URCORNER);
                else if (receiveBuffer[bufptr] == 'd')
                    waddch(stdscr, ACS_LLCORNER);
                else if (receiveBuffer[bufptr] == 'e')
                    waddch(stdscr, ACS_LRCORNER);
                else if (receiveBuffer[bufptr] == 'f')
                    waddch(stdscr, ACS_HLINE);
                else
                    waddnstr(stdscr, receiveBuffer + bufptr, 1);
                bufptr++;
            }
        }
        wrefresh(stdscr);
        return false;
    } else {
        if (receiveBuffer[bufptr] == static_cast<char> (CGame::game_state::WIN_LOCAL))
            CGame::PrintEndScreen(false);
        else
            CGame::PrintEndScreen(true);
        wrefresh(stdscr);
        return true;
    }

}

/**
 * Network code by Ladislav Vagner,
 * available at: https://edux.fit.cvut.cz/courses/BI-PA2/_media/networking-2014.tgz
 */
int CNetwork::openSrvSocket(const char* name, int port) {
    struct addrinfo * ai;
    char portStr[10];

    /* Adresa, kde server posloucha. Podle name se urci typ adresy
     * (IPv4/6) a jeji binarni podoba
     */
    snprintf(portStr, sizeof ( portStr), "%d", port);
    if (int ec = getaddrinfo(name, portStr, NULL, &ai)) {
        cerr << name << " : " << portStr << " - " << gai_strerror(ec) << endl;
        printf("addrinfo\n");
        return -1;
    }
    /* Otevreni soketu, typ soketu (family) podle navratove hodnoty getaddrinfo,
     * stream = TCP
     */
    int fd = socket(ai -> ai_family, SOCK_STREAM, 0);
    if (fd == -1) {
        freeaddrinfo(ai);
        printf("socket\n");
        return -1;
    }

    /* napojeni soketu na zadane sitove rozhrani
     */
    if (bind(fd, ai -> ai_addr, ai -> ai_addrlen) == -1) {
        close(fd);
        freeaddrinfo(ai);
        printf("bind\n");
        return -1;
    }
    freeaddrinfo(ai);
    /* prepnuti soketu na rezim naslouchani (tedy tento soket nebude vyrizovat
     * datovou komunikaci, budou po nem pouze chodit pozadavky na pripojeni.
     * 10 je max velikost fronty cekajicich pozadavku na pripojeni.
     */
    if (listen(fd, 10) == -1) {
        close(fd);
        printf("listen\n");

        return -1;
    }
    return fd;
}

/**
 * Network code by Ladislav Vagner,
 * available at: https://edux.fit.cvut.cz/courses/BI-PA2/_media/networking-2014.tgz
 */
int CNetwork::openCliSocket(const char* name, int port) {
    struct addrinfo * ai;
    char portStr[10];

    /* Adresa, kde server posloucha. Podle name se urci typ adresy
     * (IPv4/6) a jeji binarni podoba
     */
    snprintf(portStr, sizeof ( portStr), "%d", port);
    if (getaddrinfo(name, portStr, NULL, &ai)) {
        printf("addrinfo\n");
        return -1;
    }
    /* Otevreni soketu, typ soketu (family) podle navratove hodnoty getaddrinfo,
     * stream = TCP
     */
    int fd = socket(ai -> ai_family, SOCK_STREAM, 0);
    if (fd == -1) {
        freeaddrinfo(ai);
        printf("socket\n");
        return -1;
    }
    /* Zadost o spojeni se serverem (ted se teprve zacne komunikovat).
     * vysledkem je bud otevrene datove spojeni nebo chyba.
     */
    if (connect(fd, ai -> ai_addr, ai -> ai_addrlen) == -1) {
        close(fd);
        freeaddrinfo(ai);
        printf("connect\n");
        return -1;
    }
    freeaddrinfo(ai);

    return fd;
}

const char* CNetworkException::what() const noexcept {
    return "Network bad.";
}