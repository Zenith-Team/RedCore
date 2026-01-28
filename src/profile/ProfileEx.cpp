#define PROFILE_INFO_AS_NAMESPACE
#include <red/profile/ProfileEx.h>

#include <telkin/Telkin.h>

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
    
    if (id != -1 && id < ProfileInfo::cProfileID_Max) // vanilla replacements
        sProfileList[id] = this;
    else
        red::ProfileEx::addIdentifierProfile(name, this);
}

Profile* Profile::get(s32 id) {
    return sProfileList[id];
}

sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, Profile*, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfiles;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, red::ProfileEx::ResourceData, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileResources;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, s16, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileDrawPriorities;

const char* red::ProfileEx::sProfileNames[ProfileInfo::cProfileID_Max + cMaxCustomProfiles] = { nullptr };
s32 red::ProfileEx::sProfileCount = 0;

// Increase all instances of the 0x2D4 limit to 0xFFFF
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
    
    if (p <= 0) {
        OSReport("ID %i PRIO %i\n", id, int(p));
    }
    
    return p;
}

// Profile::getDrawPriority
tHook(0x2019988, "DrawPriorityHook", tk::BranchType::b);


