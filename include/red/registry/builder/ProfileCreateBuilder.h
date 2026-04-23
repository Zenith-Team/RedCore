#pragma once

#include <concepts>
#include <actor/ProfileInfo.h>
#include <actor/Actor.h>
#include <red/registry/builder/ProfileBuilder.h>
#include <red/profile/ProfileEx.h>

namespace red {

/**
 * Builder for creating new custom profiles to be registered by string-identifier.
 * @tparam T Target actor class that this profile represents and will instantiate.
 * @details Only intended to be constructed via a per-mod @c red::Registrar object which passes the namespace.
 */
template <class T> requires std::derived_from<T, ActorBase>
class ProfileCreateBuilder : public ProfileBuilder<ProfileCreateBuilder<T>> {
public:
    /**
     * Begins creating a new custom profile.
     * @param identifier The namespaced identifier of this profile.
     */
    explicit ProfileCreateBuilder(const sead::SafeString* identifier)
        : ProfileBuilder<ProfileCreateBuilder<T>>()
        , mIdentifier(identifier)
    { }

    /**
     * Completes the builder by registering the new profile.
     * @return The newly constructed profile object.
     */
    Profile* build() {
        static Profile sProfile = Profile(&TActorFactory<T>, -1, *mIdentifier, this->mCreateInfo, this->mFlag);
        ProfileEx::setDrawPriority(*mIdentifier, this->mDrawPriority);
        ProfileEx::setExecutePriority(*mIdentifier, this->mExecutePriority);
        ProfileEx::setResources(*mIdentifier, this->mResourceType, this->mResources, this->mResourceCount);

        return &sProfile;
    }

private:
    const sead::SafeString* mIdentifier; ///< The namespaced identifier of the new profile to register.
};

}
