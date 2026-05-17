#include <nw/ut/ut_FrameHeap.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadPtrUtil.h>

u32 nw::ut::FrameHeap::GetAllocatableSize(int alignment) {
    // NW_ASSERT(IsValid());

    // NW_ASSERT(alignment % MIN_ALIGNMENT == 0);
    // NW_ASSERT((Abs(alignment) & (Abs(alignment) - 1)) == 0);
    // NW_ASSERT(MIN_ALIGNMENT <= Abs(alignment));

    alignment = sead::Mathi::abs(alignment);

    {
        u32 retVal;
        const void* block = sead::PtrUtil::roundUpPow2(mHeadAllocator, (u32)alignment);

        if ((u32)(block) > (u32)(mTailAllocator)) {
            retVal = 0;
        } else {
            retVal = (u32)sead::PtrUtil::diff(mTailAllocator, block);
        }

        return retVal;
    }
}

#include <nw/snd/snd_FrameHeap.h>

u32 nw::snd::internal::FrameHeap::GetFreeSize() const {
    // NW_ASSERT(IsValid());

    u32 size = m_pHeap->GetAllocatableSize(HEAP_ALIGN);
    if (size < sizeof(Block)) {
        return 0;
    }

    size -= sizeof(Block);
    size &= ~(HEAP_ALIGN - 1);
    return size;
}
