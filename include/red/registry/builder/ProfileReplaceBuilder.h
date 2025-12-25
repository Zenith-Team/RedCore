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
    { }
    
    Profile* build() {
        Profile* profilePtr = Profile::get(mID);
        PublicProfile* profile = reinterpret_cast<PublicProfile*>(profilePtr);
        
        profile->factory = &TActorFactory<T>;
        profile->actor_create_info = this->mCreateInfo;
        profile->is_res_loaded = false;
        profile->flag = this->mFlag;
    
        return profilePtr;
    }

private:
    s32 mID;
};

}
