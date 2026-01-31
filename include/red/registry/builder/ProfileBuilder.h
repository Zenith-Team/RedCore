#pragma once

#define PROFILE_INFO_AS_NAMESPACE
#include <actor/ProfileInfo.h>
#include <basis/seadTypes.h>
#include <actor/Profile.h>
#include <red/util/Log.h>

namespace red {

template <s32 N>
class ComptimeFixedString {
public:
    constexpr ComptimeFixedString(const char (&str)[N]) {
        for (s32 i = 0; i < N; i++) {
            mValue[i] = str[i];
        }
    }

    constexpr const char* cstr() const {
        return mValue;
    }

private:
    char mValue[N];
};

template <typename Derived> //requires std::derived_from<T, ProfileBuilder>
class ProfileBuilder {
public:
    [[nodiscard]]
    Derived& createInfo(const ActorCreateInfo* createInfo) {
        if (mCreateInfoModified) {
            red::print("WARNING: .createInfo() called twice\n");
        }
        
        mCreateInfoModified = true;
        mCreateInfo = createInfo;
        return *static_cast<Derived*>(this);
    }

    [[nodiscard]]
    Derived& flag(const Profile::Flag flag) {
        if (mFlagModified) {
            red::print("WARNING: .flag() called twice\n");
        }
        
        mFlagModified = true;
        mFlag = flag;
        return *static_cast<Derived*>(this);
    }

    [[nodiscard]]
    Derived& drawPriority(const s16 drawPriority) {
        if (mDrawPriorityModified) {
            red::print("WARNING: .drawPriority() called twice\n");
        }
        
        mDrawPriorityModified = true;
        mDrawPriority = drawPriority;
        return *static_cast<Derived*>(this);
    }
    
    template <ComptimeFixedString... Args> requires (sizeof...(Args) > 0)
    [[nodiscard]]
    Derived& resources(ProfileInfo::ResType type) {
        static const sead::SafeArray<sead::SafeString, sizeof...(Args)> sResources{ Args.cstr()... };
        static bool instanciationUsed = false;
        if (instanciationUsed) {
            red::print("WARNING: SafeArray instanciation already used. Might be bad...\n");
        }
        instanciationUsed = true;
        
        mResourceCount = static_cast<u8>(sResources.size());
        mResources = sResources.getBufferPtr();
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
