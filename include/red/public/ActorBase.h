#pragma once

#include <actor/ActorBase.h>

namespace red::pub {
    
    class ActorBase : public ::ActorBase {
    public:
        using ::ActorBase::preCreate;
        using ::ActorBase::create;
        using ::ActorBase::postCreate;
        
        using ::ActorBase::preExecute;
        using ::ActorBase::execute;
        using ::ActorBase::postExecute;
        using ::ActorBase::finalUpdate;
        
        using ::ActorBase::preDraw;
        using ::ActorBase::draw;
        using ::ActorBase::postDraw;
        
        using ::ActorBase::preDelete;
        using ::ActorBase::doDelete;
        using ::ActorBase::postDelete;
        
        using ::ActorBase::setActive_;
        
        using ::ActorBase::mActorHeap;
        using ::ActorBase::mActorUniqueID;
        using ::ActorBase::mActorProfile;
        using ::ActorBase::mCreatedImmediately;
        using ::ActorBase::mIsMapActor;
        using ::ActorBase::mIsActive;
        using ::ActorBase::mDeleteRequestFlag;
        using ::ActorBase::mParam0;
        using ::ActorBase::mParam1;
        using ::ActorBase::mParamEx;
        using ::ActorBase::mChildList;
        using ::ActorBase::mChildNode;
        using ::ActorBase::mParent;
        using ::ActorBase::mExecuteNode;
        using ::ActorBase::mDrawNode;
        using ::ActorBase::mFlag;
    };
    
}
