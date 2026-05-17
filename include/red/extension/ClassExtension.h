#pragma once

#include <container/seadTList.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <basis/seadNew.h>
#include <math/seadMathCalcCommon.h>
#include <type_traits>

namespace red {

    class ClassExtension : public sead::TListNode<ClassExtension*> { // <-- this just builds a memory layout for the extension
    public:
        ClassExtension(const sead::RuntimeTypeInfo::Interface* typeInfo)
            : sead::TListNode<ClassExtension*>(this)
            , mSize(0)
            , mAlignment(sead::cDefaultAlignment)
            , mTypeInfo(typeInfo)
        { }
    
        template <typename T>
        [[nodiscard]]
        u32 add() {
            static_assert(std::is_trivially_constructible_v<T>, "T must not have a constructor");
            static_assert(std::is_trivially_destructible_v<T>, "T must not have a destructor");
            
            u32 alignment = alignof(T);
            
            mAlignment = sead::Mathu::max(mAlignment, alignment);
            
            // round up to alignment
            mSize = (mSize + alignment - 1) & ~(alignment - 1); // sead::Mathu::roundUp()
            
            u32 start = mSize;
            mSize += sizeof(T);
            return start;
        }
        
        void finish() {
            // round up to alignment
            mSize = (mSize + mAlignment - 1) & ~(mAlignment - 1);
        }
        
        [[nodiscard]]
        u32 getSize() const {
            return mSize;
        }
        
        [[nodiscard]]
        u32 getAlignment() const {
            return mAlignment;
        }
        
        [[nodiscard]]
        const sead::RuntimeTypeInfo::Interface* getTypeInfo() const {
            return mTypeInfo;
        }
    
    private:
        u32 mSize;
        u32 mAlignment;
        const sead::RuntimeTypeInfo::Interface* mTypeInfo;
    };
    
    class ClassExtensionInstance : public sead::TListNode<ClassExtensionInstance*> {
    public:
        ClassExtensionInstance(u32 size, u8* data, const sead::RuntimeTypeInfo::Interface* typeInfo)
            : sead::TListNode<ClassExtensionInstance*>(this)
            , mSize(size)
            , mData(data)
            , mTypeInfo(typeInfo)
        { }
        
        template <typename T>
        [[nodiscard]]
        T* get(u32 offset) {
            if (offset + sizeof(T) > mSize) [[unlikely]] {
               // tk::fatal("bad");
                return nullptr;
            }
            
            return reinterpret_cast<T*>(mData + offset);
        }
        
        [[nodiscard]]
        const sead::RuntimeTypeInfo::Interface* getTypeInfo() const {
            return mTypeInfo;
        }
    
    private:
        u32 mSize;
        u8* mData;
        const sead::RuntimeTypeInfo::Interface* mTypeInfo;
    };
}
