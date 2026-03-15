#include <concepts>

#define PROFILE_INFO_AS_NAMESPACE
#include <actor/ProfileInfo.h>
#include <actor/Actor.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/util/PublicProfile.h>

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
        
        Profile* profilePtr = Profile::get(mID);
        PublicProfile* profile = reinterpret_cast<PublicProfile*>(profilePtr);
        
        profile->factory = &TActorFactory<T>;
        profile->actor_create_info = this->mCreateInfo != nullptr ? this->mCreateInfo : &ActorCreateInfo::cDefault;
        profile->is_res_loaded = false;
        profile->flag = this->mFlag;
    
        return profilePtr;
    }

private:
    s32 mID;
};

}
