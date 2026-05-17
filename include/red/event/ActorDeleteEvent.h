#pragma once

#include <red/event_sys/EventBase.h>
#include <red/event_sys/EventDelegator.h>
#include <actor/ActorBase.h>

namespace red {
    
    class ActorDeleteEvent : public EventBase<ActorDeleteEvent> {
    public:
        class Listener {
        public:
            Listener(ListenerFunc f, s32 priority = 0)
                : mListener(f, priority)
            {
                ActorDeleteEvent::subscribe(mListener);
            }
            
        private:
            EventDelegator<ActorDeleteEvent>::Listener mListener;
        };
    
    public:
        [[nodiscard]]
        ActorBase* getActor() const { return mActor; }
    
    private:
        explicit ActorDeleteEvent(ActorBase* actor)
            : mActor(actor)
        { }
    
    public:
        static void subscribe(EventDelegator<ActorDeleteEvent>::Listener& listener);
        static void emit(ActorBase* actor);
    
    private:
        [[nodiscard]]
        static Delegator& getDelegator();
    
    private:
        ActorBase* mActor;
    };
    
}
