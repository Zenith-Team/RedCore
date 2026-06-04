#include <red/event/ResourceLoadEvent.h>

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

template <red::ResourceLoadEvent::Stage S>
red::ResourceLoadEvent::Delegator& red::ResourceLoadEvent::getDelegator() {
    static Delegator sDelegator;
    return sDelegator;
}

template red::ResourceLoadEvent::Delegator& red::ResourceLoadEvent::getDelegator<red::ResourceLoadEvent::Stage::Boot>();
template red::ResourceLoadEvent::Delegator& red::ResourceLoadEvent::getDelegator<red::ResourceLoadEvent::Stage::Course>();
template red::ResourceLoadEvent::Delegator& red::ResourceLoadEvent::getDelegator<red::ResourceLoadEvent::Stage::CourseSelect>();

void red::ResourceLoadEvent::emitCourse(sead::Heap* heap) {
    ResourceLoadEvent event(heap);
    getDelegator<Stage::Course>().fire(event);
    
    heap->adjust(); // replaced call
}
tBranch(0x0200A884, red::ResourceLoadEvent::emitCourse, tk::BranchType::bl);

void red::ResourceLoadEvent::emitCSBoot(sead::Heap* heap, Stage type) {
    ResourceLoadEvent event(heap);
    
    if (type == Stage::Boot) {
        getDelegator<Stage::Boot>().fire(event);
    } else {
        getDelegator<Stage::CourseSelect>().fire(event);
    }
    
    heap->adjust(); // replaced call
}

namespace red {
    void hookResourceLoadCSBoot() tAssembly(
        mr r4, r26;
        b _ZN3red17ResourceLoadEvent10emitCSBootEPN4sead4HeapENS0_5StageE;
    )
}
tBranch(0x0200A8FC, red::hookResourceLoadCSBoot, tk::BranchType::bl);
