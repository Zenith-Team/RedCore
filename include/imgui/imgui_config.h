#pragma once

#include <telkin/Preprocessor.h>
#include <telkin/Print.h>
#include <cafe/os.h>

#include <gfx/seadColor.h>
#include <math/seadVector.h>

#define IM_ASSERT(_EXPR)                                        \
do                                                              \
{                                                               \
    if (!(_EXPR))                                               \
    {                                                           \
        tk::print("ImGui assert failed: " PP_STR(_EXPR) "\n");  \
        OSFatal("ImGui assert failed: " PP_STR(_EXPR));         \
    }                                                           \
}                                                               \
while (false)

#define IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS
#define IMGUI_DISABLE_FILE_FUNCTIONS
#define IMGUI_DISABLE_DEFAULT_ALLOCATORS
#define IMGUI_DISABLE_DEFAULT_FONT

#define IM_VEC2_CLASS_EXTRA                                                 \
        constexpr ImVec2(const sead::Vector2f& f) : x(f.x), y(f.y) {}       \
        operator sead::Vector2f() const { return sead::Vector2f(x, y); }

#define IM_VEC4_CLASS_EXTRA                                                             \
        constexpr ImVec4(const sead::Vector4f& f) : x(f.x), y(f.y), z(f.z), w(f.w) {}   \
        operator sead::Vector4f() const { return sead::Vector4f(x, y, z, w); }          \
                                                                                        \
        constexpr ImVec4(const sead::Color4f& f) : x(f.r), y(f.g), z(f.b), w(f.a) {}    \
        operator sead::Color4f() const { return sead::Color4f(x, y, z, w); }
