#pragma once

#include <container/seadStrTreeMap.h>
#include <actor/Profile.h>

namespace red {

class ProfileEx {
public:
    static constexpr s32 cNameMaxLen = 64;
    static constexpr s32 cMaxCustomProfiles = 4096;

public:
    [[nodiscard]]
    static Profile* get(const sead::SafeString& identifier) {
        Profile** it = sCustomProfiles.find(identifier);
        // TODO: Check null
        return *it;
    }
    
    [[nodiscard]]
    static Profile* get(s32 id) {
        // TODO: Check range
        
        return Profile::get(id);
    }
    
    [[nodiscard]]
    static s16 getDrawPriority(const sead::SafeString& identifier) {
        s16* it = sCustomProfileDrawPriorities.find(identifier);
        // TODO: Check null
        return *it;
    }
    
    [[nodiscard]]
    static s16 getDrawPriority(s32 id) {
        // TODO: Check range
        return ProfileInfo::getDrawPriority(id);
    }
    
    [[nodiscard]]
    static ProfileInfo::ResType getResType(const sead::SafeString& identifier) {
        ResourceData* it = sCustomProfileResources.find(identifier);
        // TODO: Check null
        return it->resource_type;
    }
    
    [[nodiscard]]
    static ProfileInfo::ResType getResType(s32 id) {
        // TODO: Check range
        return ProfileInfo::getResType(id);
    }
    
    [[nodiscard]]
    static u32 getResNum(const sead::SafeString& identifier) {
        ResourceData* it = sCustomProfileResources.find(identifier);
        // TODO: Check null
        return it->resource_count;
    }
    
    [[nodiscard]]
    static u32 getResNum(s32 id) {
        // TODO: Check range
        return ProfileInfo::getResNum(id);
    }
    
    [[nodiscard]]
    static const sead::SafeString* getResList(const sead::SafeString& identifier) {
        ResourceData* it = sCustomProfileResources.find(identifier);
        // TODO: Check null
        return it->resources;
    }
    
    [[nodiscard]]
    static const sead::SafeString* getResList(s32 id) {
        // TODO: Check range
        return ProfileInfo::getResList(id);
    }

public: //! The below are RedCore-internal APIs, do not use!
    static void addIdentifierProfile(const sead::SafeString& identifier, Profile* profile) {
        sCustomProfiles.insert(identifier, profile);
    }
    
    static void setResources(const sead::SafeString& identifier, ProfileInfo::ResType resourceType, sead::SafeString* resources, u8 resourceCount) {
        ResourceData data = {
            .resource_count = resourceCount,
            .resources = resources,
            .resource_type = resourceType
        };
        sCustomProfileResources.insert(identifier, data);
    }
    
    static void setDrawPriority(const sead::SafeString& identifier, s16 priority) {
        sCustomProfileDrawPriorities.insert(identifier, priority);
    }
    
    struct ResourceData {
        u8 resource_count = 0;
        const sead::SafeString* resources = nullptr;
        ProfileInfo::ResType resource_type = ProfileInfo::cResType_Course;
    };
    
private:
    static sead::FixedStrTreeMap<cNameMaxLen, Profile*, cMaxCustomProfiles> sCustomProfiles;
    static sead::FixedStrTreeMap<cNameMaxLen, ResourceData, cMaxCustomProfiles> sCustomProfileResources;
    static sead::FixedStrTreeMap<cNameMaxLen, s16, cMaxCustomProfiles> sCustomProfileDrawPriorities;
};

} // namespace red


