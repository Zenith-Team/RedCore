#define PROFILE_INFO_AS_NAMESPACE
#include <actor/ProfileInfo.h>

#include <red/registry/builder/ProfileBuilder.h>
#include <red/util/PublicProfile.h>

namespace red {
    
class ProfileEditBuilder : public ProfileBuilder<ProfileEditBuilder> {
public:
    ProfileEditBuilder(s32 id)
        : mID()
    { }

    Profile* build() {
        Profile* profilePtr = Profile::get(mID);
        PublicProfile* profile = reinterpret_cast<PublicProfile*>(profilePtr);

        if (mCreateInfoModified) {
            profile->actor_create_info = mCreateInfo;
        }
        
        if (mFlagModified) {
            profile->flag = mFlag;
        }
        
        if (mDrawPriorityModified) {
            s16* priorities = const_cast<s16*>(ProfileInfo::cDrawPriority);
            priorities[mID] = mDrawPriority;
        }
        
        return profilePtr;
    }

private:
    s32 mID;
};

}
