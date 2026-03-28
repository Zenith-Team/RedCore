#pragma once

#include <system/LineNodeMgr.h>

#include <heap/seadDisposer.h>

class ActorBgCollisionCheck;

namespace red {
    
    class ActorBgCollisionCheckMgr {
        SEAD_SINGLETON_DISPOSER(ActorBgCollisionCheckMgr)
    
    public:
        using List = LineNodeMgr<ActorBgCollisionCheck>;
    
    public:
        ActorBgCollisionCheckMgr() = default;
    
        [[nodiscard]]
        const List& getDrawList() const {
            return mDrawList;
        }
    
    public:
        void entry(const ActorBgCollisionCheck& bc);
        void release(const ActorBgCollisionCheck& bc);
    
    private:
        List::Node* findInDrawList_(const ActorBgCollisionCheck& bc) const;
        bool isInDrawList_(const ActorBgCollisionCheck& bc) const;
        void addToDrawList_(const ActorBgCollisionCheck& bc);
        void removeFromDrawList_(const ActorBgCollisionCheck& bc);
    
    private:
        List mDrawList;
    };
    
}
