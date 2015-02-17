#ifndef TILEDRAWING_H
#define	TILEDRAWING_H

#include <cstdlib>
#include <ncurses.h>

using namespace std;

class CTile {
public:
    /**
     * Tile width
     */
    static const int WIDTH = 3;

    /**
     * Tile weight
     */
    static const int HEIGHT = 2;

    /**
     * Tile area (deduced from width and height)
     */
    static const int TILE_AREA = WIDTH * HEIGHT;

    /**
     * Create a new tile (optionally made of one character)
     *
     * @param character optional character to fill in at all positions
     */
    CTile(const chtype = ' ');

    /**
     * Create a new tile with all characters set
     *
     * @param c1..c6 characters the tile is made of (left to right, top to bottom)
     */
    CTile(const chtype, const chtype, const chtype, const chtype, const chtype, const chtype);

    virtual ~CTile();

    /**
     * Draws a tile at current position
     */
    void Draw() const;

    /**
     * Set every character of the tile
     *
     * @param c1..c6 characters the tile is made of (left to right, top to bottom)
     */
    void Set(const chtype, const chtype, const chtype, const chtype, const chtype, const chtype);

    chtype& operator[] (int);
private:
    chtype m_tiles[TILE_AREA];
};

#endif	/* TILEDRAWING_H */

