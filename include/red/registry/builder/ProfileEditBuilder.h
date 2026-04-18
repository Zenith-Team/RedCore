#pragma once

#include <red/public/ProfileInfo.h>
#include <telkin/Privilege.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/public/Profile.h>

namespace red {
    
class ProfileEditBuilder : public ProfileBuilder<ProfileEditBuilder> {
public:
    ProfileEditBuilder(s32 id)
        : mID(id)
    { }

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
        
        return profile;
    }

private:
    s32 mID;
};

}
