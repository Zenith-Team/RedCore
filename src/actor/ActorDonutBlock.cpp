#include "map_obj/ChikuwaBlockBase.h"
#include <red/actor/ActorDonutBlock.h>
#include <collision/ActorBgCollisionMgr.h>

SEAD_RTTI_OVERRIDE_IMPL(red::ActorDonutBlock, ActorMultiState)

CREATE_STATE_ID(red::ActorDonutBlock, Idle)

red::ActorDonutBlock::ActorDonutBlock(const ActorCreateParam& param)
    : ChikuwaBlockBase(param)
{ }

ActorBase::Result red::ActorDonutBlock::create() {
    loadActorRes();
    
    mSpawnPos = mPos;
    
    static constexpr sead::Vector2f points[2] = {
        { -8.0f, 0.0f },
        {  8.0f, 0.0f }
    };
    
    mCollider.set(this, {
        .pos_offset = { 0.0f, 16.0f },
        .rot_pivot_offset = { 0.0f, 0.0f },
        .points = points,
        .angle = mAngle.z(),
    });
    
    mCollider.setSlipAttr(BgUnitCode::cSlipAttr_NoSuberu);
    mCollider.setType(BgCollision::cType_DonutBlock);
    mCollider.set_170(8.0f);
    mCollider.setCallback(&ChikuwaBlockBase::stepCallback, nullptr, nullptr);
    
    reviveCollisionCheck();
    
    init();
    
    mSize = { 32.0f, 32.0f };
    mVisibleAreaSize = { 32.0f, 32.0f };
    
    changeState(StateID_Idle);
    updateModel(8.0f);
    
    return cResult_Success;
}

bool red::ActorDonutBlock::execute() {
    if (mFallDelayTimer != 0) {
        mFallDelayTimer--;
    }
    
    executeState();
    mCollider.execute();
    
    resetStep();
    updateModel(8.0f);
    
    return true;
}

void red::ActorDonutBlock::loadActorRes() {
    ChikuwaBlockBase::loadActorRes();
}

void red::ActorDonutBlock::reviveCollisionCheck() {
    ActorBgCollisionMgr::instance()->entry(mCollider);
}

void red::ActorDonutBlock::reset1() {
    changeState(StateID_Idle);
}

void red::ActorDonutBlock::reset2() {
    reviveCollisionCheck();
}

void red::ActorDonutBlock::initializeState_Idle() {
    mShakeAngleZ = 0;
    mShakeDelayTimer = 0;
    mShakeXOffset = 0;
    
    mTexAnim->getFrameCtrl().setFrame(0.0f);
}

void red::ActorDonutBlock::executeState_Idle() {
    if (hasContact()) {
        init();
        
        mTexAnim->getFrameCtrl().setFrame(1.0f);
        
        changeState(StateID_Step);
    }
}

void red::ActorDonutBlock::finalizeState_Idle() {
    
}
