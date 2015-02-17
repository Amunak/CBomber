#include "CPickup.h"

CPickup::CPickup(const CCoord & loc, const pickup_type type) :
CEntity(loc, "Pickup", CTile('|'), false), m_type(type) {
    GetTile()[4] = '+';
    if (type == CPickup::pickup_type::NONE) {
        unsigned short int val = rand() % 7;
        if (val < 1) {
            m_type = pickup_type::REMOTE;
            GetTile()[1] = 'R';
            GetTile()[4] = 'D';
        } else if (val < 2) {
            m_type = pickup_type::HEALTH;
            GetTile()[1] = 'H';
        } else if (val < 3) {
            m_type = pickup_type::SPEED;
            GetTile()[1] = 'S';
        } else if (val < 5) {
            m_type = pickup_type::BOMB;
            GetTile()[1] = 'B';
        } else if (val < 8) {
            m_type = pickup_type::FLAME;
            GetTile()[1] = 'F';
        }
    }
    if (m_type == pickup_type::NONE)
        SetRemoveFlag();
}

CPickup::pickup_type CPickup::GetType() {
    return m_type;
}

bool CPickup::Tick() {
    return false;
}
