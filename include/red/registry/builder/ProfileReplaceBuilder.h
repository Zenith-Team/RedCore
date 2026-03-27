#pragma once

#include <concepts>

#define PROFILE_INFO_AS_NAMESPACE
#include <actor/ProfileInfo.h>
#include <actor/Actor.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/public/Profile.h>

namespace red {

template <class T> requires std::derived_from<T, ActorBase>
class ProfileReplaceBuilder : public ProfileBuilder<ProfileReplaceBuilder<T>> {
public:
    ProfileReplaceBuilder(s32 id)
        : ProfileBuilder<ProfileReplaceBuilder<T>>()
        , mID(id)
    {
        if (id > ProfileInfo::cProfileID_Max) {
            tk::print("ERROR: Attempting to replace invalid vanilla profile ID: %i\n", id);
        }
    }
    
    Profile* build() {
        if (mID > ProfileInfo::cProfileID_Max) {
            return nullptr;
        }
        
        pub::Profile* profile = static_cast<pub::Profile*>(Profile::get(mID));
        
        profile->mFactory = &TActorFactory<T>;
        profile->mActorCreateInfo = this->mCreateInfo != nullptr ? this->mCreateInfo : &ActorCreateInfo::cDefault;
        profile->mIsResLoaded = false;
        profile->mFlag = this->mFlag;
    
        return profile;
    }

private:
    s32 mID;
};

}
