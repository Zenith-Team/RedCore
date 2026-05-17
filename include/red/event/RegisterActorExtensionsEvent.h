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
        using Listener = EventBase<RegisterActorExtensionsEvent>::BasicListener;
    
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
