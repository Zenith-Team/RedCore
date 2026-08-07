#pragma once

#include <audio/GameAudio.h>

class AudAudioPlayer;

namespace red {

class SoundObject : public NMSndObjectBase
{
public:
    static const u32 cPlayableSoundNum = 12;
    static const u32 cHandleNum = cPlayableSoundNum + cExtraHandleNum;

public:
    SoundObject(AudAudioPlayer* player, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN);

    // u8 vf34(const char* label, s32 actorPlayerId) override; // TODO

    nw::snd::SoundHandle* startSound(const char* label, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN)
    {
        return startSound_(label, outputLine);
    }

    nw::snd::SoundHandle* holdSound(const char* label, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN)
    {
        return holdSound_(label, outputLine);
    }

    virtual nw::snd::SoundHandle* startSound(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN)
    {
        return startSound_(label, pos, outputLine);
    }

    virtual nw::snd::SoundHandle* holdSound(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN)
    {
        return holdSound_(label, pos, outputLine);
    }

    virtual void calc(const sead::Vector2f& pos);

    void sendRemote(nw::snd::SoundHandle* handle, const char* label, nw::snd::OutputLine outputLine);
    void setSoundPosition(nw::snd::SoundHandle* handle, const sead::Vector2f& pos);

protected:
    nw::snd::SoundHandle* startSound_(const char* label, nw::snd::OutputLine outputLine);
    nw::snd::SoundHandle* holdSound_(const char* label, nw::snd::OutputLine outputLine);
    nw::snd::SoundHandle* startSound_(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine);
    nw::snd::SoundHandle* holdSound_(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine);

    nw::snd::SoundHandle* getFreeHandle_();
    nw::snd::SoundHandle* findHandle_(const char* label);

protected:
    AudAudioPlayer* mAudioPlayer;
    nw::snd::SoundHandle mHandles[cHandleNum];
    sead::Vector2f mPos;
};

class AudioObject : public SoundObject
{
public:
    AudioObject(AudAudioPlayer* player, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN)
        : SoundObject(player, outputLine)
    {
    }

    using SoundObject::startSound;
    using SoundObject::holdSound;

    nw::snd::SoundHandle* startSound(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN) override
    {
        sead::Vector2f screenPos;
        GameAudio::convertAudioObjctPos(&screenPos, pos);
        return SoundObject::startSound_(label, screenPos, outputLine);
    }

    nw::snd::SoundHandle* holdSound(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN) override
    {
        sead::Vector2f screenPos;
        GameAudio::convertAudioObjctPos(&screenPos, pos);
        return SoundObject::holdSound_(label, screenPos, outputLine);
    }

    void calc(const sead::Vector2f& pos) override
    {
        sead::Vector2f screenPos;
        GameAudio::convertAudioObjctPos(&screenPos, pos);
        SoundObject::calc(screenPos);
    }
};

// TODO
// class AudioObjectCS : public SoundObject
// {
// public:
//     AudioObjectCS(AudAudioPlayer* player, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN)
//         : SoundObject(player, outputLine)
//     {
//     }

//     nw::snd::SoundHandle* startSound(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN) override;
//     nw::snd::SoundHandle* holdSound(const char* label, const sead::Vector2f& pos, nw::snd::OutputLine outputLine = nw::snd::OutputLine::OUTPUT_LINE_MAIN) override;
//     void calc(const sead::Vector2f& pos) override;
// };

}
