/*
 * File:   LivingEntities.h
 * Author: amunak
 *
 * Created on May 26, 2014, 3:44 PM
 */

#ifndef LIVINGENTITIES_H
#define	LIVINGENTITIES_H

#include "CEntity.h"

using namespace std;

class CLivingEntity : public CEntity {
public:
    /**
     * Create a living entity with health health
     * @see CEntity::CEntity
     * @param AIcontrol true if this is AI-controlled entity
     * @param moveSpeed initial movement speed of the entity
     * @param health entity's health (defaults to 3)
     */
    CLivingEntity(const CCoord &, const string &, const CTile &, bool, bool = true, const stat_type & = 0, const stat_type & = 3);


    /**
     * Moves the living entity
     * @see CEntity::Move
     * @param dir direction of movement
     * @return false on failure
     */
    virtual bool Move(const CCoord::direction);

    /**
     * Returns true if entity is alive
     * @return true when alive, false when dead
     */
    bool IsAlive() const;

    /**
     * Returns true if entity is AI-controlled
     * @return true when AI-controlled
     */
    bool IsAIControlled() const;

    /**
     * Tests whether this entity can move
     * @return true if the entity can move this tick
     */
    bool CanMove() const;

    /**
     * @see CEntity::Tick
     */
    virtual bool Tick();

    /**
     * Detonates this bomberman's bombs if he has the remote detonation device
     */
    void DetonateOwnedBombs();

    /**
     * Returns this entity's health
     * @return current health
     */
    stat_type GetHealth() const;

    /**
     * Sets this entity's health
     * @param val new health value
     * @return true if entity is still alive
     */
    bool SetHealth(const int &);

    /**
     * Modifies this entity's health
     * @param val value to add to (or to remove from if negative) current health
     * @return true if entity died in the process
     */
    bool ModifyHealth(const int & = -1);

    /**
     * Returns this entity's current movement speed
     * @return movement speed
     */
    stat_type GetMovementSpeed() const;

    /**
     * Set this entity's movement speed
     * @param moveSpeed new movement speed
     */
    void SetMovementSpeed(const stat_type &);

    /**
     * Set this entity's movement cooldown
     * @param moveCooldown new cooldown
     */
    void SetMovementCooldown(const stat_type &);

private:
    /**
     * Health of the entity
     *
     * When it reaches 0 the entity is considered dead
     */
    stat_type m_health;

    /**
     * Entity's movement speed
     *
     * Entity can move every n-th tick. This means that lower value equals faster entity.
     */
    stat_type m_moveSpeed;

    /**
     * Time left until the entity can move again
     */
    stat_type m_moveCooldown;

    /**
     * Tells if this is AI-controlled entity
     */
    bool m_AIControlled;

    /**
     * Processes pickups
     */
    virtual void ProcessPickups() = 0;

    /**
     * Contains AI code
     *
     * It is supposed to be ran each tick
     */
    virtual void RunAILogic() = 0;
};

class CMonster : public CLivingEntity {
public:
    /**
     * Create a new monster
     * @param loc location of the monster
     */
    CMonster(const CCoord &);

private:

    CLivingEntity::stat_type m_AIAttackCooldown;

    /**
     * Saves the direction this AI-controlled entity is moving in
     */
    CCoord::direction m_AIMoveDirection;

    /**
     * Pickups for monsters
     * @see CLivingEntity::ProcessPickups
     */
    virtual void ProcessPickups();

    /**
     * AI for monsters
     * @see CLivingEntity::RunAILogic     */
    virtual void RunAILogic();
};

class CBomberman : public CLivingEntity {
public:
    /**
     * Create a bomberman
     *
     * Can be either player or AI-controlled
     * @see CLivingEntity::ClivingEntity
     * @param flameSize initial size of bomberman's flame
     */
    CBomberman(const CCoord &, const CTile &, bool, const stat_type & = 0, const stat_type & = 3, const stat_type & = 1);

    /**
     * Gives the bomberman an extra bomb
     */
    void AddBomb();

    /**
     * Increases bomberman's flame by one;
     */
    void IncreaseFlameSize();

    /**
     * Places a bomb by this bomberman
     * @return returns true on success
     */
    bool PlaceBomb();

    /**
     * Tells whether this bomberman has the remote detonation upgrade
     * @return true if he does
     */
    bool HasRemoteDetonation();

    /**
     * Returns this bomberman's flame size
     * @return flame size
     */
    stat_type GetFlameSize();

    /**
     * Returns the amount of bombs the player has
     * @return bombs amount
     */
    stat_type GetBombs();

private:
    /**
     * Holds current amount of bombs
     */
    stat_type m_bombs;

    /**
     * Tells how big the flame is
     */
    stat_type m_flameSize;

    /**
     * Tells whether this entity creates bomb which do not explode (until detonated)
     */
    bool m_remoteDetonation;

    virtual void ProcessPickups();

    virtual void RunAILogic();


};
#endif	/* LIVINGENTITIES_H */

