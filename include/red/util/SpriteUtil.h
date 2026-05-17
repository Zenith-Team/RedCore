#pragma once

#include <actor/Actor.h>
#include <telkin/Print.h>

namespace red {

    class SpriteUtil : private Actor {
    public:
        [[nodiscard]]
        static u8 getNybble1(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mSwitchFlag1 >> 0x04 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble2(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mSwitchFlag1 >> 0x00 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble3(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mSwitchFlag0 >> 0x04 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble4(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mSwitchFlag0 >> 0x00 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble5(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam0 >> 0x1C & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble6(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam0 >> 0x18 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble7(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam0 >> 0x14 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble8(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam0 >> 0x10 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble9(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam0 >> 0x0C & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble10(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam0 >> 0x08 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble11(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam0 >> 0x04 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble12(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam0 >> 0x00 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble13(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam1 >> 0x1C & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble14(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam1 >> 0x18 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble15(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam1 >> 0x14 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble16(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam1 >> 0x10 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble17(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam1 >> 0x0C & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble18(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam1 >> 0x08 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble19(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam1 >> 0x04 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble20(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParam1 >> 0x00 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble21(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParamEx.course.movement_id >> 0x04 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble22(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParamEx.course.movement_id >> 0x00 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble23(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParamEx.course.link_id >> 0x04 & 0xF;
        }
        
        [[nodiscard]]
        static u8 getNybble24(Actor* target) {
            return static_cast<SpriteUtil*>(target)->mParamEx.course.link_id >> 0x00 & 0xF;
        };

        //* Advanced functions

        /**
         * @brief Extract the value of an arbitrary 32-bit range of bits from the given actor's full 96 bits of spritedata which make up all nybbles 1-24.
         * *Bit layout cheatsheet:*
         * nybbles 1-4 (bits 0-15), nybbles 5-12 (bits 16-47), nybbles 13-20 (bits 48-79), nybbles 21-24 (bits 80-96)
         *
         * @details Max. output is limited to a 32-bit value for performance, if the requested range exceeds 32 bits in size or is invalid, @c OSFatal is called.
         * Use @c mParam0/1 directly to manually extract values greater than 32 bits.
         *
         * @param from 0-indexed Big Endian (left-to-right) *inclusive* bit index to start the range at.
         * @param end 0-indexed Big Endian (left-to-right) *exclusive* bit index to end the range at.
         */
        [[nodiscard]]
        static u32 getBitRange(Actor* target, u8 from, u8 end) {
            const u8 bitsCount = end - from;
            if (bitsCount > 32) tk::fatal("red::SpriteUtil::getBitRange called with an invalid bit range. (%i - %i)", from, end);
            if (bitsCount == 0) return 0;

            SpriteUtil* actor = static_cast<SpriteUtil*>(target);

            // Fast path 1: The entire range fits within the 16-bit mSwitchFlags fields (bits 0 - 15)
            if (end <= 16) {
                u32 flags = (static_cast<u32>(actor->mSwitchFlag1) << 8) | actor->mSwitchFlag0;
                return getBitRangeInt32(flags << 16, from, end);
            }
            // Fast path 2: The entire range fits within mParam0 field (bits 16 - 47)
            if (from >= 16 && end <= 48) {
                return getBitRangeInt32(actor->mParam0, from - 16, end - 16);
            }
            // Fast path 3: The entire range fits within mParam1 field (bits 48 - 79)
            if (from >= 48 && end <= 80) {
                return getBitRangeInt32(actor->mParam1, from - 48, end - 48);
            }
            // Fast path 4: The entire range fits within the 16-bit mParamEx fields (bits 80 - 95)
            if (from >= 80) {
                u32 paramEx = (static_cast<u32>(actor->mParamEx.course.movement_id) << 8) | actor->mParamEx.course.link_id;
                return getBitRangeInt32(paramEx << 16, from - 80, end - 80);
            }

            // Slow path: The range crosses a boundary between two fields.
            // Because bitsNum <= 32, it can only cross exactly 1 of the 3 major boundaries.
            u32 upperField = 0;
            u32 lowerField = 0;
            u8 boundaryBit = 0;

            if (from < 16) { // Crosses mSwitchFlags <-> mParam0 boundary at bit 16
                boundaryBit = 16;
                upperField = (static_cast<u32>(actor->mSwitchFlag1) << 8) | actor->mSwitchFlag0;
                upperField <<= 16; // Align to MSB of a 32-bit window
                lowerField = actor->mParam0;
            }
            else if (from < 48) { // Crosses mParam0 <-> mParam1 boundary at bit 48
                boundaryBit = 48;
                upperField = actor->mParam0;
                lowerField = actor->mParam1;
            }
            else { // Crosses mParam1 <-> mParamEx boundary at bit 80
                boundaryBit = 80;
                upperField = actor->mParam1;
                lowerField = (static_cast<u32>(actor->mParamEx.course.movement_id) << 8) | actor->mParamEx.course.link_id;
                lowerField <<= 16; // Align to MSB of a 32-bit window
            }

            // Extract the pieces from both fields and stitch them together
            const u8 upperBitsCount = boundaryBit - from;
            const u8 lowerBitsCount = bitsCount - upperBitsCount;

            const u32 upperValue = getBitRangeInt32(upperField, 32 - upperBitsCount, 32);
            const u32 lowerValue = getBitRangeInt32(lowerField, 0, lowerBitsCount);

            return (upperValue << lowerBitsCount) | lowerValue;
        }

    private:
        /** ! Uses MSB (big endian) logic: a u16 will be treated as having bits 0-16 empty if not pre-emptively shifted up. */
        [[nodiscard]]
        static u32 getBitRangeInt32(u32 num, u8 from, u8 end) {
            const u8 bitsCount = end - from;
            if (bitsCount >= 32) {
                return num;
            }
            const u32 shifted = num >> (32 - end);
            const u32 mask = (1U << bitsCount) - 1;
            return shifted & mask;
        }
    };

} // namespace red
