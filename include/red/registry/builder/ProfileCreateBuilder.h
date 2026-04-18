#pragma once

#include <concepts>
#include <actor/ProfileInfo.h>
#include <actor/Actor.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/profile/ProfileEx.h>

namespace red {

template <class T> requires std::derived_from<T, ActorBase>
class ProfileCreateBuilder : public ProfileBuilder<ProfileCreateBuilder<T>> {
public:
    ProfileCreateBuilder(const sead::SafeString* identifier)
        : ProfileBuilder<ProfileCreateBuilder<T>>()
        , mIdentifier(identifier)
    { }

    Profile* build() {
        static Profile sProfile = Profile(&TActorFactory<T>, -1, *mIdentifier, this->mCreateInfo, this->mFlag);
        ProfileEx::setDrawPriority(*mIdentifier, this->mDrawPriority);
        ProfileEx::setResources(*mIdentifier, this->mResourceType, this->mResources, this->mResourceCount);

        return &sProfile;
    }

private:
    const sead::SafeString* mIdentifier;
};

}
