#include "Blocks.h"
#include "CTile.h"

CBlock::CBlock() : m_destructible(false), m_passable(true), m_tile(CTile()) {
}

CBlock::CBlock(const CTile & tile, const bool destr, const bool pass) :
m_destructible(destr), m_passable(pass), m_tile(tile) {
}

void CBlock::Draw() const {
    m_tile.Draw();
}

bool CBlock::IsDestructible() const {
    return m_destructible;
}

bool CBlock::IsPassable() const {
    return m_passable;
}

CBedrockBlock::CBedrockBlock(const CTile & tile) :
CBlock(tile, false, false) {
}

CCrateBlock::CCrateBlock(const CTile & tile) :
CBlock(tile, true, false) {
}
