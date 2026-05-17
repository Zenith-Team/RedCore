#include "prim/seadDelegate.h"
#include <red/heap/RedCoreHeap.h>
#include <telkin/Hooks.h>
#include <telkin/Print.h>
#include <red/event/TaskPrepareEvent.h>

sead::ExpHeap* red::RedCoreHeap::sInstance = nullptr;

void red::RedCoreHeap::create(sead::Heap* parentHeap) {
    sInstance = sead::ExpHeap::tryCreate(99 * 1024 * 1024, "RedCoreHeap", parentHeap); // 99 MiB
}

namespace red {

TaskPrepareEvent::Listener<TaskPrepareEvent::Stage::Before> MakeRedCoreHeap([](TaskPrepareEvent& e) {
    if (e.getTask()->getName() != "RootTask")
        return;
    
    RedCoreHeap::create(e.getTask()->getHeap());
}, 10000);

}

