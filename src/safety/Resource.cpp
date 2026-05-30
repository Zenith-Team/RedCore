#include <system/ResMgr.h>
#include <red/public/ResMgr.h>

#define TELKIN_REGISTERS
#include <telkin/Assembly.h>
#include <telkin/Hooks.h>
#include <telkin/Print.h>

namespace red {
    void caughtNullResourceInModel() {
        tk::fatal("Attempted to get an unloaded resource!\nForgot to register a resource for your profile?");
    }
    
    void checkNullResource() tAssembly (
        cmpwi r21, 0x0;
        beq _ZN3red25caughtNullResourceInModelEv;
        
        lwz r20, 0x10(r21); // replaced instruction
        blr;
    )
}
tBranch(0x024EE384, red::checkNullResource, tk::BranchType::bl); // Model::createG3dImpl

namespace red {
    void* resMgrGetFileFromArchiveResSafe(const ResMgr* self, const sead::SafeString& key, const sead::SafeString& filename, u32* length);
}

void* red::resMgrGetFileFromArchiveResSafe(const ResMgr* self, const sead::SafeString& key, const sead::SafeString& filename, u32* length) {
    sead::ArchiveRes* archive = self->getArchiveRes(key);
    if (archive == nullptr) [[unlikely]] {
        tk::fatal("Failed to get file %s from archive %s\n    because the archive was not loaded.", key.cstr());
    }
    
    return red::pub::ResMgr::getFileFromArchiveResImpl_(archive, filename, length);
}
tBranch(0x029D3CF4, red::resMgrGetFileFromArchiveResSafe, tk::BranchType::b); // ResMgr::getFileFromArchiveRes
