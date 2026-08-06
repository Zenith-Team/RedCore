#pragma once

#include <filedevice/seadAliasFileDevice.h>
#include <filedevice/seadAnyFileDevice.h>
#include <filedevice/seadArchiveFileDevice.h>
#include <filedevice/seadFileDevice.h>
#include <filedevice/cafe/seadCafeFSAFileDeviceCafe.h>
#include <filedevice/cafe/seadCafeSaveFileDeviceCafe.h>

namespace red::pub {

    class FileDevice : public sead::FileDevice
    {
    public:
        using sead::FileDevice::doResolvePath_;
    };

    class AliasFileDevice : public sead::AliasFileDevice
    {
    public:
        using sead::AliasFileDevice::mFileDevice;
        using sead::AliasFileDevice::mPath;
    };

    class AnyFileDevice : public sead::AnyFileDevice
    {
    public:
        using sead::AnyFileDevice::findFileDeviceByDirectory_;

        using sead::AnyFileDevice::mLastFileDevice;
    };

    class ArchiveFileDevice : public sead::ArchiveFileDevice
    {
    public:
        using sead::ArchiveFileDevice::mArchive;
    };

    class MainFileDevice : public sead::MainFileDevice
    {
    public:
        using sead::MainFileDevice::mFileDevice;
    };

    class CafeFSAFileDevice : public sead::CafeFSAFileDevice
    {
    public:
        using sead::CafeFSAFileDevice::formatPathForFSA_;
        using sead::CafeFSAFileDevice::getUsableFSClient_;

        using sead::CafeFSAFileDevice::mLastRawError;
    };

    class CafeSaveFileDevice : public sead::CafeSaveFileDevice
    {
    public:
        using sead::CafeSaveFileDevice::getUsableFSClient_;

        using sead::CafeSaveFileDevice::mLastRawError;
        using sead::CafeSaveFileDevice::mAccountSlotNo;
    };

}
