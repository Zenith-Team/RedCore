#include <red/mod/Mod.h>
#include <red/heap/RedCoreHeap.h>
#include <red/event/TaskPrepareEvent.h>

#include <audio/cafe/seadAudioSoundDataMgrCafe.h>
#include <audio/seadAudioMgr.h>
#include <filedevice/seadFileDeviceMgr.h>

#include <player/PlayerMgr.h>
#include <player/PlayerObject.h>
#include <sound/SndAudioMgr.h>

#include <telkin/Hooks.h>

namespace red {

sead::FixedStrTreeMap<Mod::cModNameMax, Mod*, 32> sMods; // TODO: Dynamic

Mod::FileSystem::FileSystem(const char* nameSpace)
    : mFileDevice(nameSpace, sead::FileDeviceMgr::instance()->getMainFileDevice(), sead::FormatFixedSafeString<256>("%s/", nameSpace))
{
    sead::FileDeviceMgr::instance()->mount(&mFileDevice);
}

Mod::FileSystem::~FileSystem()
{
    sead::FileDeviceMgr::instance()->unmount(&mFileDevice);
}

Mod::AudioSystem::AudioSystem(const char* nameSpace)
    : mAudioPlayer()
    , mAudioObject(&mAudioPlayer)
{
    mAudioPlayer.initialize();

    mAudioPlayer.getSoundDataMgr()->setContentRootPath("/vol/content");
    mAudioPlayer.getSoundDataMgr()->mountSoundArchiveFromFs(
        sead::FormatFixedSafeString<256>("%s/sound/%s.bfsar", nameSpace, nameSpace),
        nullptr, false, true
    );

    mAudioPlayer.setupDataManagement(0, 0, 0, nullptr);
    mAudioPlayer.createSoundHeap(128, nullptr);
}

Mod::AudioSystem::~AudioSystem()
{
    mAudioPlayer.finalize();
}

bool Mod::AudioSystem::loadData(const char* itemName, u32 loadFlag, sead::AudioSoundHeapCafe* soundHeap)
{
    if (!soundHeap)
    {
        soundHeap = SndAudioMgr::instance()->getAudioPlayer()->getSoundHeap();
    }

    return mAudioPlayer.getSoundDataMgr()->loadData(
        mAudioPlayer.getSoundDataMgr()->getSoundArchive()->GetItemId(itemName),
        loadFlag, 0, soundHeap
    );
}

void Mod::AudioSystem::update()
{
    mAudioPlayer.calc();
}

Mod::Mod(const char* nameSpace)
    : mNamespace(nameSpace)
    , mFileSystem(nullptr)
    , mAudioSystem(nullptr)
{
    sMods.insert(nameSpace, this);
}

Mod::~Mod()
{
    if (mAudioSystem)
    {
        delete mAudioSystem;
    }

    if (mFileSystem)
    {
        delete mFileSystem;
    }
}

Mod* Mod::getMod(const char* nameSpace)
{
    Mod** mod = sMods.find(nameSpace);
    if (mod)
    {
        return *mod;
    }

    return nullptr;
}

Mod* Mod::findModFromID(const char* id, sead::BufferedSafeString* noModID)
{
    noModID->trim(0);

    sead::SafeString idStr(id);

    s32 index = idStr.findIndex(":");
    if (index == -1)
    {
        return nullptr;
    }

    sead::FixedSafeString<cModNameMax> modName;
    modName.copy(id, index);

    Mod* mod = Mod::getMod(modName.cstr());
    if (!mod)
    {
        return nullptr;
    }

    noModID->copy(idStr.getPart(index + 1));
    return mod;
}

bool Mod::initFileSystem(sead::Heap* heap)
{
    if (!sead::FileDeviceMgr::instance()->getMainFileDevice()->isExistDirectory(mNamespace))
    {
        return false;
    }

    mFileSystem = new(heap) FileSystem(mNamespace);
    return true;
}

bool Mod::initAudioSystem(sead::Heap* heap)
{
    if (!mFileSystem)
    {
        return false;
    }

    if (!mFileSystem->getFileDevice()->isExistFile(sead::FormatFixedSafeString<256>("sound/%s.bfsar", mNamespace)))
    {
        return false;
    }

    mAudioSystem = new(heap) AudioSystem(mNamespace);
    return true;
}

TaskPrepareEvent::Listener<TaskPrepareEvent::Stage::Before> StartMods([](TaskPrepareEvent& e) {
    if (e.getTask()->getName() != "RootTask")
        return;

    sMods.forEach([](const sead::SafeString& key, Mod* mod) {
        if (mod->initFileSystem(RedCoreHeap::instance()))
        {
            tk::println("Initialized FileSystem for mod: %s", mod->getNamespace());
        }
    });
}, 9999);

TaskPrepareEvent::Listener<TaskPrepareEvent::Stage::After> StartModsAudio([](TaskPrepareEvent& e) {
    if (e.getTask()->getName() != "RootTask2")
        return;

    sead::ExpHeap* audioHeap = sead::ExpHeap::create(0, "AudioHeap", RedCoreHeap::instance());
    sead::CurrentHeapSetter chs(audioHeap);

    sMods.forEach([](const sead::SafeString& key, Mod* mod) {
        if (mod->initAudioSystem(nullptr))
        {
            tk::println("Initialized AudioSystem for mod: %s", mod->getNamespace());

            mod->getAudioSystem()->loadData("GROUP_BOOT");
        }
    });

    audioHeap->adjust();
}, 10000);

TaskPrepareEvent::Listener<TaskPrepareEvent::Stage::After> StartModsCourseAudio([](TaskPrepareEvent& e) {
    if (e.getTask()->getName() != "CourseTask")
        return;

    sMods.forEach([](const sead::SafeString& key, Mod* mod) {
        if (mod->getAudioSystem())
        {
            mod->getAudioSystem()->loadData("GROUP_COURSE");
        }
    });
}, 10000);

void UpdateModsAudio() {
    sead::AudioMgr::instance()->calc();

    sMods.forEach([](const sead::SafeString& key, Mod* mod) {
        if (mod->getAudioSystem())
        {
            mod->getAudioSystem()->update();
        }
    });
}

} // namespace red

tBranch(0x029B502C, red::UpdateModsAudio, tk::BranchType::bl); // SndAudioMgr::calc
