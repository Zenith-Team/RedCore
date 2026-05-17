#pragma once

#include <actor/ActorUniqueID.h>
#include <container/seadTreeMap.h>
#include <container/seadTList.h>
#include <prim/seadRuntimeTypeInfo.h>

class ActorBase;

namespace red {

    class ClassExtension;
    class ClassExtensionInstance;
    
    class ActorClassExtender {
    public:
        [[nodiscard]]
        static ClassExtensionInstance* get(ActorUniqueID id, const sead::RuntimeTypeInfo::Interface* typeInfo);
        
        template <typename T>
        [[nodiscard]]
        static ClassExtensionInstance* get(ActorUniqueID id) {
            return get(id, T::getRuntimeTypeInfoStatic());
        }

    public: // hooks
        static void init();
        static void onCreate(ActorBase* actor);
        static void onDelete(ActorBase* actor);
    
    private:
        friend class RegisterActorExtensionsEvent;
    
        static sead::TList<red::ClassExtension*> sActorExtensions;
        static sead::FixedTreeMap<u32, sead::TList<red::ClassExtensionInstance*>, 512> sActorExtensionInstances;
    };

}
