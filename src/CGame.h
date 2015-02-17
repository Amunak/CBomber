#ifndef CGAME_H
#define	CGAME_H

#include <utility>
#include <panel.h>
#include <deque>

#include "CWorld.h"
#include "CCoord.h"

/**
 * Contains current game information, states and provides methods of
 * manipulation from outside
 */
class CGame {
public:
    /**
     * Type representing a curses panel containing pointers to
     * the panel and its window
     */
    typedef pair<PANEL*, WINDOW*> panel_type;

    /**
     * Enumeration of constants representing various multi-player modes
     */
    enum class multiplayer_mode {
        OFFLINE, CLIENT, SERVER
    };

    enum class game_state : char {
        ONGOING = 'A',
        WIN_LOCAL = 'B',
        WIN_OTHER = 'C',
        HALTED = 'D'
    };

    /**
     * Desired frames per second for the game.
     * Used to determine blocking time for getch()
     */
    static const short unsigned int FPS = 20;

    /**
     * Tells whether we are in debug or regular mode
     */
    static const bool DEBUG = false;

    /**
     * Provides access to the game world
     * @return reference to current game world
     */
    static CWorld & GetWorld();

    /**
     * Counts the minimum screen space required to fit the game in
     * @return minimal required screen coordinates
     */
    static CCoord::screen_coord_type minScreenSize();

    /**
     * Starts the game loop
     */
    static void StartGameLoop();

    /**
     * (Re-)draws the whole screen
     */
    static void Draw();

    /**
     * Prepares the game
     * @param multiplayerMode sets multi-player mode for the game (defaults to offline)
     * @param seed RNG seed; 0 = random; (defaults to 0)
     * @return seed used as integer
     */
    static int InitGame(const CGame::multiplayer_mode & = CGame::multiplayer_mode::OFFLINE, unsigned int = 0);

    /**
     * Initializes the curses library
     */
    static void InitCurses();

    /**
     * Stops the curses session
     */
    static void EndCurses();

    /**
     * Prints str in the debug window
     * @param str text to be printed
     */
    static void PrintDebug(const string &);

    /**
     * Returns the type of game mode
     * @return a multiplayer mode
     */
    static multiplayer_mode GetMPGameMode();

    /**
     * Returns current game state
     * @return game state
     */
    static game_state GetGameState();
    
    /**
     * Sets the network address to connect (or bind) to
     * @param address
     */
    static void SetNetworkAddress(const string &);
    
    /**
     * Prints the winner/loser screen
     * @param winner true for winner screen
     */
    static void PrintEndScreen(bool winner);

private:
    static CWorld m_world;
    static multiplayer_mode m_mp_mode;

    /**
     * Curses panels (and their windows) used to display various stuff
     */
    static panel_type errpan, debugpan;

    /**
     * Holds contents of the debug window
     */
    static deque<string> m_debugWindowContent;
    
    /**
     * Holds the server address to bind or connect to
     */
    static string m_serverAddr;

    /**
     * This is a static class. Prevents instantiation
     */
    CGame();


    /**
     * Initializes the panels/windows
     */
    static void InitErrorWindow();
    static void InitDebugWindow();

    /**
     * Handle terminal resize
     * @param signal
     */
    static void HandleResize(int = 0);
};

#endif	/* CGAME_H */

