#pragma once

#include <container/seadSafeArray.h>
#include <container/seadStrTreeMap.h>
#include <actor/ProfileInfo.h>
#include <actor/Profile.h>

namespace red {

class ProfileEx {
public:
    static constexpr s32 cNameMaxLen = 64;
    static constexpr s32 cMaxCustomProfiles = 4096;

public:
    [[nodiscard]]
    static Profile* get(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static Profile* get(s32 id);
    
    [[nodiscard]]
    static const char* getName(s32 id);
    
    [[nodiscard]]
    static s16 getDrawPriority(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static s16 getDrawPriority(s32 id);
    
    [[nodiscard]]
    static s16 getExecutePriority(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static s16 getExecutePriority(s32 id);
    
    [[nodiscard]]
    static ProfileInfo::ResType getResType(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static ProfileInfo::ResType getResType(s32 id);
    
    [[nodiscard]]
    static u32 getResNum(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static u32 getResNum(s32 id);
    
    [[nodiscard]]
    static const sead::SafeString* getResList(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static const sead::SafeString* getResList(s32 id);

public: //! The below are RedCore-internal APIs, do not use!
    static void addIdentifierProfile(const sead::SafeString& identifier, Profile* profile);
    static void setResources(const sead::SafeString& identifier, ProfileInfo::ResType resourceType, sead::SafeString* resources, u8 resourceCount);
    static void setName(s32 id, const char* name);
    static void setDrawPriority(const sead::SafeString& identifier, s16 priority);
    static void setExecutePriority(const sead::SafeString& identifier, s16 priority);
    static void setExecutePriority(s32 id, s16 priority);
    
    struct ResourceData {
        u8 resource_count = 0;
        const sead::SafeString* resources = nullptr;
        ProfileInfo::ResType resource_type = ProfileInfo::cResType_Course;
    };

private:
    class VanillaProfileExecutePriorities : public sead::SafeArray<s16, ProfileInfo::cProfileID_Max> {
    public:
        VanillaProfileExecutePriorities() {
            for (s32 i = 0; i < ProfileInfo::cProfileID_Max; i++)
                (*this)[i] = static_cast<s16>(i);
        }
    };

private:
    static sead::FixedStrTreeMap<cNameMaxLen, Profile*, cMaxCustomProfiles> sCustomProfiles;
    static sead::FixedStrTreeMap<cNameMaxLen, ResourceData, cMaxCustomProfiles> sCustomProfileResources;
    static sead::FixedStrTreeMap<cNameMaxLen, s16, cMaxCustomProfiles> sCustomProfileDrawPriorities;
    static sead::FixedStrTreeMap<cNameMaxLen, s16, cMaxCustomProfiles> sCustomProfileExecutePriorities;
    static VanillaProfileExecutePriorities sVanillaProfileExecutePriorities;
    static const char* sProfileNames[];
};

} // namespace red
