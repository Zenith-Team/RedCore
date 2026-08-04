#include <red/heap/AudioSoundHeap.h>
#include <audio/cafe/seadAudioSoundHeapCafe.h>
#include <audio/GameAudio.h>
#include <sound/SndAudioMgr.h>
#include <sound/AudAudioPlayer.h>
#include <sound/SndSceneMgr.h>
#include <sound/SndItemID.h>
#include <telkin/Telkin.h>

using namespace tk::ppc;

// disable autoloading all bgm on boot
tPatch32u(0x029C60E8, li(GPR::r3, 1)); // BackgroundLoadMgr::runCallback

// disable CourseCacheMgr
tPatchNop(0x0223B680); // CourseSelectPlayer::setDestinationLevel

namespace red {

    int AudioSoundHeap::sState = 0;

    void AudioSoundHeap::saveState() {
        AudAudioPlayer* audioPlayer = SndAudioMgr::instance()->getAudioPlayer();
        sead::AudioSoundHeapCafe* soundHeap = audioPlayer->getSoundHeap();

        sState = soundHeap->SaveState();

        //tk::print("Saving sound heap state: %d\n", sState);

        GameAudio::instance()->initSound(); // replaced call
    }

    void AudioSoundHeap::loadState() {
        AudAudioPlayer* audioPlayer = SndAudioMgr::instance()->getAudioPlayer();
        sead::AudioSoundHeapCafe* soundHeap = audioPlayer->getSoundHeap();

        //tk::print("Loading sound heap state: %d\n", sState);

        soundHeap->LoadState(sState);
    }
}
tBranch(0x0202ADD4, red::AudioSoundHeap::saveState, tk::BranchType::b); // GameAudio::initSound (the static one)
tBranch(0x024BFD54, red::AudioSoundHeap::loadState, tk::BranchType::b); // CourseTask::exit

// shrink the sound heap size from 239MiB to 140MiB
tPatch32u(0x020293EC, lis(GPR::r3, 0x08C0)); // GameAudio::createSoundExpHeap
tPatchNop(0x020293F8); // GameAudio::createSoundExpHeap

namespace red {
    void preloadMenuBgm(SndSceneMgr* self) {
        self->loadBaseSndGroup();

        self->loadData(GROUP_TITLE);      //? Preload so it doesn't get unloaded when TitleScene exits
        self->loadData(GROUP_MENU);       //? ^
        self->loadData(GROUP_BGM_COURSE); //? ^ (Needed for challenge mode)
    }
}
tBranch(0x029C0F24, red::preloadMenuBgm, tk::BranchType::bl); // SndSceneMgr::loadSceneSound
