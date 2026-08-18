#pragma once

#include <red/public/ProfileInfo.h>
#include <telkin/Privilege.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/public/Profile.h>
#include <red/profile/ProfileEx.h>
#include <red/event/StaticInitEvent.h>

namespace red {

/**
 * @brief Builder for partially modifying fields on a vanilla profile by its numeric ID.
 * @details Intended to be constructed via a per-mod @c red::Registrar object.
 * @tparam ID The target profile ID to modify.
 */
template <s32 ID>
class ProfileEditBuilder : public ProfileBuilder<ProfileEditBuilder<ID>> {
public:
    static_assert(ID <= ProfileInfo::cProfileID_Max, "ERROR: Attempting to edit an invalid vanilla profile ID.");

    ProfileEditBuilder()
        : ProfileBuilder<ProfileEditBuilder<ID>>()
    { }

    /**
     * @brief Completes the builder by applying the changes to the vanilla profile.
     * @return The vanilla profile which was modified.
     */
    Profile* build() {
        static bool instantiated = false;
        if (instantiated) {
            tk::fatal("Cannot reuse the same template instanciation for two edits.");
        }
        instantiated = true;

        pub::Profile* profile = static_cast<pub::Profile*>(Profile::get(ID));

        // defer until the vanilla profile is inited so we have the final say
        static const struct {
            const ActorCreateInfo* mCreateInfo;
            bool mCreateInfoModified;
            Profile::Flag mFlag;
            bool mFlagModified;
            s16 mDrawPriority;
            bool mDrawPriorityModified;
            s16 mExecutePriority;
            bool mExecutePriorityModified;
        } sSnapshot = {
            this->mCreateInfo, this->mCreateInfoModified,
            this->mFlag, this->mFlagModified,
            this->mDrawPriority, this->mDrawPriorityModified,
            this->mExecutePriority, this->mExecutePriorityModified
        };

        static red::StaticInitEvent::Listener listener([](red::StaticInitEvent&) {
            pub::Profile* profile = static_cast<pub::Profile*>(Profile::get(ID));

            if (sSnapshot.mCreateInfoModified) {
                profile->mActorCreateInfo = sSnapshot.mCreateInfo;
            }

            if (sSnapshot.mFlagModified) {
                profile->mFlag = sSnapshot.mFlag;
            }

            if (sSnapshot.mDrawPriorityModified) {
                //s16* priorities = const_cast<s16*>(ProfileInfo::cDrawPriority);
                //priorities[ID] = mDrawPriority;
                tk::privilegedWrite(pub::ProfileInfo::cDrawPriority + ID, &sSnapshot.mDrawPriority, sizeof(s16));
            }

            if (sSnapshot.mExecutePriorityModified) {
                ProfileEx::setExecutePriority(ID, sSnapshot.mExecutePriority);
            }
        });

        return profile;
    }

};

}
