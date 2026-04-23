#pragma once

#include <red/public/ProfileInfo.h>
#include <telkin/Privilege.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/public/Profile.h>

namespace red {

/**
 * @brief Builder for partially modifying fields on a vanilla profile by its numeric ID.
 * @details Intended to be constructed via a per-mod @c red::Registrar object.
 */
class ProfileEditBuilder : public ProfileBuilder<ProfileEditBuilder> {
public:
    /**
     * @brief Begins editing a vanilla profile.
     * @param id The target profile ID to modify.
     */
    explicit ProfileEditBuilder(s32 id)
        : mID(id)
    { }

    /**
     * @brief Completes the builder by applying the changes to the vanilla profile.
     * @return The vanilla profile which was modified.
     */
    Profile* build() {
        pub::Profile* profile = static_cast<pub::Profile*>(Profile::get(mID));

        if (mCreateInfoModified) {
            profile->mActorCreateInfo = mCreateInfo;
        }
        
        if (mFlagModified) {
            profile->mFlag = mFlag;
        }
        
        if (mDrawPriorityModified) {
            //s16* priorities = const_cast<s16*>(ProfileInfo::cDrawPriority);
            //priorities[mID] = mDrawPriority;
            tk::privilegedWrite(pub::ProfileInfo::cDrawPriority + mID, &mDrawPriority, sizeof(s16));
        }
        
        if (mExecutePriorityModified) {
            ProfileEx::setExecutePriority(mID, mExecutePriority);
        }
        
        return profile;
    }

private:
    s32 mID; ///< The target profile ID to modify.
};

}
