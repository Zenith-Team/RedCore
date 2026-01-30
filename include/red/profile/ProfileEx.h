#pragma once

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
    static Profile* get(const s32 id);
    
    [[nodiscard]]
    static const char* getName(const s32 id);
    
    [[nodiscard]]
    static s16 getDrawPriority(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static s16 getDrawPriority(const s32 id);
    
    [[nodiscard]]
    static ProfileInfo::ResType getResType(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static ProfileInfo::ResType getResType(const s32 id);
    
    [[nodiscard]]
    static u32 getResNum(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static u32 getResNum(const s32 id);
    
    [[nodiscard]]
    static const sead::SafeString* getResList(const sead::SafeString& identifier);
    
    [[nodiscard]]
    static const sead::SafeString* getResList(const s32 id);

public: //! The below are RedCore-internal APIs, do not use!
    static void addIdentifierProfile(const sead::SafeString& identifier, Profile* profile);
    static void setResources(const sead::SafeString& identifier, const ProfileInfo::ResType resourceType, sead::SafeString* resources, const u8 resourceCount);
    static void setName(const s32 id, const char* name);
    static void setDrawPriority(const sead::SafeString& identifier, const s16 priority);
    
    struct ResourceData {
        u8 resource_count = 0;
        const sead::SafeString* resources = nullptr;
        ProfileInfo::ResType resource_type = ProfileInfo::cResType_Course;
    };
    
    static s32 sProfileCount;

private:
    static sead::FixedStrTreeMap<cNameMaxLen, Profile*, cMaxCustomProfiles> sCustomProfiles;
    static sead::FixedStrTreeMap<cNameMaxLen, ResourceData, cMaxCustomProfiles> sCustomProfileResources;
    static sead::FixedStrTreeMap<cNameMaxLen, s16, cMaxCustomProfiles> sCustomProfileDrawPriorities;
    static const char* sProfileNames[];
};

} // namespace red


