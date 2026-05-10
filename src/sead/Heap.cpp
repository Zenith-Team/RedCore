#include <heap/seadExpHeap.h>
#include <heap/seadFrameHeap.h>
#include <heap/seadSeparateHeap.h>
#include <heap/seadUnitHeap.h>
#include <telkin/Telkin.h>

const void* sead::UnitHeap::getStartAddress() const { return mStart; }
tPointer(0x101914A4, sead::UnitHeap::getStartAddress, false);

const void* sead::SeparateHeap::getStartAddress() const { return mStart; }
tPointer(0x101A4658, sead::SeparateHeap::getStartAddress, false);

size_t sead::ExpHeap::getSize() const { return mSize; }
tPointer(0x10191094, sead::ExpHeap::getSize, false);

size_t sead::FrameHeap::getSize() const { return mSize; }
tPointer(0x101911C4, sead::FrameHeap::getSize, false);

size_t sead::UnitHeap::getSize() const { return mSize; }
tPointer(0x101914B4, sead::UnitHeap::getSize, false);

// The problem with the RTTI functions below is that they are defined inline in the header, so the body never gets instantiated
// and we cannot force a reference to them because they are virtual so the resulting PTMF will only contain the vtable index.
// So we have to manually define them

SEAD_RTTI_BASE_IMPL(sead::Heap)

namespace red {
    bool sead_Heap_checkDerivedRuntimeTypeInfo(const sead::Heap* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::Heap::getRuntimeTypeInfoStatic();
        return type == clsTypeInfo;
    }
}
tPointer(0x101912C0, red::sead_Heap_checkDerivedRuntimeTypeInfo, false);

namespace red {
    const sead::RuntimeTypeInfo::Interface* sead_Heap_getRuntimeTypeInfo(const sead::Heap* self) {
        return sead::Heap::getRuntimeTypeInfoStatic();
    }
}
tPointer(0x101912C8, red::sead_Heap_getRuntimeTypeInfo, false);

SEAD_RTTI_OVERRIDE_IMPL(sead::UnitHeap, sead::Heap)

namespace red {
    bool sead_UnitHeap_checkDerivedRuntimeTypeInfo(const sead::UnitHeap* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::UnitHeap::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;
        
        return sead_Heap_checkDerivedRuntimeTypeInfo(self, type);
    }
}

tPointer(0x1019145C, red::sead_UnitHeap_checkDerivedRuntimeTypeInfo, false);

namespace red {
    const sead::RuntimeTypeInfo::Interface* sead_UnitHeap_getRuntimeTypeInfo(const sead::UnitHeap* self) {
        return sead::UnitHeap::getRuntimeTypeInfoStatic();
    }
}
tPointer(0x10191464, red::sead_UnitHeap_getRuntimeTypeInfo, false);

SEAD_RTTI_OVERRIDE_IMPL(sead::FrameHeap, sead::Heap)

namespace red {
    bool sead_FrameHeap_checkDerivedRuntimeTypeInfo(const sead::FrameHeap* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::FrameHeap::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;
        
        return sead_Heap_checkDerivedRuntimeTypeInfo(self, type);
    }
}
tPointer(0x1019116C, red::sead_FrameHeap_checkDerivedRuntimeTypeInfo, false);

namespace red {
    const sead::RuntimeTypeInfo::Interface* sead_FrameHeap_getRuntimeTypeInfo(const sead::FrameHeap* self) {
        return sead::FrameHeap::getRuntimeTypeInfoStatic();
    }
}
tPointer(0x10191174, red::sead_FrameHeap_getRuntimeTypeInfo, false);

SEAD_RTTI_OVERRIDE_IMPL(sead::SeparateHeap, sead::Heap)

namespace red {
    bool sead_SeparateHeap_checkDerivedRuntimeTypeInfo(const sead::SeparateHeap* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::SeparateHeap::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;
        
        return sead_Heap_checkDerivedRuntimeTypeInfo(self, type);
    }
}
tPointer(0x101A4610, red::sead_SeparateHeap_checkDerivedRuntimeTypeInfo, false);

namespace red {
    const sead::RuntimeTypeInfo::Interface* sead_SeparateHeap_getRuntimeTypeInfo(const sead::SeparateHeap* self) {
        return sead::SeparateHeap::getRuntimeTypeInfoStatic();
    }
}
tPointer(0x101a4618, red::sead_SeparateHeap_getRuntimeTypeInfo, false);

SEAD_RTTI_OVERRIDE_IMPL(sead::ExpHeap, sead::Heap)

namespace red {
    bool sead_ExpHeap_checkDerivedRuntimeTypeInfo(const sead::ExpHeap* self, const sead::RuntimeTypeInfo::Interface* type) {
        const sead::RuntimeTypeInfo::Interface* clsTypeInfo = sead::ExpHeap::getRuntimeTypeInfoStatic();
        if (type == clsTypeInfo)
            return true;
        
        return sead_Heap_checkDerivedRuntimeTypeInfo(self, type);
    }
}
tPointer(0x1019103C, red::sead_ExpHeap_checkDerivedRuntimeTypeInfo, false);

namespace red {
    const sead::RuntimeTypeInfo::Interface* sead_ExpHeap_getRuntimeTypeInfo(const sead::ExpHeap* self) {
        return sead::ExpHeap::getRuntimeTypeInfoStatic();
    }
}
tPointer(0x10191044, red::sead_ExpHeap_getRuntimeTypeInfo, false);
