#pragma once

#include <red/event_sys/EventBase.h>
#include <red/event_sys/EventDelegator.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <red/extension/ClassExtension.h>
#include <red/extension/ActorClassExtender.h>

namespace red {
    
    class ClassExtension;
    
    class RegisterActorExtensionsEvent : public EventBase<RegisterActorExtensionsEvent> {
    public:
    class Listener {
        public:
            Listener(ListenerFunc f, s32 priority = 0)
                : mListener(f, priority)
            {
                RegisterActorExtensionsEvent::subscribe(mListener);
            }
            
        private:
            EventDelegator<RegisterActorExtensionsEvent>::Listener mListener;
        };
    
    public:
        [[nodiscard]]
        ClassExtension& extend(const sead::RuntimeTypeInfo::Interface* typeInfo);
    
    public:
        static void subscribe(EventDelegator<RegisterActorExtensionsEvent>::Listener& listener);
        static void emit();
    
    private:
        [[nodiscard]]
        static Delegator& getDelegator();
    };
    
}
