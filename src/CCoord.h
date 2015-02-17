#ifndef CCOORD_H
#define	CCOORD_H

#include <utility>
#include <string>

using namespace std;

/**
 * Used to store and manipulate world coordinates
 */
class CCoord {
public:
    /**
     * Type representing either horizontal or vertical position in the world
     */
    typedef short int pos_type;

    /**
     * Type representing screen coordinates
     */
    typedef pair<int, int> screen_coord_type;

    /**
     * Type representing the coordinates
     */
    typedef pair<pos_type, pos_type> value_type;

    /**
     * Enumeration of constants representing movement directions
     */
    enum class direction {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3,
        NONE = 4
    };

    /**
     * World width in blocks
     */
    const static pos_type WIDTH = 15;

    /**
     * World height in blocks
     */
    const static pos_type HEIGHT = 13;

    /**
     * Construct empty coordinates
     */
    CCoord();

    /**
     * Construct coordinates from values
     * @param x horizontal position
     * @param y vertical position
     */
    CCoord(const pos_type &, const pos_type &);

    /**
     * Construct coordinates from existing coordinates
     * @param coordinates to copy
     */
    CCoord(const value_type &);

    virtual ~CCoord();


    /**
     * Returns maximum possible (or minimum needed) screen coords to draw the whole screen
     * @return screen coords
     */
    static screen_coord_type MaxScreenCoords();

    /**
     * Returns the coordinates as screen coordinates
     * @return screen coordinates
     */
    screen_coord_type ScreenCoord() const;

    friend bool operator==(const CCoord &, const CCoord &);

    /**
     * Converts CCoord to string
     * @return stringified CCoord
     */
    friend string to_string(const CCoord &);

    /**
     * @return the X (horizontal) coordinate
     */
    pos_type GetX() const;

    /**
     * @return the Y (vertical) coordinate
     */
    pos_type GetY() const;

    /**
     * Sets the X (horizontal) coordinate
     * @param x new X value
     * @return true on overflow
     */
    bool SetX(const pos_type &);

    /**
     * Sets the Y (vertical) coordinate
     * @param y new Y value
     * @return true on overflow
     */
    bool SetY(const pos_type &);

    /**
     * Move the coordinates in direction dir
     * @param dir direction to take
     */
    void Move(const direction);

private:
    pos_type m_x;
    pos_type m_y;
};

#endif	/* CCOORD_H */

