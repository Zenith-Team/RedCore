#include <heap/seadHeapMgr.h>
#include <red/event/TaskPrepareEvent.h>
#include <heap/seadHeap.h>

namespace red {

TaskPrepareEvent::Listener<TaskPrepareEvent::Stage::Before> InstallAllocFailedCallback([](TaskPrepareEvent& e) {
    if (e.getTask()->getName() != "RootTask")
        return;

    static auto deleg = sead::FunctionDelegateCreator<const sead::HeapMgr::AllocFailedCallbackArg*>([](const sead::HeapMgr::AllocFailedCallbackArg* arg) {
        tk::fatal("Failed to allocate 0x%X bytes (align 0x%X) from heap: %s (had free size 0x%X)", arg->request_size, arg->request_alignment, arg->heap->getName().cstr(), arg->heap->getFreeSize());
    });
    
    sead::HeapMgr::instance()->setAllocFailedCallback(&deleg);
}, 10000);

}
