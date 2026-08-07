#pragma once

#include <audio/cafe/seadAudioSoundHeapCafe.h>
#include <container/seadStrTreeMap.h>
#include <filedevice/seadAliasFileDevice.h>

#include <sound/AudAudioPlayer.h>

#include <red/audio/AudioObject.h>

namespace red {

class Mod
{
public:
    class FileSystem
    {
    public:
        FileSystem(const char* nameSpace);
        ~FileSystem();

        sead::FileDevice* getFileDevice()
        {
            return &mFileDevice;
        }

    private:
        sead::AliasFileDevice mFileDevice;
    };

    class AudioSystem
    {
    public:
        AudioSystem(const char* nameSpace);
        ~AudioSystem();

        AudAudioPlayer* getAudioPlayer()
        {
            return &mAudioPlayer;
        }

        AudioObject* getAudioObject()
        {
            return &mAudioObject;
        }

        bool loadData(const char* itemName, u32 loadFlag = 0xFFFFFFFF, sead::AudioSoundHeapCafe* soundHeap = nullptr);
        void update();

    private:
        AudAudioPlayer mAudioPlayer;
        AudioObject mAudioObject;
    };

    static const u32 cModNameMax = 32;

public:
    explicit Mod(const char* nameSpace);
    ~Mod();

    const char* getNamespace() const
    {
        return mNamespace;
    }

    FileSystem* getFileSystem()
    {
        return mFileSystem;
    }

    AudioSystem* getAudioSystem()
    {
        return mAudioSystem;
    }

    static Mod* getMod(const char* nameSpace);
    static Mod* findModFromID(const char* id, sead::BufferedSafeString* noModID);

    bool initFileSystem(sead::Heap* heap);
    bool initAudioSystem(sead::Heap* heap);

private:
    const char* mNamespace;
    FileSystem* mFileSystem;
    AudioSystem* mAudioSystem;
};

} // namespace red
