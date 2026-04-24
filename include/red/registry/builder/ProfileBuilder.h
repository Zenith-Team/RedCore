#pragma once

#include <actor/ProfileInfo.h>
#include <basis/seadTypes.h>
#include <actor/Profile.h>
#include <telkin/Print.h>

namespace red {

template <size_t N>
struct ComptimeFixedString {
    char mValue[N];

    constexpr ComptimeFixedString(const char (&str)[N]) {
        for (size_t i = 0; i < N; i++) {
            mValue[i] = str[i];
        }
    }

    constexpr const char* cstr() const {
        return mValue;
    }
};

template <size_t N>
ComptimeFixedString(const char (&)[N]) -> ComptimeFixedString<N>;

/**
 * @brief Base class for builder objects that operate on profiles. Tracks which fields were set and holds their data.
 * @tparam Derived The type of builder which inherits this one. Used for casting for the builder pattern.
 */
template <typename Derived> //requires std::derived_from<T, ProfileBuilder>
class ProfileBuilder {
public:
    /**
     * @brief Set the @c ActorCreateInfo field in the profile.
     * @return The builder object for chaining calls.
     */
    [[nodiscard]]
    Derived& createInfo(const ActorCreateInfo* createInfo) {
        if (mCreateInfoModified) {
            tk::println("WARNING: .createInfo() called twice");
        }
        
        mCreateInfoModified = true;
        mCreateInfo = createInfo;
        return *static_cast<Derived*>(this);
    }

    /**
     * @brief Set the @c Flag field in the profile.
     * @return The builder object for chaining calls.
     */
    [[nodiscard]]
    Derived& flag(const Profile::Flag flag) {
        if (mFlagModified) {
            tk::println("WARNING: .flag() called twice");
        }
        
        mFlagModified = true;
        mFlag = flag;
        return *static_cast<Derived*>(this);
    }

    /**
     * @brief Set the @c drawPriority field in the profile.
     * @return The builder object for chaining calls.
     */
    [[nodiscard]]
    Derived& drawPriority(const s16 drawPriority) {
        if (mDrawPriorityModified) {
            tk::println("WARNING: .drawPriority() called twice");
        }
        
        mDrawPriorityModified = true;
        mDrawPriority = drawPriority;
        return *static_cast<Derived*>(this);
    }

    /**
     * @brief Set the @c executePriority field in the profile.
     * @note This is a custom field added by RedCore.
     * @return The builder object for chaining calls.
     */
    [[nodiscard]]
    Derived& executePriority(const s16 executePriority) {
        if (mExecutePriorityModified) {
            tk::println("WARNING: .executePriority() called twice");
        }
        
        mExecutePriorityModified = true;
        mExecutePriority = executePriority;
        return *static_cast<Derived*>(this);
    }

    /**
     * @brief Set the @c resources field in the profile.
     * @tparam Args Comma-separated list of string literals representing which resources to load.
     * @param type At what stage of the game to load these resources.
     * @return The builder object for chaining calls.
     */
    template <ComptimeFixedString... Args> requires (sizeof...(Args) > 0)
    [[nodiscard]]
    Derived& resources(ProfileInfo::ResType type) {
        static sead::SafeString sResources[] = { sead::SafeString(Args.cstr())... };
        static bool instanciationUsed = false;
        if (instanciationUsed) {
            tk::println("WARNING: SafeArray instanciation already used. Might be bad...");
        }
        instanciationUsed = true;
        
        mResourceCount = static_cast<u8>(sizeof...(Args));
        mResources = sResources;
        mResourceType = type;
        
        return *static_cast<Derived*>(this);
    }
    
protected:
    const ActorCreateInfo* mCreateInfo = nullptr;                       ///< The @c createInfo data for the profile we are building.
    Profile::Flag mFlag = Profile::cFlag_None;                          ///< The @c flag data for the profile we are building.
    s16 mDrawPriority = ProfileInfo::cProfileID_Max;                    ///< The @c drawPriority data for the profile we are building.
    s16 mExecutePriority = ProfileInfo::cProfileID_Max;                 ///< The @c createInfo data for the profile we are building. This is a custom field added by RedCore.
    u8 mResourceCount = 0;                                              ///< The entry for @c ProfileInfo::cResNum for the profile we are building. This is stored separately for custom profiles.
    sead::SafeString* mResources = nullptr;                             ///< The entry for @c ProfileInfo::cResList for the profile we are building. This is stored separately for custom profiles.
    ProfileInfo::ResType mResourceType = ProfileInfo::cResType_Course;  ///< The entry for @c ProfileInfo::cResType for the profile we are building. This is stored separately for custom profiles.
    bool mCreateInfoModified = false;                                   ///< Whether @c mCreateInfo was modified. Used for validation and partial application in @c red::ProfileEditBuilder.
    bool mFlagModified = false;                                         ///< Whether @c mFlag was modified. Used for validation and partial application in @c red::ProfileEditBuilder.
    bool mDrawPriorityModified = false;                                 ///< Whether @c mDrawPriority was modified. Used for validation and partial application in @c red::ProfileEditBuilder.
    bool mExecutePriorityModified = false;                              ///< Whether @c mExecutePriority was modified. Used for validation and partial application in @c red::ProfileEditBuilder.
    bool mResourcesModified = false;                                    ///< Whether @c mResources was modified. Used for validation and partial application in @c red::ProfileEditBuilder.
};

}
