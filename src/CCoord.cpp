#include <string>

#include "CCoord.h"
#include "CTile.h"
#include "CGame.h"

const CCoord::pos_type CCoord::WIDTH;
const CCoord::pos_type CCoord::HEIGHT;

string to_string(const CCoord & c) {
    string s("(");
    s.append(to_string(c.GetX()));
    s.append(", ");
    s.append(to_string(c.GetY()));
    s.append(")");
    return s;
}

CCoord::CCoord() :
CCoord(0, 0) {
}

CCoord::CCoord(const pos_type & x, const pos_type & y) {
    SetX(x);
    SetY(y);
}

CCoord::CCoord(const value_type & c) :
CCoord(c.first, c.second) {

}

CCoord::~CCoord() {
}

CCoord::screen_coord_type CCoord::MaxScreenCoords() {
    return screen_coord_type(CCoord::WIDTH * CTile::WIDTH + CWorld::DRAWOFFSET_LEFT, CCoord::HEIGHT * CTile::HEIGHT + CWorld::DRAWOFFSET_TOP);
}

CCoord::screen_coord_type CCoord::ScreenCoord() const {
    return screen_coord_type(m_x * CTile::WIDTH + CWorld::DRAWOFFSET_LEFT, m_y * CTile::HEIGHT + CWorld::DRAWOFFSET_TOP);
}

void CCoord::Move(const direction dir) {
    switch (dir) {
        case direction::UP:
            SetY(GetY() - 1);
            break;
        case direction::DOWN:
            SetY(GetY() + 1);
            break;
        case direction::LEFT:
            SetX(GetX() - 1);
            break;
        case direction::RIGHT:
            SetX(GetX() + 1);
            break;
        default:
            //CGame::PrintDebug("  -> not moving");
            break;
    }
}

/**
 * @todo implement proper "wrap-around"
 */
bool CCoord::SetX(const pos_type& p) {
    pos_type x = p;
    if (x > WIDTH - 1 || x < 0) {
        return true;
    }
    m_x = x;
    return false;
}

/**
 * @todo implement proper "wrap-around"
 */
bool CCoord::SetY(const pos_type& p) {
    pos_type y = p;
    bool overflow = true;
    if (y > HEIGHT - 1 || y < 0) {
        return true;
    }
    m_y = y;
    return overflow;
}

CCoord::pos_type CCoord::GetX() const {
    return m_x;
}

CCoord::pos_type CCoord::GetY() const {
    return m_y;
}

bool operator==(const CCoord & a, const CCoord & b) {
    if (a.GetX() == b.GetX() && a.GetY() == b.GetY())
        return true;
    return false;
}
