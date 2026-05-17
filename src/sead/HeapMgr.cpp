#include <heap/seadHeapMgr.h>

sead::HeapMgr::IAllocFailedCallback* sead::HeapMgr::setAllocFailedCallback(IAllocFailedCallback* callback) {
    IAllocFailedCallback* prev = mAllocFailedCallback;
    mAllocFailedCallback = callback;
    return prev;
}
