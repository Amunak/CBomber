#ifndef CPICKUP_H
#define	CPICKUP_H

#include "CEntity.h"

class CPickup : public CEntity {
public:

    /**
     * A type representing pickup type
     */
    enum class pickup_type {
        HEALTH,
        FLAME,
        BOMB,
        SPEED,
        REMOTE,
        NONE
    };

    /**
     * Create a pickup entity at target location loc
     *
     * Note that using custom pickup types means you have to handle
     * the tile appearance yourself.
     * @param loc location of pickup
     * @param pickupType (optional) pickup type; defaults to random selection
     */
    CPickup(const CCoord &, const CPickup::pickup_type = pickup_type::NONE);

    /**
     * Returns the type of this pickup
     * @return pickup type
     */
    pickup_type GetType();
private:
    /**
     * Holds the type of this pickup
     */
    pickup_type m_type;

    /**
     * @see CEntity::Tick
     */
    virtual bool Tick();

};

#endif	/* CICKUP_H */

