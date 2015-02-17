#include <ncurses.h>
#include <vector>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "CWorld.h"
#include "CTile.h"
#include "CCoord.h"
#include "LivingEntities.h"
#include "CGame.h"
#include "CBomb.h"

using namespace std;

CWorld::CWorld() :
m_entitiesLock(false), m_player(NULL), m_remotePlayer(NULL) {
}

void CWorld::Clear() {
    UnlockEntities();
    m_entities.clear();
    for (int i = 0; i < CCoord::WIDTH; i++)
        for (int j = 0; j < CCoord::HEIGHT; j++)
            m_world[i][j] = CBlock();
    FixBorders();
}

void CWorld::Draw() const {
    for (int i = 0; i < CCoord::WIDTH; i++) {
        for (int j = 0; j < CCoord::HEIGHT; j++) {
            CCoord c(i, j);
            wmove(stdscr, c.ScreenCoord().second, c.ScreenCoord().first);
            m_world[i][j].Draw();
        }
    }

    // draw all
    for (auto i = m_entities.rbegin(); i != m_entities.rend(); ++i)
        (*i)->Draw();
    // bombs and flames
    for (auto i = m_entities.rbegin(); i != m_entities.rend(); ++i)
        if (dynamic_pointer_cast<CBomb>(*i) || dynamic_pointer_cast<CBombFlame>(*i))
            (*i)->Draw();
    // living stuff
    for (auto i = m_entities.rbegin(); i != m_entities.rend(); ++i)
        if (dynamic_pointer_cast<CLivingEntity>(*i))
            (*i)->Draw();

    wmove(stdscr, 0, 0);
    if (m_player != NULL) {
        waddstr(stdscr, string("Health: " + to_string(m_player->GetHealth())
                + " Bombs: " + to_string(m_player->GetBombs())
                + " Flame: " + to_string(m_player->GetFlameSize())).c_str());
    }
}

void CWorld::LoadRetroWorld() {
    Clear();
    for (int i = 1; i < CCoord::HEIGHT - 1; i++) {
        for (int j = 1; j < CCoord::WIDTH - 1; j++)
            if (rand() % 100 > 25)
                m_world[j][i] = CCrateBlock();

        if (i < CCoord::HEIGHT && i % 2 == 0) {
            for (int j = 2; j < CCoord::WIDTH; j += 2) {
                m_world[j][i] = CBedrockBlock();
            }
        }
    }

    FixBorders();

    // clear startpoints
    m_world[1][1] = m_world[1][2] = m_world[2][1] = CBlock();
    m_world[CCoord::WIDTH - 2][1] = m_world[CCoord::WIDTH - 2][2] = m_world[CCoord::WIDTH - 3][1] = CBlock();
    m_world[1][CCoord::HEIGHT - 2] = m_world[1][CCoord::HEIGHT - 3] = m_world[2][CCoord::HEIGHT - 2] = CBlock();
    m_world[CCoord::WIDTH - 2][CCoord::HEIGHT - 2] = m_world[CCoord::WIDTH - 2][CCoord::HEIGHT - 3] = m_world[CCoord::WIDTH - 3][CCoord::HEIGHT - 2] = CBlock();

    // player
    ebomber_pointer player = ebomber_pointer(new CBomberman(CCoord(1, 1), CTile('<', 'P', '>', '<', '1', '>'), false));
    AddEntity(player);
    m_player = ebomber_pointer(player);

    // monsters and stuff
    AddEntity(entity_pointer(new CMonster(CCoord(7, 6))));
    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::SERVER) {
        // remote player
        ebomber_pointer player = ebomber_pointer(new CBomberman(CCoord(CCoord::WIDTH - 2, CCoord::HEIGHT - 2), CTile('<', 'P', '>', '<', '2', '>'), false));
        AddEntity(player);
        m_remotePlayer = ebomber_pointer(player);
    } else
        AddEntity(entity_pointer(new CBomberman(CCoord(CCoord::WIDTH - 2, CCoord::HEIGHT - 2), CTile('<', 'A', '>', '<', 'I', '>'), true)));
    //AddEntity(entity_pointer(new CBomberman(CCoord(CCoord::WIDTH - 2, 1), CTile('<', 'A', '>', '<', 'I', '>'), true)));
    //AddEntity(entity_pointer(new CBomberman(CCoord(1, CCoord::HEIGHT - 2), CTile('<', 'A', '>', '<', 'I', '>'), true)));

}

void CWorld::LoadPickupArenaWorld() {
    Clear();
    // player
    ebomber_pointer player = ebomber_pointer(new CBomberman(CCoord(1, 1), CTile('<', 'P', '>', '<', '1', '>'), false));
    AddEntity(player);
    m_player = ebomber_pointer(player);
    if (CGame::GetMPGameMode() == CGame::multiplayer_mode::SERVER) {
        // remote player
        ebomber_pointer player = ebomber_pointer(new CBomberman(CCoord(CCoord::WIDTH - 2, CCoord::HEIGHT - 2), CTile('<', 'P', '>', '<', '2', '>'), false));
        AddEntity(player);
        m_remotePlayer = ebomber_pointer(player);
    } else // AI
        AddEntity(entity_pointer(new CBomberman(CCoord(CCoord::WIDTH - 2, CCoord::HEIGHT - 2), CTile('<', 'A', '>', '<', 'I', '>'), true)));

    m_player->AddBomb();
    m_player->AddBomb();
    m_player->AddBomb();
    m_player->IncreaseFlameSize();
    m_player->IncreaseFlameSize();
    m_player->IncreaseFlameSize();
    m_player->SetMovementSpeed(m_player->GetMovementSpeed() / 2);
    if (m_remotePlayer != NULL) {
        m_remotePlayer->AddBomb();
        m_remotePlayer->AddBomb();
        m_remotePlayer->AddBomb();
        m_remotePlayer->IncreaseFlameSize();
        m_remotePlayer->IncreaseFlameSize();
        m_remotePlayer->IncreaseFlameSize();
        m_remotePlayer->SetMovementSpeed(m_remotePlayer->GetMovementSpeed() / 2);
    }
}

void CWorld::FixBorders() {
    for (int i = 0; i < CCoord::WIDTH; i++) {
        m_world[i][0] = CBedrockBlock();
        m_world[i][CCoord::HEIGHT - 1] = CBedrockBlock();
    }

    for (int i = 1; i < CCoord::HEIGHT - 1; i++) {
        m_world[0][i] = CBedrockBlock();
        m_world[CCoord::WIDTH - 1][i] = CBedrockBlock();
    }
}

bool CWorld::LoadFromFile(const string & f) {
    ifstream file;
    file.open(f);
    if (file.fail()) {
        cerr << "map file could not be opened" << endl;
        file.close();
        return false;
    }
    int bomberCount = 0;
    Clear();
    for (int j = 0; j < CCoord::HEIGHT; j++)
        for (int i = 0; i < CCoord::WIDTH; i++) {
            char t;
            while ((t = file.get()) == '\n');
            if (file.fail()) {
                file.close();
                cerr << "map file is not complete" << endl;
                Clear();
                return false;
            }
            t = toupper(t);
            string s;
            s.push_back(t);
            CGame::PrintDebug("printing " + s + " at " + to_string(CCoord(i, j)));
            switch (t) {
                case 'X': // hard wall
                    m_world[i][j] = CBedrockBlock();
                    break;
                case 'C': // guaranteed crate
                    m_world[i][j] = CCrateBlock();
                    break;
                case 'O': // maybe-crate
                    if (rand() % 100 > 25)
                        m_world[i][j] = CCrateBlock();
                    break;
                case 'M': // monster spawn
                    AddEntity(entity_pointer(new CMonster(CCoord(i, j))));
                    break;
                case 'B': // bomberman spawn
                    bomberCount++;
                    if (bomberCount == 1) {
                        // player
                        ebomber_pointer player = ebomber_pointer(new CBomberman(CCoord(i, j), CTile('<', 'P', '>', '<', '1', '>'), false));
                        AddEntity(player);
                        m_player = ebomber_pointer(player);
                    } else if (bomberCount == 2 && CGame::GetMPGameMode() == CGame::multiplayer_mode::SERVER) {
                        // remote player
                        ebomber_pointer player = ebomber_pointer(new CBomberman(CCoord(i, j), CTile('<', 'P', '>', '<', '2', '>'), false));
                        AddEntity(player);
                        m_remotePlayer = ebomber_pointer(player);
                    } else {
                        // AI
                        AddEntity(entity_pointer(new CBomberman(CCoord(i, j), CTile('<', 'A', '>', '<', 'I', '>'), true)));
                    }
                    break;
                case 'N': // empty space
                case ' ':
                    break;
                default:
                    cerr << "map file contains unknown character(s): '" << t << "'" << endl;
                    return false;
                    break;
            }
        }
    file.close();
    if (bomberCount < 2) {
        cerr << "map file requires at least two places to spawn bombermen" << endl;
        Clear();
        return false;
    }

    FixBorders();
    return true;
}

bool CWorld::IsLocationOccupied(const CCoord & loc, const string & entName) const {
    if (entName.empty()) {
        if (!m_world[loc.GetX()][loc.GetY()].IsPassable())
            return true;
        for (auto i = m_entities.begin(); i != m_entities.end(); ++i)
            if ((*i)->HasCollision() && (*i)->GetPosition() == loc)
                return true;
    } else {
        for (auto i = m_entities.begin(); i != m_entities.end(); ++i)
            if ((*i)->HasCollision() && (*i)->GetName() == entName && (*i)->GetPosition() == loc)
                return true;
    }
    return false;
}

void CWorld::AddEntity(entity_pointer e) {
    if (m_entitiesLock)
        m_entitiesTMP.push_back(e);
    else
        m_entities.push_back(e);
}

CWorld::entities_vector & CWorld::GetEntities() {
    return m_entities;
}

CWorld::ebomber_pointer CWorld::GetPlayer() {
    return m_player;
}

void CWorld::Tick() {
    LockEntities();
    for (auto i : m_entities)
        i->Tick();

    for (auto i = m_entities.begin(); i != m_entities.end();) {
        if ((*i)->GetRemoveFlag())
            i = m_entities.erase(i);
        else
            ++i;
    }
    UnlockEntities();
}

void CWorld::SetBlock(const CCoord & loc, const CBlock & block) {
    m_world[loc.GetX()][loc.GetY()] = block;
}

CBlock CWorld::GetBlock(const CCoord & loc) const {
    return m_world[loc.GetX()][loc.GetY()];
}

CWorld::ebomber_pointer CWorld::GetNetworkedEntity() {
    return m_remotePlayer;
}

void CWorld::LockEntities() {
    m_entitiesLock = true;
}

void CWorld::UnlockEntities() {
    m_entities.insert(m_entities.end(), m_entitiesTMP.begin(), m_entitiesTMP.end());
    m_entitiesTMP.clear();
    m_entitiesLock = false;
}

/* @todo advanced AI
CWorld::aispots_type& CWorld::GetAISpotsMap() {
    return m_AISpotsMap;
}

void CWorld::RecalcAISpotsMap() {
    for (int i = 0; i < CCoord::WIDTH; i++)
        for (int j = 0; j < CCoord::HEIGHT; j++)
            if (!m_world[i][j].IsDestructible() && !m_world[i][j].IsPassable())
                m_AISpotsMap[i][j] = 100;
            else if (!m_world[i][j].IsPassable())
                m_AISpotsMap[i][j] = 20;
            else
                m_AISpotsMap[i][j] = 10;

    for (auto i = m_entities.begin(); i != m_entities.end(); i++) {

    }
}
 */