#pragma once

#include <actor/Actor.h>

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
    };

} // namespace red
