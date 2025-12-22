#include <red/profile/ProfileEx.h>

#include <telkin/Telkin.h>

// Custom impl of profile ctor, mods use this instead of defining the symbol for original.
Profile::Profile(ActorFactory factory, s32 id, const sead::SafeString& name, const ActorCreateInfo* createInfo, Flag flag)
    : mFactory(factory)
    , mID(id)
    , mActorCreateInfo(createInfo != nullptr ? createInfo : &ActorCreateInfo::cDefault)
    , mIsResLoaded(false)
    , mFlag(flag)
{
    if (id != -1 && id < ProfileInfo::cProfileID_Max) // vanilla replacements
        sProfileList[id] = this;
    else
        red::ProfileEx::addIdentifierProfile(name, this);
}

sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, Profile*, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfiles;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, red::ProfileEx::ResourceData, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileResources;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, s16, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileDrawPriorities;

// Increase all instances of the 0x2D4 limit to 0xFFFF
// TODO: Only keep the ones that are needed, else it may crash trying to use out of bounds sprite id to perform array lookup
tPatch16(0x200501A, 0xFFFF);
tPatch16(0x2007C62, 0xFFFF);
tPatch16(0x200806E, 0xFFFF);
tPatch16(0x2008262, 0xFFFF);
tPatch16(0x200844E, 0xFFFF);
tPatch16(0x200457A, 0xFFFF);
