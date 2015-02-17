/* 
 * File:   CScreen.h
 * Author: amunak
 *
 * Created on May 18, 2014, 9:05 PM
 */

#ifndef CSCREEN_H
#define	CSCREEN_H

#include "CCoord.h"

/**
 * Simplifies access to screen and its drawing
 */
class CScreen {
public:
    CScreen();
    CScreen(const CScreen& orig);
    virtual ~CScreen();
    
    /**
     * Returns the minimal required size needed for the game to work
     * @return 
     */
    CCoord::screen_coord_type GetMinSize();
    
    /**
     * Assembles the whole game screen and draws it
     */
    void Draw();
private:

};

#endif	/* CSCREEN_H */

