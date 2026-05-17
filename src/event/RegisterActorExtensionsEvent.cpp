#include <red/event/RegisterActorExtensionsEvent.h>
#include <red/extension/ActorClassExtender.h>
#include <red/extension/ClassExtension.h>
#include <red/heap/RedCoreHeap.h>

red::ClassExtension& red::RegisterActorExtensionsEvent::extend(const sead::RuntimeTypeInfo::Interface* typeInfo) {
    // TODO: what??, make this good
    for (auto& extension : ActorClassExtender::sActorExtensions) {
        if (extension->getTypeInfo() == typeInfo) {
            return *extension;
        }
    }
    
    ClassExtension* extension = new(RedCoreHeap::instance()) ClassExtension(typeInfo);
    ActorClassExtender::sActorExtensions.pushBack(extension);
    return *extension;
}

red::RegisterActorExtensionsEvent::Delegator& red::RegisterActorExtensionsEvent::getDelegator() {
    static Delegator sDelegator;
    return sDelegator;
}

void red::RegisterActorExtensionsEvent::subscribe(EventDelegator<RegisterActorExtensionsEvent>::Listener& listener) {
    getDelegator().connect(listener);
}

void red::RegisterActorExtensionsEvent::emit() {
    RegisterActorExtensionsEvent event;
    getDelegator().fire(event);
}
