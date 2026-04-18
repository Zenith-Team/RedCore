#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <new>
#include <source_location>
#include <span>
#include <type_traits>
#include <utility>

#include <cafe.h>

#include <heap/seadHeap.h>
#include <math/seadMathCalcCommon.h>
#include <math/seadVector.h>
#include <random/seadGlobalRandom.h>

#include <audio/GameAudio.h>
#include <boss/Boss.h>
#include <course_select/CourseSelectTask.h>
#include <effect/EffectCreateUtil.h>
#include <enemy/Enemy.h>
#include <enemy/Kuribo.h>
#include <game/CourseTask.h>
#include <graphics/JointBlendModel.h>
#include <player/PlayerObject.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
