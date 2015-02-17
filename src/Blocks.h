#ifndef CBLOCK_H
#define	CBLOCK_H

#include "CTile.h"

/**
 * Represents a world's block
 */
class CBlock {
public:
    /**
     * Create a block with default tile
     */
    CBlock();

    /**
     * Create a block with specified characteristics
     * @param tile the tile this block is made of
     * @param destructible should it be destructible?
     * @param passable should it be passable (have collision)?
     */
    CBlock(const CTile &, const bool = false, const bool = true);

    /**
     * Draws the block at current position
     */
    void Draw() const;

    /**
     * Checks if the block is destructible
     * @return true if destructible
     */
    bool IsDestructible() const;

    /**
     * Checks if the block is passable
     * @return true if passable
     */
    bool IsPassable() const;

private:
    bool m_destructible;
    bool m_passable;
    CTile m_tile;

};

class CBedrockBlock : public CBlock {
public:
    CBedrockBlock(const CTile & = CTile(ACS_CKBOARD));
};

class CCrateBlock : public CBlock {
public:
    CCrateBlock(const CTile & = CTile(ACS_ULCORNER, ACS_HLINE, ACS_URCORNER, ACS_LLCORNER, ACS_HLINE, ACS_LRCORNER));
};

#endif	/* CBLOCK_H */

