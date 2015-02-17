#ifndef CENTITY_H
#define	CENTITY_H

#include <string>

#include "CTile.h"
#include "CCoord.h"

class CEntity {
public:
    /**
     * Type representing an entity's statistics
     */
    typedef unsigned char stat_type;

    /**
     * Create an entity in location loc
     * @param loc location
     * @param name entity name
     * @param tile tile representing the entity
     * @param collision turn on collision for this entity?
     */
    CEntity(const CCoord &, const string &, const CTile &, bool);

    /**
     * Move the entity in the specified direction
     * @param direction the new direction
     * @return true on success
     */
    virtual bool Move(const CCoord::direction);

    /**
     * Jump-moves the entity to specified location
     * @param coord target location
     */
    void Jump(const CCoord &);

    /**
     * Checks whether this entity can occupy target location
     * @param coord target location
     * @return true if target location is available
     */
    virtual bool CanOccupyLocation(const CCoord &) const;

    /**
     * Draws the entity in the world
     */
    virtual void Draw();

    /**
     * Gets called on every game tick
     * @return true if entity was updated
     */
    virtual bool Tick() = 0;

    /**
     * Returns entity's position in the world
     * @return coordinates of the entity
     */
    CCoord GetPosition() const;

    /**
     * Returns this entity's collision
     * @return true if it has collision
     */
    bool HasCollision() const;

    /**
     * Sets the collision
     * @param collision should this entity collide?
     */
    void SetCollision(bool);

    /**
     * Marks the entity for removal
     */
    void SetRemoveFlag();

    /**
     * Returns the removal flag
     * @return true if entity is to be removed, false otherwise
     */
    bool GetRemoveFlag();

    /**
     * Returns the name of this entity
     * @return string name
     */
    string GetName() const;

    /**
     * Returns this entity's tile allowing for changes
     * @return ref to tile
     */
    CTile & GetTile();

private:
    /**
     * Name of the entity
     */
    string m_name;

    /**
     * The tile this entity is represented by
     */
    CTile m_tile;

    /**
     * Position in the world
     */
    CCoord m_position;

    /**
     * Enable collision for this entity
     */
    bool m_collision;

    /**
     * Holds the remove flag
     *
     * This entity will be removed on the next possible occasion (tick)
     */
    bool m_remove;
};



#endif	/* CENTITY_H */

