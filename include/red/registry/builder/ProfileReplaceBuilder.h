#pragma once

#include <concepts>

#include <actor/ProfileInfo.h>
#include <actor/Actor.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/public/Profile.h>
#include <red/profile/ProfileEx.h>
#include <red/event/StaticInitEvent.h>

namespace red {

/**
 * @brief Builder for replacing a vanilla profile with a custom one by its numeric ID.
 * @tparam T Actor class that the profile will now instantiate.
 * @tparam ID The target profile ID to replace.
 */
template <class T, s32 ID> requires std::derived_from<T, ActorBase>
class ProfileReplaceBuilder : public ProfileBuilder<ProfileReplaceBuilder<T, ID>> {
public:
    static_assert(ID <= ProfileInfo::cProfileID_Max, "ERROR: Attempting to replace an invalid vanilla profile ID.");

    ProfileReplaceBuilder()
        : ProfileBuilder<ProfileReplaceBuilder<T, ID>>()
    { }

    /**
     * @brief Completes the builder by replacing the vanilla profile.
     * @return The profile which now contains the replaced data.
     */
    Profile* build() {
        static bool instantiated = false;
        if (instantiated) {
            tk::fatal("Cannot reuse the same template instanciation for two replacements.");
        }
        instantiated = true;

        // defer until the vanilla profile is inited so we have the final say
        static const struct {
            const ActorCreateInfo* mCreateInfo;
            Profile::Flag mFlag;
            s16 mExecutePriority;
        } sSnapshot = { this->mCreateInfo, this->mFlag, this->mExecutePriority };

        static red::StaticInitEvent::Listener listener([](red::StaticInitEvent&) {
            pub::Profile* profile = static_cast<pub::Profile*>(Profile::get(ID));

            profile->mFactory = &TActorFactory<T>;
            profile->mActorCreateInfo = sSnapshot.mCreateInfo != nullptr ? sSnapshot.mCreateInfo : &ActorCreateInfo::cDefault;
            profile->mIsResLoaded = false;
            profile->mFlag = sSnapshot.mFlag;

            ProfileEx::setExecutePriority(ID, sSnapshot.mExecutePriority);
        });

        return ProfileEx::get(ID);
    }
};

}
