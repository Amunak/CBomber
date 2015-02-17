#include <ncurses.h>

#include "CEntity.h"
#include "CGame.h"
#include "CCoord.h"

using namespace std;

CEntity::CEntity(const CCoord & loc, const string & name, const CTile & tile, bool collide) :
m_name(name), m_tile(tile), m_position(loc), m_collision(collide), m_remove(false) {
    if (m_collision) {
        // clear ground under entity if it has collision
        CGame::GetWorld().SetBlock(CCoord(loc), CBlock());
    }
}

bool CEntity::Move(const CCoord::direction dir) {
    CCoord newLoc(m_position);
    newLoc.Move(dir);

    if (CanOccupyLocation(newLoc)) {
        Jump(newLoc);
        return true;
    } else
        return false;
}

void CEntity::Draw() {
    move(GetPosition().ScreenCoord().second, GetPosition().ScreenCoord().first);
    m_tile.Draw();
}

void CEntity::Jump(const CCoord & loc) {
    m_position = loc;
}

bool CEntity::CanOccupyLocation(const CCoord & loc) const {
    if (!m_collision)
        return true;
    return !CGame::GetWorld().IsLocationOccupied(loc);
}

CCoord CEntity::GetPosition() const {
    return m_position;
}

bool CEntity::HasCollision() const {
    return m_collision;
}

void CEntity::SetCollision(bool cl) {
    m_collision = cl;
}

string CEntity::GetName() const {
    return m_name;
}

CTile& CEntity::GetTile() {
    return m_tile;
}

void CEntity::SetRemoveFlag() {
    m_remove = true;
}

bool CEntity::GetRemoveFlag() {
    return m_remove;
}


