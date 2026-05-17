#include <red/extension/ActorClassExtender.h>
#include <red/extension/ClassExtension.h>
#include <red/event/RegisterActorExtensionsEvent.h>
#include <actor/ActorBase.h>
#include <red/event/TaskPrepareEvent.h>
#include <red/event/ActorCreateEvent.h>
#include <red/event/ActorDeleteEvent.h>

sead::TList<red::ClassExtension*> red::ActorClassExtender::sActorExtensions;
sead::FixedTreeMap<u32, sead::TList<red::ClassExtensionInstance*>, 512> red::ActorClassExtender::sActorExtensionInstances;

void red::ActorClassExtender::init() {
    // collect registrations from mods
    red::RegisterActorExtensionsEvent::emit();
    
    // finalize the blueprints
    for (auto& extension : sActorExtensions) {
        extension->finish();
    }
}

namespace red {

TaskPrepareEvent::Listener<TaskPrepareEvent::Stage::Before> InitActorClassExtender([](TaskPrepareEvent& e) {
    if (e.getTask()->getName() != "RootTask")
        return;
    
    red::ActorClassExtender::init();
});

} // namespace red

red::ClassExtensionInstance* red::ActorClassExtender::get(ActorUniqueID id, const sead::RuntimeTypeInfo::Interface* typeInfo) {
    auto bucketForThisActor = sActorExtensionInstances.find(id.getValue());
    if (bucketForThisActor == nullptr) [[unlikely]] {
        tk::fatal("Tried to get extension data for actor 0x%X, but it has no extensions", id.getValue());
    }
    
    for (ClassExtensionInstance* extension : *bucketForThisActor) {
        if (extension->getTypeInfo() == typeInfo) {
            return extension;
        }
    }
    
    tk::fatal("Tried to get extension data for actor 0x%X, but it doesn't exist", id.getValue());
    return nullptr;
}

void red::ActorClassExtender::onCreate(ActorBase *actor) {
    const u32 key = actor->getActorUniqueID().getValue();
    for (const auto& extension : sActorExtensions) {
        if (extension->getSize() == 0) {
            tk::fatal("Extension has no members. Don't call extend() without adding members.");
            continue;
        }
        
        // actor may have multiple extensions which apply to it, if the base classes has extensions too
        if (actor->checkDerivedRuntimeTypeInfo(extension->getTypeInfo())) {
            // find the bucket for this actor by ID, or make a bucket for it if it doesn't exist
            auto bucketForThisActor = sActorExtensionInstances.find(key);
            if (bucketForThisActor == nullptr) {
                sActorExtensionInstances.insert(key);
                bucketForThisActor = sActorExtensionInstances.find(key);
            }
            
            // allocate the buffer which holds the extension data
            ClassExtensionInstance* ext = new(actor->getActorHeap()) ClassExtensionInstance(
                extension->getSize(),
                new(actor->getActorHeap(), extension->getAlignment()) u8[extension->getSize()],
                extension->getTypeInfo()
            );

            // add the attachment
            bucketForThisActor->pushBack(ext);
        }
    }
}


namespace red {

ActorCreateEvent::Listener ActorClassExtenderCreate([](ActorCreateEvent& e) {
    ActorClassExtender::onCreate(e.getActor());
});

} // namespace red

void red::ActorClassExtender::onDelete(ActorBase *actor) {
    auto bucketForThisActor = sActorExtensionInstances.find(actor->getActorUniqueID().getValue());
    if (bucketForThisActor == nullptr) {
        return;
    }
    
    // extension data automatically freed when the heap is destroyed
    
    sActorExtensionInstances.erase(actor->getActorUniqueID().getValue());
}

namespace red {

ActorDeleteEvent::Listener ActorClassExtenderDelete([](ActorDeleteEvent& e) {
    ActorClassExtender::onDelete(e.getActor());
});

} // namespace red

// usage:

//register some extended members:

// // inited in the register phase, then use it whenever later to lookup the member
// u32 sActorBase_mHealth;

// //vv event handler on boot
// void registerActorExtensions(red::RegisterActorExtensionsEvent& e) {
//     red::ClassExtension& actorbaseExtension = e.extend(ActorBase::getRuntimeTypeInfoStatic());
//     sActorBase_mHealth = actorbaseExtension.add<u32>(); // <- returns offset
// }

// //read them later at any time:
// void someHook(ActorBase* actor) {
//     red::ClassExtensionInstance* actorbaseExtension = red::ActorClassExtender::get(actor->getActorUniqueID(), ActorBase::getRuntimeTypeInfoStatic());
    
//     u32 health = *actorbaseExtension->get<u32>(sActorBase_mHealth);
//     *actorbaseExtension->get<u32>(sActorBase_mHealth) = health * 2;
// }
