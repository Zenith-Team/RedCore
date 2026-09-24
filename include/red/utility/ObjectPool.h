#pragma once

#include <concepts>

#include <basis/seadAssert.h>
#include <math/seadMathCalcCommon.h>

#include <utility/Bitfield.h>

namespace red {

    template <typename T>
    concept PoolObject = requires(T& t) {
        { t.acquire() } -> std::same_as<void>;
        { t.release() } -> std::same_as<void>;
    };
    
    /**
     * @brief A fixed-capacity, allocation-free container holding a pool of objects which get reused on acquire/release without destruction.
     * Useful for handling recyclable objects which should only be initialized once but can be marked inactive and reused without re-creation. 
     * @tparam T The type of object to hold.
     * @tparam N Maximum capacity of the pool.
     * @details Insertion: O(1), Removal: O(1).
     */
    template <PoolObject T, u32 N>
    class FixedObjectPool {
        static_assert(N > 0, "FixedObjectPool requires N > 0");
        
    public:
        FixedObjectPool()
            : mFreeCount(N)
        {
            for (u32 i = 0; i < N; i++) {
                mFreeIndices[i] = (N-1) - i;
            }
        }
    
        T* acquire() {
            if (mFreeCount == 0) {
                return nullptr;
            }
            
            u32 i = mFreeIndices[--mFreeCount];
            mWork[i].acquire();
            mUsed.setBit(i);
            return &mWork[i];
        }
    
        void release(T* obj) {
            if (obj == nullptr) [[unlikely]]
                return;
            
            SEAD_ASSERT(contains(obj));
            u32 i = obj - mWork;
            SEAD_ASSERT(isUsed(i));
            
            mWork[i].release();
            mUsed.resetBit(i);
            mFreeIndices[mFreeCount++] = i;
        }

        u32 size() const {
            return N - mFreeCount;
        }

        u32 capacity() const {
            return N;
        }

        bool isUsed(u32 i) const {
            return mUsed.isOnBit(i);
        }

        bool contains(const T* obj) const {
            return obj >= mWork && obj < mWork + N;
        }

        template <typename F>
        void forEach(F fn, bool ignoreActivity = false) {
            forEach_(this, fn, ignoreActivity);
        }
    
        template <typename F>
        void forEach(F fn, bool ignoreActivity = false) const {
            forEach_(this, fn, ignoreActivity);
        }

    private:
        template <typename F, typename U>
        static void forEach_(U* self, F fn, bool ignoreActivity = false) {
            for (u32 i = 0; i < N; i++) {
                if (ignoreActivity || self->isUsed(i)) {
                    if constexpr (std::is_void_v<std::invoke_result_t<F&, T&>>)  {
                        fn(self->mWork[i]);
                    } else {
                        const bool shouldContinue = fn(self->mWork[i]);
                        if (!shouldContinue) {
                            break;
                        }
                    }
                }
            }
        }

    private:
        T mWork[N];
        Bitfield<sead::Mathi::roundUpN(N, 32)> mUsed;
        u32 mFreeIndices[N];
        u32 mFreeCount;
    };

} // namespace red
