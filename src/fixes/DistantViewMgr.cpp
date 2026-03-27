#include <telkin/Hooks.h>

// Disable flicker
tPatch32u(0x022A7974, 0x7C094800); // DistantViewMgr::updateCameraAndProjection, cmpw r9, r9
