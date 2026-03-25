#define TELKIN_REGISTERS
#include <layer/aglLayer.h>
#include <layer/aglRenderStep.h>
#include <layer/aglRenderInfo.h>
#include <prim/seadDelegateEventSlot.h>
#include <red/event/RenderStepEvent.h>
#include <telkin/Telkin.h>

red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegatorPre() {
    static Delegator sDelegator;
    return sDelegator;
}

red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegatorAfterPre() {
    static Delegator sDelegator;
    return sDelegator;
}

red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegatorBeforePost() {
    static Delegator sDelegator;
    return sDelegator;
}

red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegatorPost() {
    static Delegator sDelegator;
    return sDelegator;
}

void red::RenderStepEvent::subscribe(Listener& listener, EventStage stage) {
    switch (stage) {
        case EventStage::Pre: {
            return getDelegatorPre().connect(listener);
        }
        
        case EventStage::AfterPre: {
            return getDelegatorAfterPre().connect(listener);
        }
        
        case EventStage::BeforePost: {
            return getDelegatorBeforePost().connect(listener);
        }
        
        case EventStage::Post: {
            return getDelegatorPost().connect(listener);
        }
        
        default: {
            tk::print("ERROR: Invalid event stage: %d\n", stage);
        }
    }
}

void red::RenderStepEvent::emit(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo, EventStage stage) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    
    switch (stage) {
        case EventStage::Pre: {
            return getDelegatorPre().fire(event);
        }
        
        case EventStage::AfterPre: {
            return getDelegatorAfterPre().fire(event);
        }
        
        case EventStage::BeforePost: {
            return getDelegatorBeforePost().fire(event);
        }
        
        case EventStage::Post: {
            return getDelegatorPost().fire(event);
        }
        
        default: {
            tk::print("ERROR: Invalid event stage: %d\n", stage);
        }
    }
}

void red::RenderStepEvent::hookPre() tAssembly(
    mr r2, r1; // reg saving modifies the stack pointer, so smuggle it through r2
    
    tSaveVolatileRegisters;
    
    // layer: r3 (inherited)
    // renderasdl: r27
    mr r4, r27;
    // renderstep: r29
    mr r5, r29;
    // renderinfo: sp+0x94
    addi r6, r2, 0x94;
    // stage: pre
    li r7, 1;
    // emit!
    bl _ZN3red15RenderStepEvent4emitEPN3agl3lyr5LayerEbPNS2_10RenderStepEPNS2_10RenderInfoENS_10EventStageE;
    
    bctrl; // replaced instruction
    
    // reload.
    // layer: r28
    mr r3, r28;
    // renderasdl: r27
    mr r4, r27;
    // renderstep: r29
    mr r5, r29;
    // renderinfo: sp+0x94
    addi r6, r2, 0x94;
    // stage: afterpre
    li r7, 2;
    // emit!
    bl _ZN3red15RenderStepEvent4emitEPN3agl3lyr5LayerEbPNS2_10RenderStepEPNS2_10RenderInfoENS_10EventStageE;
    
    tRestoreVolatileRegisters;
    blr;
)
tBranch(0x02A39644, red::RenderStepEvent::hookPre, tk::BranchType::bl); // agl::lyr::Layer::draw_()
tBranch(0x02A395D0, red::RenderStepEvent::hookPre, tk::BranchType::bl); // agl::lyr::Layer::draw_()

void red::RenderStepEvent::hookPost() tAssembly(
    mr r2, r1; // reg saving modifies the stack pointer, so smuggle it through r2
    
    tSaveVolatileRegisters;
    
    // layer: r3 (inherited)
    // renderasdl: r27
    mr r4, r27;
    // renderstep: r29
    mr r5, r29;
    // renderinfo: sp+0x94
    addi r6, r2, 0x94;
    // stage: beforepost
    li r7, 3;
    // emit!
    bl _ZN3red15RenderStepEvent4emitEPN3agl3lyr5LayerEbPNS2_10RenderStepEPNS2_10RenderInfoENS_10EventStageE;
    
    bctrl; // replaced instruction
    
    // reload.
    // layer: r28
    mr r3, r28;
    // renderasdl: r27
    mr r4, r27;
    // renderstep: r29
    mr r5, r29;
    // renderinfo: sp+0x94
    addi r6, r2, 0x94;
    // stage: post
    li r7, 4;
    // emit!
    bl _ZN3red15RenderStepEvent4emitEPN3agl3lyr5LayerEbPNS2_10RenderStepEPNS2_10RenderInfoENS_10EventStageE;
    
    tRestoreVolatileRegisters;
    blr;
)
tBranch(0x02A39668, red::RenderStepEvent::hookPost, tk::BranchType::bl); // agl::lyr::Layer::draw_()
tBranch(0x02A395F4, red::RenderStepEvent::hookPost, tk::BranchType::bl); // agl::lyr::Layer::draw_()
