#include <filedevice/seadAliasFileDevice.h>
#include <filedevice/seadAnyFileDevice.h>
#include <filedevice/seadArchiveFileDevice.h>
#include <filedevice/seadMainFileDevice.h>
#include <filedevice/cafe/seadCafeSaveFileDeviceCafe.h>

#include <filedevice/seadFileDeviceMgr.h>

#include <red/public/seadFileDevice.h>

#include <telkin/Hooks.h>

bool sead::FileDevice::tryIsExistDirectory(bool* is_exist, const sead::SafeString& path)
{
    SEAD_ASSERT_MSG(mPermission, "Device permission error.");

    if (!mPermission)
        return false;

    if (is_exist == nullptr)
    {
        SEAD_ASSERT_MSG(false, "is_exist is null");
        return false;
    }

    return doIsExistDirectory_(is_exist, path);
}

sead::FileDevice* sead::AnyFileDevice::findFileDeviceByDirectory_(const sead::SafeString& path) const
{
    for (FileDeviceList::iterator it = mFileDeviceList.begin(); it != mFileDeviceList.end(); ++it)
    {
        FileDevice* device = &(*it);
        if (device->isAvailable() && device->isExistDirectory(path))
            return device;
    }

    return nullptr;
}

namespace red {

bool sead_AliasFileDevice_doIsExistDirectory_(red::pub::AliasFileDevice* self, bool* is_exist, const sead::SafeString& path)
{
    sead::FixedSafeString<512> full_path;
    full_path = self->mPath;
    full_path.append(path);
    return self->mFileDevice->tryIsExistDirectory(is_exist, full_path);
}

}

tPointerCode(0x1018ee14, red::sead_AliasFileDevice_doIsExistDirectory_); // sead::AliasFileDevice::vtable

namespace red {

bool sead_AnyFileDevice_doIsExistDirectory_(red::pub::AnyFileDevice* self, bool* is_exist, const sead::SafeString& path)
{
    sead::FileDevice* device = self->findFileDeviceByDirectory_(path);
    self->mLastFileDevice = device;
    *is_exist = device != nullptr;
    return true;
}

}

tPointerCode(0x1018efa8, red::sead_AnyFileDevice_doIsExistDirectory_); // sead::AnyFileDevice::vtable

namespace red {

bool sead_ArchiveFileDevice_doIsExistDirectory_(red::pub::ArchiveFileDevice* self, bool* is_exist, const sead::SafeString& path)
{
    if (self->mArchive == nullptr)
    {
        SEAD_ASSERT_MSG(false, "no archive mounted");
        return false;
    }

    if (path.cstr() == nullptr)
    {
        SEAD_ASSERT_MSG(false, "invalid path");
        return false;
    }

    *is_exist = false; // LOL
    return true;
}

}

tPointerCode(0x1018f0c8, red::sead_ArchiveFileDevice_doIsExistDirectory_); // sead::ArchiveFileDevice::vtable

namespace red {

bool sead_CafeFSAFileDevice_doIsExistDirectory_(red::pub::CafeFSAFileDevice* self, bool* is_exist, const sead::SafeString& path)
{
    FSCmdBlock block;
    FSInitCmdBlock(&block);

    FSClient* client = self->getUsableFSClient_();

    sead::FixedSafeString<sead::FileDeviceMgr::cNoDrivePathBufferSize> dir_path;
    self->formatPathForFSA_(&dir_path, path);

    FSStat stat;
    FSStatus status = FSGetStat(client, &block, dir_path.cstr(), &stat, FS_RET_PERMISSION_ERROR | FS_RET_NOT_FOUND);

    if (self->mLastRawError = status, status != FS_STATUS_OK)
    {
        if (status != FS_STATUS_NOT_FOUND)
            return false;

        *is_exist = false;
    }
    else
    {
        *is_exist = stat.flag & FS_STAT_FLAG_IS_DIRECTORY;
    }

    return true;
}

}

tPointerCode(0x1018f804, red::sead_CafeFSAFileDevice_doIsExistDirectory_); // sead::CafeFSAFileDevice::vtable
tPointerCode(0x1018f6cc, red::sead_CafeFSAFileDevice_doIsExistDirectory_); // sead::CafeContentFileDevice::vtable
tPointerCode(0x1018f8fc, red::sead_CafeFSAFileDevice_doIsExistDirectory_); // sead::CafeHostIOFileDevice::vtable
tPointerCode(0x1018f9f4, red::sead_CafeFSAFileDevice_doIsExistDirectory_); // sead::CafeFSNativePathFileDevice::vtable
tPointerCode(0x10185710, red::sead_CafeFSAFileDevice_doIsExistDirectory_); // CafeDebugSaveFileDevice::vtable

namespace red {

bool sead_CafeSaveFileDevice_doIsExistDirectory_(red::pub::CafeSaveFileDevice* self, bool* is_exist, const sead::SafeString& path)
{
    FSCmdBlock block;
    FSInitCmdBlock(&block);

    FSClient* client = self->getUsableFSClient_();

    FSStat stat;
    SAVEStatus status = SAVEGetStat(client, &block, self->mAccountSlotNo, path.cstr(), &stat, (
        FS_RET_NOT_FOUND |
        FS_RET_PERMISSION_ERROR
    ));

    self->mLastRawError = status;

    switch (status)
    {
    default:
        return false;
    case SAVE_STATUS_NOT_FOUND:
        *is_exist = false;
        return true;
    case SAVE_STATUS_OK:
        *is_exist = (stat.flag & (FS_STAT_FLAG_IS_DIRECTORY)) != 0;
        return true;
    }
}

}

tPointerCode(0x1018fb7c, red::sead_CafeSaveFileDevice_doIsExistDirectory_); // sead::CafeSaveFileDevice::vtable

namespace red {

bool sead_MainFileDevice_doIsExistDirectory_(red::pub::MainFileDevice* self, bool* is_exist, const sead::SafeString& path)
{
    return self->mFileDevice->tryIsExistDirectory(is_exist, path);
}

}

tPointerCode(0x1018f4ec, red::sead_MainFileDevice_doIsExistDirectory_); // sead::MainFileDevice::vtable

namespace red {

//? Because this is defined in the header
void sead_FileDevice_resolveFilePath(red::pub::FileDevice* self, sead::BufferedSafeString* out, const sead::SafeString& path)
{
    self->doResolvePath_(out, path);
}

}

tPointerCode(0x1018f24c, red::sead_FileDevice_resolveFilePath); // sead::FileDevice::vtable
tPointerCode(0x1018f058, red::sead_FileDevice_resolveFilePath); // sead::ArchiveFileDevice::vtable
tPointerCode(0x1018f794, red::sead_FileDevice_resolveFilePath); // sead::CafeFSAFileDevice::vtable
tPointerCode(0x1018f65c, red::sead_FileDevice_resolveFilePath); // sead::CafeContentFileDevice::vtable
tPointerCode(0x1018f88c, red::sead_FileDevice_resolveFilePath); // sead::CafeHostIOFileDevice::vtable
tPointerCode(0x1018f984, red::sead_FileDevice_resolveFilePath); // sead::CafeFSNativePathFileDevice::vtable
tPointerCode(0x1018fb0c, red::sead_FileDevice_resolveFilePath); // sead::CafeSaveFileDevice::vtable
tPointerCode(0x101856a0, red::sead_FileDevice_resolveFilePath); // CafeDebugSaveFileDevice::vtable

namespace red {

void sead_FileDevice_doResolvePath_(red::pub::FileDevice* self, sead::BufferedSafeString* out, const sead::SafeString& path)
{
    out->copy(path);
}

}

tPointerCode(0x1018f2f4, red::sead_FileDevice_doResolvePath_); // sead::FileDevice::vtable
tPointerCode(0x1018f524, red::sead_FileDevice_doResolvePath_); // sead::MainFileDevice::vtable
tPointerCode(0x1018ee4c, red::sead_FileDevice_doResolvePath_); // sead::AliasFileDevice::vtable
tPointerCode(0x1018efe0, red::sead_FileDevice_doResolvePath_); // sead::AnyFileDevice::vtable
tPointerCode(0x1018f100, red::sead_FileDevice_doResolvePath_); // sead::ArchiveFileDevice::vtable

void sead::MainFileDevice::resolveFilePath(sead::BufferedSafeString* out, const sead::SafeString& path) const
{
    mFileDevice->resolveFilePath(out, path);
}

tPointerCode(0x1018f47c, sead::MainFileDevice::resolveFilePath); // sead::MainFileDevice::vtable

void sead::AliasFileDevice::resolveFilePath(sead::BufferedSafeString* out, const sead::SafeString& path) const
{
    sead::FixedSafeString<512> full_path;
    full_path = mPath;
    full_path.append(path);
    mFileDevice->resolveFilePath(out, full_path);
}

tPointerCode(0x1018eda4, sead::AliasFileDevice::resolveFilePath); // sead::AliasFileDevice::vtable

void sead::AnyFileDevice::resolveFilePath(sead::BufferedSafeString* out, const sead::SafeString& path) const
{
    sead::FileDevice* device = findFileDeviceByFile_(path);
    if (!device)
    {
        //SEAD_WARNING(false, "FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->resolveFilePath(out, path);
}

tPointerCode(0x1018ef38, sead::AnyFileDevice::resolveFilePath); // sead::AnyFileDevice::vtable

namespace red {

//? Because this is defined in the header
void sead_CafeFSAFileDevice_doResolvePath_(red::pub::CafeFSAFileDevice* self, sead::BufferedSafeString* out, const sead::SafeString& path)
{
    self->formatPathForFSA_(out, path);
}

}

tPointerCode(0x1018f83c, red::sead_CafeFSAFileDevice_doResolvePath_); // sead::CafeFSAFileDevice::vtable
tPointerCode(0x1018f704, red::sead_CafeFSAFileDevice_doResolvePath_); // sead::CafeContentFileDevice::vtable
tPointerCode(0x1018f934, red::sead_CafeFSAFileDevice_doResolvePath_); // sead::CafeHostIOFileDevice::vtable
tPointerCode(0x1018fa2c, red::sead_CafeFSAFileDevice_doResolvePath_); // sead::CafeFSNativePathFileDevice::vtable
tPointerCode(0x10185748, red::sead_CafeFSAFileDevice_doResolvePath_); // CafeDebugSaveFileDevice::vtable

namespace red {

void sead_CafeSaveFileDevice_doResolvePath_(sead::CafeSaveFileDevice* self, sead::BufferedSafeString* out, const sead::SafeString& path)
{
    out->copy(path);
}

}

tPointerCode(0x1018fbb4, red::sead_CafeSaveFileDevice_doResolvePath_); // sead::CafeSaveFileDevice::vtable

// RTTI

SEAD_RTTI_BASE_IMPL(sead::FileDevice)

namespace red {
    bool sead_FileDevice_checkDerivedRuntimeTypeInfo(const sead::FileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::FileDevice::getRuntimeTypeInfoStatic();
        return type == clsTypeInfo;
    }
}
tPointerCode(0x1018f224, red::sead_FileDevice_checkDerivedRuntimeTypeInfo);

SEAD_RTTI_OVERRIDE_IMPL(sead::MainFileDevice, sead::FileDevice)

namespace red {
    bool sead_MainFileDevice_checkDerivedRuntimeTypeInfo(const sead::MainFileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::MainFileDevice::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;

        return sead_FileDevice_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointerCode(0x1018f454, red::sead_MainFileDevice_checkDerivedRuntimeTypeInfo);

SEAD_RTTI_OVERRIDE_IMPL(sead::AliasFileDevice, sead::FileDevice)

namespace red {
    bool sead_AliasFileDevice_checkDerivedRuntimeTypeInfo(const sead::AliasFileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::AliasFileDevice::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;

        return sead_FileDevice_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointerCode(0x1018ed7c, red::sead_AliasFileDevice_checkDerivedRuntimeTypeInfo);

SEAD_RTTI_OVERRIDE_IMPL(sead::AnyFileDevice, sead::FileDevice)

namespace red {
    bool sead_AnyFileDevice_checkDerivedRuntimeTypeInfo(const sead::AnyFileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::AnyFileDevice::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;

        return sead_FileDevice_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointerCode(0x1018ef10, red::sead_AnyFileDevice_checkDerivedRuntimeTypeInfo);

tPointerCode(0x1018f030, red::sead_FileDevice_checkDerivedRuntimeTypeInfo);

SEAD_RTTI_OVERRIDE_IMPL(sead::CafeFSAFileDevice, sead::FileDevice)

namespace red {
    bool sead_CafeFSAFileDevice_checkDerivedRuntimeTypeInfo(const sead::CafeFSAFileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::CafeFSAFileDevice::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;

        return sead_FileDevice_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointerCode(0x1018f76c, red::sead_CafeFSAFileDevice_checkDerivedRuntimeTypeInfo);

SEAD_RTTI_OVERRIDE_IMPL(sead::CafeContentFileDevice, sead::CafeFSAFileDevice)

namespace red {
    bool sead_CafeContentFileDevice_checkDerivedRuntimeTypeInfo(const sead::CafeContentFileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::CafeContentFileDevice::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;

        return sead_CafeFSAFileDevice_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointerCode(0x1018f634, red::sead_CafeContentFileDevice_checkDerivedRuntimeTypeInfo);

SEAD_RTTI_OVERRIDE_IMPL(sead::CafeHostIOFileDevice, sead::CafeFSAFileDevice)

namespace red {
    bool sead_CafeHostIOFileDevice_checkDerivedRuntimeTypeInfo(const sead::CafeHostIOFileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::CafeHostIOFileDevice::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;

        return sead_CafeFSAFileDevice_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointerCode(0x1018f864, red::sead_CafeHostIOFileDevice_checkDerivedRuntimeTypeInfo);

SEAD_RTTI_OVERRIDE_IMPL(sead::CafeFSNativePathFileDevice, sead::CafeFSAFileDevice)

namespace red {
    bool sead_CafeFSNativePathFileDevice_checkDerivedRuntimeTypeInfo(const sead::CafeFSNativePathFileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::CafeFSNativePathFileDevice::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;

        return sead_CafeFSAFileDevice_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointerCode(0x1018f95c, red::sead_CafeFSNativePathFileDevice_checkDerivedRuntimeTypeInfo);

SEAD_RTTI_OVERRIDE_IMPL(sead::CafeSaveFileDevice, sead::FileDevice)

namespace red {
    bool sead_CafeSaveFileDevice_checkDerivedRuntimeTypeInfo(const sead::CafeSaveFileDevice* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::CafeSaveFileDevice::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;

        return sead_FileDevice_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointerCode(0x1018fae4, red::sead_CafeSaveFileDevice_checkDerivedRuntimeTypeInfo);

tPointerCode(0x10185678, red::sead_FileDevice_checkDerivedRuntimeTypeInfo);
