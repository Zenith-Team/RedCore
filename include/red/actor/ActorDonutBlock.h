#pragma once

#include <map_obj/ChikuwaBlockBase.h>

namespace red {

    /**
     * @brief An implementation of Donut Block as an actor instead of a tile.
     */
    class ActorDonutBlock : public ChikuwaBlockBase {
        SEAD_RTTI_OVERRIDE(ActorDonutBlock, ActorMultiState)
    
    public:
        static Profile* sProfile;
    
        ActorDonutBlock(const ActorCreateParam& param);
        ~ActorDonutBlock() override = default;
        
        Result create() override;
        bool execute() override;
        
        void reviveCollisionCheck() override;
        
        void reset1() override;
        void reset2() override;
        
        DECLARE_STATE_ID(ActorDonutBlock, Idle)
    };

}
