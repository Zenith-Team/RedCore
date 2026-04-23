#pragma once

#include <actor/ActorBase>
#include <red/registry/builder/ProfileCreateBuilder.h>
#include <red/registry/builder/ProfileReplaceBuilder.h>
#include <red/registry/builder/ProfileEditBuilder.h>
#include <concepts>

namespace red {

/**
 * @brief Per-mod registrar object from which all registrations must be made.
 * @details Every mod should construct and maintain its own global @c red::Registrar with a unique namespace.
 */
class Registrar {
public:
    /**
     * Construct a registrar using a namespace.
     * @param nameSpace The unique namespace for the mod.
     */
    explicit Registrar(const char* nameSpace)
        : mNamespace(nameSpace)
    { }

    /**
     * @brief Register a new profile by string-name.
     * @tparam T Target actor class that this profile represents and will instantiate.
     * @param name The identifier of this profile, to which the namespace will automatically be prefixed.
     * @return A builder on which additional methods for setting parameters are available.
     * @warning One profile per instantiation of T, meaning if two profiles with the same class are desired, a second class that inherits the first is required to force creation of a new template instance.
     */
    template <class T> requires std::derived_from<T, ActorBase>
    ProfileCreateBuilder<T> newProfile(const char* name) const {
        static char buf[ProfileEx::cNameMaxLen + 1];
        static sead::BufferedSafeString sIdentifier{buf, ProfileEx::cNameMaxLen};
        sIdentifier.format("%s:%s", mNamespace, name);
        
        return ProfileCreateBuilder<T>(&sIdentifier);
    }

    /**
     * @brief Replace a vanilla profile by its numeric ID.
     * @tparam T Actor class that this profile will now instantiate.
     * @param id The target profile ID to replace.
     * @return A builder on which additional methods for setting parameters are available.
     */
    template <class T> requires std::derived_from<T, ActorBase>
    ProfileReplaceBuilder<T> replaceProfile(s32 id) const {
        return ProfileReplaceBuilder<T>(id);
    }

    /**
     * @brief Partially modify fields on a vanilla profile by its numeric ID.
     * @param id The target profile ID to modify.
     * @return A builder on which additional methods for setting parameters are available.
     */
    ProfileEditBuilder editProfile(s32 id) const {
        return ProfileEditBuilder(id);
    }

private:
    const char* mNamespace; ///< The unique namespace for this mod.
};

}
