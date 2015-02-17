#ifndef CBOMB_H
#define	CBOMB_H

#include "CEntity.h"
#include "LivingEntities.h"
#include "CWorld.h"

class CBomb : public CEntity {
public:
    /**
     * Creates a bomb
     *
     * All data is taken from the owner
     * @param pointer to this bomb's owner
     */
    CBomb(CBomberman*);

    /**
     * @see CEntity::Tick
     */
    bool Tick();

    /**
     * Returns pointer to this bomb's owner
     */
    CBomberman* GetOwner();

    /**
     * Detonates the bomb spawning flames and removing destructible blocks
     */
    void Detonate();

private:
    /**
     * Pointer to owner of this bomb
     */
    CBomberman* m_owner;

    /**
     * Size (range) of flame that will be spawned on detonation
     */
    CLivingEntity::stat_type m_flameSize;

    /**
     * Detonation time in ticks
     *
     * Bomb detonates when this reaches 0
     * When over 200, the bomb does NOT detonate
     */
    CLivingEntity::stat_type m_detonationTime;

    /**
     * Places a bomb flame in target location (and destroys anything that is currently standing there)
     * @param loc location of the flame
     */
    void PlaceFlame(const CCoord &) const;
};

class CBombFlame : public CEntity {
public:
    /**
     * Create a flame in location loc
     * @param loc flame location
     */
    CBombFlame(const CCoord &);

    /**
     * Processess tick
     * @see CEntity::Tick
     */
    virtual bool Tick();

private:
    /**
     * Time in ticks until the fire is extinguished
     */
    CLivingEntity::stat_type m_extinguishTime;

    /**
     * Randomizes this flame's tile
     */
    void RandomizeAppearance();
};

#endif	/* CBOMB_H */

