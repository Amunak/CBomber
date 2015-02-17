#include "LivingEntities.h"
#include "CWorld.h"
#include "CGame.h"
#include "CBomb.h"
#include "CPickup.h"

#include <vector>
#include <cstdlib>
#include <unistd.h>

using namespace std;

CLivingEntity::CLivingEntity(const CCoord & loc, const string & name, const CTile & tile, bool collide, bool AIControlled, const stat_type & moveSpeed, const stat_type & health) :
CEntity(loc, name, tile, collide), m_health(health), m_moveSpeed(moveSpeed), m_moveCooldown(0), m_AIControlled(AIControlled) {
    if (m_moveSpeed <= 0)
        m_moveSpeed = CGame::FPS / 3;
}

bool CLivingEntity::CanMove() const {
    if (IsAlive() && m_moveCooldown == 0)
        return true;
    return false;
}

bool CLivingEntity::Move(const CCoord::direction dir) {
    if (!CanMove())
        return false;

    if (CEntity::Move(dir)) {
        m_moveCooldown = m_moveSpeed;
        for (auto i = CGame::GetWorld().GetEntities().begin(); i != CGame::GetWorld().GetEntities().end(); ++i)
            if (dynamic_pointer_cast<CBombFlame>(*i) && (*i)->GetPosition() == GetPosition()) {
                ModifyHealth(-1);
                break;
            }
        ProcessPickups();
        return true;
    } else
        return false;
}

bool CLivingEntity::Tick() {
    if (!IsAlive())
        return false;

    if (m_moveCooldown > 0) {
        m_moveCooldown--;
        return true;
    }
    if (m_AIControlled)
        RunAILogic();

    return false;
}

bool CLivingEntity::IsAlive() const {
    return m_health > 0;
}

bool CLivingEntity::IsAIControlled() const {
    return m_AIControlled;
}

CEntity::stat_type CLivingEntity::GetHealth() const {
    return m_health;
}

bool CLivingEntity::SetHealth(const int & v) {
    if (m_health == 0 && v < 0) {
        SetRemoveFlag();
        return false;
    } else
        m_health = v;
    if (m_health <= 0) {
        SetCollision(false);
        GetTile()[0] = GetTile()[2] = GetTile()[3] = GetTile()[5] = 'X';
    }
    CGame::PrintDebug("Health changed to " + to_string(m_health));
    return IsAlive();
}

bool CLivingEntity::ModifyHealth(const int & v) {
    return SetHealth(GetHealth() + v);
}

CEntity::stat_type CLivingEntity::GetMovementSpeed() const {
    return m_moveSpeed;
}

void CLivingEntity::SetMovementSpeed(const stat_type & ms) {
    if (m_moveCooldown > 0)
        m_moveCooldown = 4;
    m_moveSpeed = ms;
}

void CLivingEntity::SetMovementCooldown(const stat_type & cd) {
    m_moveCooldown = cd;
}

/**********************************************
 ******************* MONSTER ******************
 **********************************************/

CMonster::CMonster(const CCoord & loc) :
CLivingEntity(loc, "Monster", CTile('M', 'O', 'N', 'M', 'O', 'N'), true, true), m_AIAttackCooldown(0) {
    SetHealth(1);
    m_AIMoveDirection = static_cast<CCoord::direction> (rand() % 5);
}

void CMonster::RunAILogic() {
    if (m_AIAttackCooldown > 0)
        m_AIAttackCooldown--;

    if (CanMove()) {
        // All possible directions of stuff
        CCoord t[4];
        fill_n(t, 4, GetPosition());
        t[0].Move(CCoord::direction::UP);
        t[1].Move(CCoord::direction::DOWN);
        t[2].Move(CCoord::direction::LEFT);
        t[3].Move(CCoord::direction::RIGHT);

        // Monster Attack
        if (m_AIAttackCooldown <= 0) {
            CWorld::entities_vector ents = CGame::GetWorld().GetEntities();
            CWorld::ebomber_pointer bomber = NULL;
            for (auto i = ents.begin(); i != ents.end(); ++i)
                if ((bomber = dynamic_pointer_cast<CBomberman>(*i)) && ((*i)->GetPosition() == t[0] || (*i)->GetPosition() == t[1] || (*i)->GetPosition() == t[2] || (*i)->GetPosition() == t[3])) {
                    CGame::PrintDebug("AIAttack");
                    bomber->ModifyHealth(-1);
                    m_AIAttackCooldown = GetMovementSpeed() - 2;
                }
        }

        // Monster Move
        short unsigned int canOccupy = CanOccupyLocation(t[0]) + CanOccupyLocation(t[1]) + CanOccupyLocation(t[2]) + CanOccupyLocation(t[3]);
        if (canOccupy == 0) {
            // Monster is trapped. We set the cooldown because it is unlikely to be un-trapped for some time.
            SetMovementCooldown(GetMovementSpeed());
        } else {
            short int action = rand() % 100;
            CCoord nc(GetPosition());
            nc.Move(m_AIMoveDirection);
            if (action > 90) {
                m_AIMoveDirection = CCoord::direction::NONE;
            } else if (action > 65 || (!CanOccupyLocation(nc) && !(nc == GetPosition()))) {
                int ctr = 0;
                int dir;
                while (!CanOccupyLocation(t[dir = rand() % 4])) {
                    if (++ctr == 12) {
                        CGame::PrintDebug("!!! AI was pathfinding but got stuck");
                        m_AIMoveDirection = CCoord::direction::NONE;
                        break;
                    }
                }
                m_AIMoveDirection = static_cast<CCoord::direction> (dir);
            }
            if (m_AIMoveDirection != CCoord::direction::NONE) {
                CCoord md(GetPosition());
                md.Move(m_AIMoveDirection);
                for (auto i = CGame::GetWorld().GetEntities().begin(); i != CGame::GetWorld().GetEntities().end(); ++i)
                    if (dynamic_pointer_cast<CBombFlame>(*i) && (*i)->GetPosition() == md) {
                        m_AIMoveDirection = CCoord::direction::NONE;
                        break;
                    }
            }
            Move(m_AIMoveDirection);
        }
    }
}

void CMonster::ProcessPickups() {
}

/**********************************************
 ****************** BOMBERMAN *****************
 **********************************************/

CBomberman::CBomberman(const CCoord & loc, const CTile & tile, bool AIControlled, const stat_type & ms, const stat_type & hp, const stat_type & flameSize) :
CLivingEntity(loc, "Bomberman", tile, true, AIControlled, ms, hp), m_bombs(1), m_flameSize(flameSize), m_remoteDetonation(false) {
}

bool CBomberman::PlaceBomb() {
    if (!IsAlive())
        return false;
    if (m_bombs > 0 && !CGame::GetWorld().IsLocationOccupied(GetPosition(), "Bomb")) {
        m_bombs--;
        CGame::GetWorld().AddEntity(CWorld::entity_pointer(new CBomb(this)));
        return true;
    }
    return false;
}

bool CBomberman::HasRemoteDetonation() {
    return m_remoteDetonation;
}

CEntity::stat_type CBomberman::GetFlameSize() {
    return m_flameSize;
}

CEntity::stat_type CBomberman::GetBombs() {
    return m_bombs;
}

void CBomberman::AddBomb() {
    m_bombs++;
    if (IsAIControlled() && m_bombs > 1)
        m_bombs = 1;
}

void CBomberman::IncreaseFlameSize() {
    m_flameSize++;
}

void CBomberman::ProcessPickups() {
    if (!IsAlive())
        return;
    CWorld::entities_vector v = CGame::GetWorld().GetEntities();
    CWorld::epickup_pointer pickup = NULL;
    for (CWorld::entities_vector::iterator i = v.begin(); i != v.end(); ++i) {
        if ((pickup = dynamic_pointer_cast<CPickup>(*i)) && pickup->GetPosition() == GetPosition()) {
            if (pickup->GetType() == CPickup::pickup_type::REMOTE && IsAIControlled())
                continue;
            switch (pickup->GetType()) {
                case CPickup::pickup_type::BOMB:
                    AddBomb();
                    break;
                case CPickup::pickup_type::FLAME:
                    IncreaseFlameSize();
                    break;
                case CPickup::pickup_type::HEALTH:
                    ModifyHealth(+1);
                    break;
                case CPickup::pickup_type::SPEED:
                    SetMovementSpeed(GetMovementSpeed() - GetMovementSpeed() / 3);
                    if (GetMovementSpeed() < 4)
                        SetMovementSpeed(4);
                    break;
                case CPickup::pickup_type::REMOTE:
                    m_remoteDetonation = true;
                    break;
                default:
                    break;
            }
            pickup->SetRemoveFlag();
        }
    }
}

void CLivingEntity::DetonateOwnedBombs() {
    if (!IsAlive())
        return;
    CGame::GetWorld().LockEntities();
    shared_ptr<CBomb> bomb = NULL;
    for (auto i = CGame::GetWorld().GetEntities().rbegin(); i != CGame::GetWorld().GetEntities().rend(); ++i) {
        if ((bomb = dynamic_pointer_cast<CBomb>(*i)) && bomb->GetOwner() == this) {
            bomb->Detonate();
        }
    }
    CGame::GetWorld().UnlockEntities();
}

/**
 * tests entity pointer for dangerous stuff
 * @param i entity pointer
 * @return true if it is dangerous
 */
bool isDanger(const CWorld::entity_pointer i) {
    return dynamic_pointer_cast<CBombFlame>(i) != NULL || dynamic_pointer_cast<CBomb>(i) != NULL || dynamic_pointer_cast<CLivingEntity>(i) != NULL;
}

/**
 * counts score for nearby tiles
 * @param c reference location
 * @return score of neighbors
 */
int deepScan(const CCoord & c) {
    CCoord t[4];
    fill_n(t, 4, c);
    int r = 0;
    for (int i = 0; i < 4; i++) {
        t[i].Move(static_cast<CCoord::direction> (i));
        if (!CGame::GetWorld().GetBlock(t[i]).IsPassable() && !CGame::GetWorld().GetBlock(t[i]).IsDestructible())
            r += 100;
        else if (!CGame::GetWorld().GetBlock(t[i]).IsPassable())
            r += 25;
        else
            r += 0;
    }
    return r;
}

/**
 * Looks around central location c and tells if the target location is near search
 *
 * The nearer the target is to search the more points are added
 * @param c center of search
 * @param t searched target
 * @return true if there are bombs nearby
 */
int weightedRowColSearch(const CCoord & c, const CCoord & t) {
    int r = 0;
    CCoord e[4];
    fill_n(e, 4, c);
    for (int j = 1; j < 5; j++)
        for (int b = 0; b < 4; b++) {
            e[b].Move(static_cast<CCoord::direction> (b));
            if (e[b] == t) {
                r += 60 + (5 - j) * 28;
            }
        }
    return r;
}

void CBomberman::RunAILogic() {
rerunAILogic:
    if (CanMove()) {

        enum class behavior {
            WAIT, RUN, ANY, BONUS
        };
        behavior b = behavior::ANY;

        // get score for all nearby tiles
        CCoord t[4];
        fill_n(t, 4, GetPosition());
        int dirScore[4];
        for (int i = 0; i < 4; i++) {
            t[i].Move(static_cast<CCoord::direction> (i));
            if (!CGame::GetWorld().GetBlock(t[i]).IsPassable() && !CGame::GetWorld().GetBlock(t[i]).IsDestructible())
                dirScore[i] = 1000;
            else if (!CGame::GetWorld().GetBlock(t[i]).IsPassable())
                dirScore[i] = 100;
            else
                dirScore[i] = 0;
            if (dirScore[i] < 1000)
                dirScore[i] += deepScan(t[i]);
            for (auto f = CGame::GetWorld().GetEntities().begin(); f != CGame::GetWorld().GetEntities().end(); ++f)
                if (dynamic_pointer_cast<CBomb>(*f))
                    dirScore[i] += weightedRowColSearch(t[i], (*f)->GetPosition());
        }
        // determine the action (should we run? take a pickup?)
        CWorld::epickup_pointer p = NULL;
        int dir = 0;
        for (auto i = CGame::GetWorld().GetEntities().rbegin(); i != CGame::GetWorld().GetEntities().rend(); ++i) {
            if ((*i)->GetPosition() == GetPosition()) {
                if (dynamic_pointer_cast<CBomberman>(*i))
                    continue;
                else if (isDanger(*i)) {
                    b = behavior::RUN;
                    break;
                }
            } else if (dynamic_pointer_cast<CBomb>(*i) && weightedRowColSearch(GetPosition(), (*i)->GetPosition()) > 0) {
                // this checks for immediate bombs. just RUN
                b = behavior::RUN;
                CGame::PrintDebug("MUST RUN");
                break;
            } else {
                for (int k = 0; k < 4; k++)
                    if ((*i)->GetPosition() == t[k]) {
                        if (isDanger(*i))
                            dirScore[k] += 1000;
                        else if ((p = dynamic_pointer_cast<CPickup>(*i)) && p->GetType() != CPickup::pickup_type::REMOTE && CanOccupyLocation(t[k])) {
                            b = behavior::BONUS;
                            dir = k;
                            break;
                        }
                    }
            }

        }
        int dirScoreTotal = 0;
        for (int k = 0; k < 4; k++)
            dirScoreTotal += dirScore[k];

        if (dirScoreTotal > 3800) //too dangerous out there, do nothing
            if (b == behavior::ANY)
                b = behavior::WAIT;

        switch (b) {
            case behavior::WAIT:
                SetMovementCooldown(GetMovementSpeed() / 3 + 1);
                break;
            case behavior::BONUS:
                Move(static_cast<CCoord::direction> (dir));
                break;
            case behavior::ANY:
                // ANY differs from RUN mostly in just that it has a chance to stay in place
                if (rand() % 10 > 6) {
                    SetMovementCooldown(GetMovementSpeed() * 1.7);
                    break;
                }
            case behavior::RUN:
                int min = 0;
                for (int i = 0; i < 4; i++) //set default dir to something occupyable
                    if (CanOccupyLocation(t[i])) {
                        min = i;
                        break;
                    }

                for (int i = 0; i < 4; i++) //find safest *available* direction
                    if (dirScore[i] < 1000 && (rand() % 10 > 3)) {
                        if (!CanOccupyLocation(t[i]))
                            continue;
                        if (dirScore[min] > dirScore[i])
                            min = i;
                        else if (dirScore[min] == dirScore[i])
                            min = ((rand() % 1 == 1) ? min : i);
                    }
                //CGame::PrintDebug("SCORES: " + to_string(--dirScore[0]) + " | " + to_string(--dirScore[1]) + " | " + to_string(--dirScore[2]) + " | " + to_string(--dirScore[3]) + " | ");
                // final check if the chosen tile isn't too dangerous
                for (auto f = CGame::GetWorld().GetEntities().begin(); f != CGame::GetWorld().GetEntities().end(); ++f)
                    if (dynamic_pointer_cast<CBombFlame>(*f) && (*f)->GetPosition() == t[min]) {
                        // there is fire in the spot we want to go. ABORT
                        b = behavior::WAIT;
                        SetMovementCooldown(GetMovementSpeed() / 2 + 1);
                        break;
                    } else if (b == behavior::ANY && dynamic_pointer_cast<CBomb>(*f) && weightedRowColSearch(t[min], (*f)->GetPosition()) > 0) {
                        //CGame::PrintDebug("BOMB in our direction ABORT");
                        b = behavior::WAIT;
                        SetMovementCooldown(GetMovementSpeed() / 2 + 1);
                        break;
                    }
                // chance to place a bomb if we are sort of fine
                if (dirScoreTotal < 2400 && rand() % 12 > 9 && b == behavior::ANY) {
                    PlaceBomb();
                    CGame::PrintDebug("Random AI bomb placed!");
                    goto rerunAILogic;
                }
                Move(static_cast<CCoord::direction> (min));
                break;
        }

        // plant bomb when enemy is near
        for (auto f = CGame::GetWorld().GetEntities().begin(); f != CGame::GetWorld().GetEntities().end(); ++f)
            if (dynamic_pointer_cast<CLivingEntity>(*f) && weightedRowColSearch(GetPosition(), (*f)->GetPosition()) > 0) {
                PlaceBomb();
                break;
            }

        // plant bomb in enclosed space
        int co = 0;
        for (int i = 0; i < 4; i++) {
            CCoord a(GetPosition());
            a.Move(static_cast<CCoord::direction> (i));
            if (CanOccupyLocation(a))
                co++;
        }
        if (co == 1)
            PlaceBomb();

    }


}