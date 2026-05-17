#pragma once

#include <red/event_sys/EventBase.h>
#include <red/event_sys/EventDelegator.h>
#include <actor/ActorBase.h>

namespace red {
    
    class ActorCreateEvent : public EventBase<ActorCreateEvent> {
    public:
        using Listener = EventBase<ActorCreateEvent>::BasicListener;
    
    public:
        [[nodiscard]]
        ActorBase* getActor() const { return mActor; }
    
    private:
        explicit ActorCreateEvent(ActorBase* actor)
            : mActor(actor)
        { }
    
    public:
        static void subscribe(EventDelegator<ActorCreateEvent>::Listener& listener);
        static void emit(ActorBase* actor);
    
    private:
        [[nodiscard]]
        static Delegator& getDelegator();
    
    private:
        ActorBase* mActor;
    };
    
}
