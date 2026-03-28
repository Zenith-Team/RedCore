#define TELKIN_REGISTERS
#include <telkin/Assembly.h>
#include <telkin/Hooks.h>
#include <collision/ActorBgCollisionCheck.h>
#include <red/collision/ActorBgCollisionCheckMgr.h>

namespace red {

SEAD_SINGLETON_DISPOSER_IMPL(ActorBgCollisionCheckMgr)

} // namespace red

red::ActorBgCollisionCheckMgr::List::Node* red::ActorBgCollisionCheckMgr::findInDrawList_(const ActorBgCollisionCheck& bc) const {
    for (List::Node* node = mDrawList.front(); node != nullptr; node = node->next)
        if (node->obj == &bc)
            return node;

    return nullptr;
}

bool red::ActorBgCollisionCheckMgr::isInDrawList_(const ActorBgCollisionCheck& bc) const {
    return findInDrawList_(bc) != nullptr;
}

void red::ActorBgCollisionCheckMgr::addToDrawList_(const ActorBgCollisionCheck& bc) {
    if (!isInDrawList_(bc)) {
        List::Node* node = new (MEMAllocFromDefaultHeap(sizeof(List::Node))) List::Node(const_cast<ActorBgCollisionCheck*>(&bc));
        mDrawList.pushBack(node);
    }
}

void red::ActorBgCollisionCheckMgr::removeFromDrawList_(const ActorBgCollisionCheck& bc) {
    List::Node* node = findInDrawList_(bc);
    if (node != nullptr) {
        mDrawList.erase(node);
        MEMFreeToDefaultHeap(node);
    }
}

void red::ActorBgCollisionCheckMgr::entry(const ActorBgCollisionCheck& bc) {
    addToDrawList_(bc);
}

void red::ActorBgCollisionCheckMgr::release(const ActorBgCollisionCheck& bc) {
    removeFromDrawList_(bc);
}

namespace red {
    uintptr ActorBgCollisionCheckMgr_createInstance(uintptr collider_mgr) {
        red::ActorBgCollisionCheckMgr::createInstance(nullptr);
        return collider_mgr;
    }
} // namespace red
tBranch(0x021924DC, red::ActorBgCollisionCheckMgr_createInstance, tk::BranchType::b);

namespace red {
    void ActorBgCollisionCheckMgr_entry(const ActorBgCollisionCheck& bc){
        red::ActorBgCollisionCheckMgr::instance()->entry(bc);
    }
    
    void ActorBgCollisionCheckMgr_entry_R28() tAssembly(
        mr r3, r28;
        
        // original prologue
        lmw r21, 0xC(r1);
        lfs f31, 0x40(r1);
        lfd f31, 0x38(r1);
        isync;
        lwz r0, 0x4C(r1);
        mtlr r0;
        addi r1, r1, 0x48;
        
        b _ZN3red30ActorBgCollisionCheckMgr_entryERK21ActorBgCollisionCheck;
    )
} // namespace red
tBranch(0x0218B3B4, red::ActorBgCollisionCheckMgr_entry_R28, tk::BranchType::b);
