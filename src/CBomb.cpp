#include <algorithm>

#include "CBomb.h"
#include "CGame.h"

CBomb::CBomb(CBomberman* owner) :
CEntity(owner->GetPosition(), "Bomb", CTile('B', 'O', 'M', '<', '5', '>'), true), m_owner(owner), m_flameSize(owner->GetFlameSize()), m_detonationTime(owner->HasRemoteDetonation() ? 250 : 50) {
    if (m_detonationTime > 99)
        GetTile()[4] = '+';
}

bool CBomb::Tick() {
    if (m_detonationTime > 200)
        return false;

    if (--m_detonationTime < 1) {
        Detonate();
        return true;
    }

    if (m_detonationTime % 10 == 0) {
        GetTile()[4] = (m_detonationTime < 100) ? to_string(m_detonationTime / 10).at(0) : '+';
        return true;
    }

    return false;
}

void CBomb::Detonate() {
    m_detonationTime = 0;
    CGame::PrintDebug("Bomb detonated!");
    m_owner->AddBomb();

    PlaceFlame(GetPosition());
    CCoord c;
    for (unsigned short int j = 0; j < 4; j++) {
        c = GetPosition();
        for (int i = 0; i < m_flameSize; i++) {
            c.Move(static_cast<CCoord::direction> (j));
            // chain detonation
            shared_ptr<CBomb> bomb = NULL;
            for (auto i = CGame::GetWorld().GetEntities().rbegin(); i != CGame::GetWorld().GetEntities().rend(); ++i) {
                if ((bomb = dynamic_pointer_cast<CBomb>(*i)) && bomb->GetPosition() == c && bomb->m_detonationTime > 0) {
                    bomb->Detonate();
                    break;
                }
            }

            const CBlock & b = CGame::GetWorld().GetBlock(c);
            if (b.IsPassable())
                PlaceFlame(c);
            else if (b.IsDestructible()) {
                PlaceFlame(c);
                CGame::GetWorld().SetBlock(c, CBlock());
                // chance to spawn a pickup
                if (rand() % 100 > 62)
                    CGame::GetWorld().AddEntity(CWorld::entity_pointer(new CPickup(c)));
                break;
            } else {
                break;
            }
        }
    }

    SetRemoveFlag();
}

void CBomb::PlaceFlame(const CCoord & loc) const {
    CGame::GetWorld().SetBlock(loc, CBlock());
    CGame::GetWorld().AddEntity(CWorld::entity_pointer(new CBombFlame(loc)));
    CWorld::eliving_pointer e = NULL;

    for (auto i = CGame::GetWorld().GetEntities().begin(); i != CGame::GetWorld().GetEntities().end(); ++i)
        if ((e = dynamic_pointer_cast<CLivingEntity>(*i)) && e->GetPosition() == loc)
            e->ModifyHealth(-1);
}

CBombFlame::CBombFlame(const CCoord & loc) :
CEntity(loc, "Flame", CTile('X'), false), m_extinguishTime(22) {
    RandomizeAppearance();
}

CBomberman* CBomb::GetOwner() {
    return m_owner;
}

/**
 * @todo implement
 */
void CBombFlame::RandomizeAppearance() {
    for (short unsigned int i = 0; i < 6; i++)
        GetTile()[i] = (rand() % 2 == 0) ? 'X' : 'x';
}

bool CBombFlame::Tick() {
    if (--m_extinguishTime < 1) {
        SetRemoveFlag();
        return true;
    }

    if (m_extinguishTime % 2 == 0) {
        RandomizeAppearance();
        return true;
    }
    return false;
}
