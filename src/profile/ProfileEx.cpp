#define PROFILE_INFO_AS_NAMESPACE
#include <red/profile/ProfileEx.h>

#include <telkin/Telkin.h>

#include <red/util/Log.h>

static s32 getNext(s32 id) {
    red::ProfileEx::sProfileCount++;
    
    if (id == -1) {
        return red::ProfileEx::sProfileCount;
    }
    
    return id;
}

// Custom impl of profile ctor, mods use this instead of defining the symbol for original.
Profile::Profile(ActorFactory factory, s32 id, const sead::SafeString& name, const ActorCreateInfo* createInfo, Flag flag)
    : mFactory(factory)
    , mID(getNext(id))
    , mActorCreateInfo(createInfo != nullptr ? createInfo : &ActorCreateInfo::cDefault)
    , mIsResLoaded(false)
    , mFlag(flag)
{
    red::ProfileEx::setName(mID, name.cstr());
    
    if (id != -1 && id < ProfileInfo::cProfileID_Max) { // vanilla replacements
        sProfileList[id] = this;
    } else {
        red::ProfileEx::addIdentifierProfile(name, this);
    }
}

Profile* Profile::get(s32 id) {
    return sProfileList[id];
}

sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, Profile*, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfiles;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, red::ProfileEx::ResourceData, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileResources;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, s16, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileDrawPriorities;

const char* red::ProfileEx::sProfileNames[ProfileInfo::cProfileID_Max + cMaxCustomProfiles] = { nullptr };
s32 red::ProfileEx::sProfileCount = 0;

Profile* red::ProfileEx::get(const sead::SafeString& identifier)  {
    Profile** it = sCustomProfiles.find(identifier);
    if (it == nullptr) [[unlikely]] {
        OSReport("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return nullptr;
    }
    return *it;
}

Profile* red::ProfileEx::get(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n", id);
        return nullptr;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return Profile::get(id); // Not infinite loop because we define a custom impl above, not link to original
    }
    
    sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get draw priority\n");
        return nullptr;
    }
    
    return ProfileEx::get(identifier);
}

const char* red::ProfileEx::getName(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n", id);
        return "";
    }
    
    const char* name = sProfileNames[id];
    
    red::print("Mapped ID [%i:%u]\n", id, name);
    
    return name;
}

s16 red::ProfileEx::getDrawPriority(const sead::SafeString& identifier) {
    s16* it = sCustomProfileDrawPriorities.find(identifier);
    if (it == nullptr) [[unlikely]] {
        OSReport("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return 0;
    }
    return *it;
}

s16 red::ProfileEx::getDrawPriority(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n", id);
        return 0;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return ProfileInfo::cDrawPriority[id];
    }
    
    sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get draw priority\n");
        return 0;
    }
    
    s16* it = sCustomProfileDrawPriorities.find(identifier);
    if (it == nullptr) [[unlikely]] {
        red::print("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return 0;
    }
    if (*it < 0) {
        red::print("PROFILE [%i:%s] PRIO: %i", id, identifier.cstr(), int(*it));
    }
    return *it;
}

ProfileInfo::ResType red::ProfileEx::getResType(const sead::SafeString& identifier) {
    ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        OSReport("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return ProfileInfo::cResType_Num;
    }
    return it->resource_type;
}

ProfileInfo::ResType red::ProfileEx::getResType(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: ProfileID %i (0x%x) was not found\n", id, id);
        return ProfileInfo::cResType_Boot;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return static_cast<const ProfileInfo::ResType>(ProfileInfo::cResType[id]);
    }
    
    sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get res type\n");
        return ProfileInfo::cResType_Boot;
    }
    
    return ProfileEx::getResType(identifier);
}

u32 red::ProfileEx::getResNum(const sead::SafeString& identifier) {
    ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        OSReport("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return 0;
    }
    return it->resource_count;
}

u32 red::ProfileEx::getResNum(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n", id);
        return 0;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return ProfileInfo::cResNum[id];
    }
    
    sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get resource count\n");
        return 0;
    }
    
    return ProfileEx::getResNum(identifier);
}

const sead::SafeString* red::ProfileEx::getResList(const sead::SafeString& identifier) {
    ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        OSReport("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return nullptr;
    }
    return it->resources;
}

const sead::SafeString* red::ProfileEx::getResList(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n");
        return nullptr;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return ProfileInfo::cResList[id];
    }
    
    sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get resource list\n");
        return nullptr;
    }
    
    return ProfileEx::getResList(identifier);
}

void red::ProfileEx::addIdentifierProfile(const sead::SafeString& identifier, Profile* profile) {
    sCustomProfiles.insert(identifier, profile);
}

void red::ProfileEx::setResources(const sead::SafeString& identifier, const ProfileInfo::ResType resourceType, sead::SafeString* resources, const u8 resourceCount) {
    ResourceData data = {
        .resource_count = resourceCount,
        .resources = resources,
        .resource_type = resourceType
    };
    sCustomProfileResources.insert(identifier, data);
}

void red::ProfileEx::setName(const s32 id, const char* name) {
    sProfileNames[id] = name;
}

void red::ProfileEx::setDrawPriority(const sead::SafeString& identifier, const s16 priority) {
    sCustomProfileDrawPriorities.insert(identifier, priority);
}

// Increase all instances of the 0x2D4 spriteToProfileTable limit to 0xFFFF
// TODO: Only keep the ones that are needed, else it may crash trying to use out of bounds sprite id to perform array lookup
tPatch16(0x200501A, 0xFFFF);
tPatch16(0x2007C62, 0xFFFF);
tPatch16(0x200806E, 0xFFFF);
tPatch16(0x2008262, 0xFFFF);
tPatch16(0x200844E, 0xFFFF);
tPatch16(0x200457A, 0xFFFF);

// Patch info

extern "C" Profile* GetProfileHook(s32 id) {
    return red::ProfileEx::get(id);
}

// Profile::get
tHook(0x2019774, "GetProfileHook", tk::BranchType::b);

extern "C" s16 DrawPriorityHook(s32 id) {
    s16 p = red::ProfileEx::getDrawPriority(id);
    
    red::print("ID %i PRIO %i\n", id, int(p));
    
    return p;
}

// ProfileInfo::getDrawPriority
tHook(0x2019988, "DrawPriorityHook", tk::BranchType::b);

extern "C" ProfileInfo::ResType GetResTypeHook(const s32 id) {
    return red::ProfileEx::getResType(id);
}

// ProfileInfo::getResType
tHook(0x20199A8, "GetResTypeHook", tk::BranchType::b);

extern "C" u32 GetResNumHook(const s32 id) {
    return red::ProfileEx::getResNum(id);
}

// ProfileInfo::getResNum
tHook(0x20199CC, "GetResNumHook", tk::BranchType::b);

extern "C" const sead::SafeString* GetResListHook(const s32 id) {
    return red::ProfileEx::getResList(id);
}

// ProfileInfo::getResList
tHook(0x20199EC, "GetResListHook", tk::BranchType::b);
