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

template <typename Derived> //requires std::derived_from<T, ProfileBuilder>
class ProfileBuilder {
public:
    [[nodiscard]]
    Derived& createInfo(const ActorCreateInfo* createInfo) {
        if (mCreateInfoModified) {
            tk::print("WARNING: .createInfo() called twice\n");
        }
        
        mCreateInfoModified = true;
        mCreateInfo = createInfo;
        return *static_cast<Derived*>(this);
    }

    [[nodiscard]]
    Derived& flag(const Profile::Flag flag) {
        if (mFlagModified) {
            tk::print("WARNING: .flag() called twice\n");
        }
        
        mFlagModified = true;
        mFlag = flag;
        return *static_cast<Derived*>(this);
    }

    [[nodiscard]]
    Derived& drawPriority(const s16 drawPriority) {
        if (mDrawPriorityModified) {
            tk::print("WARNING: .drawPriority() called twice\n");
        }
        
        mDrawPriorityModified = true;
        mDrawPriority = drawPriority;
        return *static_cast<Derived*>(this);
    }
    
    template <ComptimeFixedString... Args> requires (sizeof...(Args) > 0)
    [[nodiscard]]
    Derived& resources(ProfileInfo::ResType type) {
        static sead::SafeString sResources[] = { sead::SafeString(Args.cstr())... };
        static bool instanciationUsed = false;
        if (instanciationUsed) {
            tk::print("WARNING: SafeArray instanciation already used. Might be bad...\n");
        }
        instanciationUsed = true;
        
        mResourceCount = static_cast<u8>(sizeof...(Args));
        mResources = sResources;
        mResourceType = type;
        
        return *static_cast<Derived*>(this);
    }
    
protected:
    const ActorCreateInfo* mCreateInfo = nullptr;
    Profile::Flag mFlag = Profile::cFlag_None;
    s16 mDrawPriority = ProfileInfo::cProfileID_Max;
    u8 mResourceCount = 0;
    sead::SafeString* mResources = nullptr;
    ProfileInfo::ResType mResourceType = ProfileInfo::cResType_Course;
    bool mCreateInfoModified = false;
    bool mFlagModified = false;
    bool mDrawPriorityModified = false;
    bool mResourcesModified = false;
};

}
