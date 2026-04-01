#include <telkin/Telkin.h>

using namespace tk::ppc;

// Disable flicker
tPatch32u(0x022A7974, cmpw(R::r9, R::r9)); // DistantViewMgr::updateCameraAndProjection
