#pragma once

#define PROFILE_INFO_AS_NAMESPACE
#include <actor/ProfileInfo.h>
#include <basis/seadTypes.h>
#include <actor/Profile.h>

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

template <class T> //requires std::derived_from<T, ProfileBuilder>
class ProfileBuilder {
public:
    [[nodiscard]]
    T& createInfo(const ActorCreateInfo* createInfo) {
        if (mCreateInfoModified) {
            OSReport("WARNING: .createInfo() called twice\n");
        }
        
        mCreateInfoModified = true;
        mCreateInfo = createInfo;
        return *this;
    }

    [[nodiscard]]
    T& flag(u32 flag) {
        if (mFlagModified) {
            OSReport("WARNING: .flag() called twice\n");
        }
        
        mFlagModified = true;
        mFlag = flag;
        return *this;
    }

    [[nodiscard]]
    T& drawPriority(s16 drawPriority) {
        if (mDrawPriorityModified) {
            OSReport("WARNING: .drawPriority() called twice\n");
        }
        
        mDrawPriorityModified = true;
        mDrawPriority = drawPriority;
        return *this;
    }
    
    template <ComptimeFixedString... Args> requires (sizeof...(Args) > 0)
    [[nodiscard]]
    T& resources(ProfileInfo::ResType type) {
        static const sead::SafeArray<sead::SafeString, sizeof...(Args)> sResources{ Args.cstr()... };
        static bool instanciationUsed = false;
        if (instanciationUsed) {
            OSReport("WARNING: SafeArray instanciation already used. Might be bad...\n");
        }
        instanciationUsed = true;
        
        mResourceCount = static_cast<u8>(sResources.size());
        mResources = sResources.getBufferPtr();
        mResourceType = type;
        
        return *this;
    }
    
protected:
    const ActorCreateInfo* mCreateInfo = nullptr;
    u32 mFlag = 0;
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
