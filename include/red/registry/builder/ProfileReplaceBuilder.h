#pragma once

#include <concepts>

#include <actor/ProfileInfo.h>
#include <actor/Actor.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/public/Profile.h>

namespace red {

/**
 * @brief Builder for replacing a vanilla profile with a custom one by its numeric ID.
 * @tparam T Actor class that the profile will now instantiate.
 */
template <class T> requires std::derived_from<T, ActorBase>
class ProfileReplaceBuilder : public ProfileBuilder<ProfileReplaceBuilder<T>> {
public:
    /**
     * @brief Begins replacing a vanilla profile.
     * @param id The target profile ID to replace.
     */
    explicit ProfileReplaceBuilder(s32 id)
        : ProfileBuilder<ProfileReplaceBuilder<T>>()
        , mID(id)
    {
        if (id > ProfileInfo::cProfileID_Max) {
            tk::fatal("ERROR: Attempting to replace invalid vanilla profile ID: %i", id);
        }
    }

    /**
     * @brief Completes the builder by replacing the vanilla profile.
     * @return The profile which now contains the replaced data.
     */
    Profile* build() {
        if (mID > ProfileInfo::cProfileID_Max) {
            return nullptr;
        }
        
        pub::Profile* profile = static_cast<pub::Profile*>(Profile::get(mID));
        
        profile->mFactory = &TActorFactory<T>;
        profile->mActorCreateInfo = this->mCreateInfo != nullptr ? this->mCreateInfo : &ActorCreateInfo::cDefault;
        profile->mIsResLoaded = false;
        profile->mFlag = this->mFlag;
        
        ProfileEx::setExecutePriority(mID, this->mExecutePriority);
    
        return profile;
    }

private:
    s32 mID; ///< The target profile ID to replace.
};

}
