#ifndef CWORLD_H
#define	CWORLD_H

#include <cstdlib>
#include <utility>
#include <memory>
#include <vector>

#include "Blocks.h"
#include "CCoord.h"
#include "CEntity.h"
#include "LivingEntities.h"
#include "CPickup.h"

using namespace std;

class CWorld {
public:
    /**
     * Type representing a pointer to entity
     */
    typedef shared_ptr<CEntity> entity_pointer;

    /**
     * Type representing a pointer to entity
     */
    typedef shared_ptr<CLivingEntity> eliving_pointer;

    /**
     * Type representing a pointer to bomberman entity
     */
    typedef shared_ptr<CBomberman> ebomber_pointer;

    /**
     * Type representing a pointer to pickup entity
     */
    typedef shared_ptr<CPickup> epickup_pointer;

    /**
     * Type representing vector of entities
     */
    typedef vector<CWorld::entity_pointer> entities_vector;

    /**
     * Type representing various AI spots
     */
    //typedef unsigned char aispots_type;

    /**
     * World tile draw offset in screen coordinates from top
     */
    static const int DRAWOFFSET_TOP = 2;

    /**
     * World tile draw offset in screen coordinates from left
     */
    static const int DRAWOFFSET_LEFT = 0;

    /**
     * Create an empty world
     */
    CWorld();

    /**
     * Draws the world to screen
     */
    void Draw() const;

    /**
     * Empties the world
     */
    void Clear();

    /**
     * Gets called on every game tick
     */
    void Tick();

    /**
     * Loads the "original" world, slightly randomized
     */
    void LoadRetroWorld();

    /**
     * Loads the "pickup arena" world
     */
    void LoadPickupArenaWorld();

    /**
     * Loads world from a file
     * @param fileName path to file
     * @return true on success
     */
    bool LoadFromFile(const string &);

    /**
     * Set a block in location loc to block
     * @param loc target location
     * @param block new block for the location
     */
    void SetBlock(const CCoord &, const CBlock & = CBlock());

    /**
     * Returns the block at location loc
     * @param loc target location
     * @return block residing on loc
     */
    CBlock GetBlock(const CCoord &) const;

    /**
     * Checks whether a location is occupied (either by a solid block or collision-enabled entity)
     *
     * When second parameter is specified it checks only for entities of that name
     * @param coord location to be checked
     * @param entityName
     * @return true if occupied, false when empty
     */
    bool IsLocationOccupied(const CCoord &, const string & = "") const;

    /**
     * Insert entity in the world
     * @param entity the entity
     */
    void AddEntity(entity_pointer);

    /**
     * Provides access to this world's entities
     * @return vector of all entities
     */
    vector<entity_pointer> & GetEntities();

    /**
     * Provides access to player-controlled character
     * @return entity representing the player
     */
    CWorld::ebomber_pointer GetPlayer();

    /**
     * Locks the entities array for safe addition
     */
    void LockEntities();

    /**
     * Unlocks the entities array and commits the changes
     */
    void UnlockEntities();

    /**
     * Returns the remote-controlled entity
     * @return pointer to bomber entity
     */
    CWorld::ebomber_pointer GetNetworkedEntity();

    /**
     * Returns the spot map for AI
     * @return AI spot map
     */
    //aispots_type  & GetAISpotsMap();

    /**
     * Recalculates the spots map for current positions of entities and blocks
     */
    //void RecalcAISpotsMap();

private:
    /**
     * Two-dimensional array of this world's blocks
     */
    CBlock m_world[CCoord::WIDTH][CCoord::HEIGHT];

    //aispots_type m_AISpotsMap[CCoord::WIDTH][CCoord::HEIGHT];

    /**
     * Vector of pointers to all entities
     * @todo add other vectors holding specific types of entities to make processing times faster
     */
    entities_vector m_entities;

    entities_vector m_entitiesTMP;
    bool m_entitiesLock;


    /**
     * Pointer to the player-controlled entity
     */
    ebomber_pointer m_player;

    /**
     * Pointer to the "remote" player entity
     */
    ebomber_pointer m_remotePlayer;

    /**
     * Makes borders of the map from solid, unbreakable blocks
     */
    void FixBorders();

};

#endif	/* CWORLD_H */

