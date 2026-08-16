#include <red/audio/AudioObject.h>

#include <audio/cafe/seadAudioSoundDataMgrCafe.h>

#include <sound/AudAudioPlayer.h>
#include <sound/SndAudioMgr.h>
#include <sound/SndSpeakerMgr.h>
#include <system/MainGame.h>

namespace red {

SoundObject::SoundObject(AudAudioPlayer* player, nw::snd::OutputLine outputLine)
    : NMSndObjectBase(ObjType(0), player)
    , mAudioPlayer(player)
{
    SetPlayableSoundCount(0, cPlayableSoundNum);

    mOutputLine = outputLine;
    mHandleNum = cHandleNum;
}

void SoundObject::calc(const sead::Vector2f& pos)
{
    mPos = pos;

    // TODO
}

void SoundObject::sendRemote(nw::snd::SoundHandle* handle, const char* label, nw::snd::OutputLine outputLine)
{
    if (MainGame::instance()->isSinglePlayerDRC()) //? Game does another check here... should we care ?
    {
        return;
    }

    if (outputLine == 0)
    {
        outputLine = mOutputLine;
    }

    nw::snd::SoundArchive* arc = mAudioPlayer->getSoundDataMgr()->getSoundArchive();
    u32 id = arc->GetItemId(label);
    u32 flags = arc->GetSoundUserParam(id);
    SndSpeakerMgr::instance()->setRemoteSend(handle, outputLine, flags);
}

void SoundObject::setSoundPosition(nw::snd::SoundHandle* handle, const sead::Vector2f& pos)
{
    if (!handle || !handle->IsAttachedSound())
    {
        return;
    }

    // TODO: Game does some flags manip here
    u32 flags = 0xFFFFFFFF;
    sead::Vector2f screenSize(1.0f, 1.0f);

    Snd2DCalc::Param param;

    Snd2DCalc snd2DCalc;
    snd2DCalc.calcParam(&param, pos, flags, &screenSize);

    handle->SetVolume(param.volume);
    handle->SetPitch(param.pitch);
    handle->SetPan(param.pan);

    // TODO: Priority
}

nw::snd::SoundHandle* SoundObject::startSound_(const char* label, nw::snd::OutputLine outputLine)
{
    nw::snd::SoundHandle* handle = getFreeHandle_();
    if (!handle)
    {
        return nullptr;
    }

    StartSound(handle, label);

    if (!handle->IsAttachedSound())
    {
        return nullptr;
    }

    SndAudioMgr::instance()->setSoundOutputLine(handle);
    sendRemote(handle, label, outputLine);

    return handle;
}

nw::snd::SoundHandle* SoundObject::holdSound_(const char* label, nw::snd::OutputLine outputLine)
{
    nw::snd::SoundHandle* handle = findHandle_(label);
    if (!handle)
    {
        handle = getFreeHandle_();
    }

    if (!handle)
    {
        return nullptr;
    }

    HoldSound(handle, label);

    if (!handle->IsAttachedSound())
    {
        return nullptr;
    }

    SndAudioMgr::instance()->setSoundOutputLine(handle);
    sendRemote(handle, label, outputLine);

    return handle;
}

nw::snd::SoundHandle* SoundObject::startSound_(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine)
{
    nw::snd::SoundHandle* handle = startSound_(label, outputLine);
    if (handle)
    {
        setSoundPosition(handle, pos);
    }

    return handle;
}

nw::snd::SoundHandle* SoundObject::holdSound_(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine)
{
    nw::snd::SoundHandle* handle = holdSound_(label, outputLine);
    if (handle)
    {
        setSoundPosition(handle, pos);
    }

    return handle;
}

nw::snd::SoundHandle* SoundObject::getFreeHandle_()
{
    for (u32 i = 0; i < mHandleNum; i++)
    {
        if (!mHandles[i].IsAttachedSound())
        {
            return &mHandles[i];
        }
    }

    return nullptr;
}

nw::snd::SoundHandle* SoundObject::findHandle_(const char* label)
{
    u32 id = mAudioPlayer->getSoundDataMgr()->getSoundArchive()->GetItemId(label);

    for (u32 i = 0; i < mHandleNum; i++)
    {
        if (mHandles[i].IsAttachedSound() && mHandles[i].GetId() == id)
        {
            return &mHandles[i];
        }
    }

    return nullptr;
}

}
