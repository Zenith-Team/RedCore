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

void red::RenderStepEvent::subscribePre(EventDelegator<RenderStepEvent>::Listener& listener) {
    getDelegatorPre().connect(listener);
}

void red::RenderStepEvent::subscribeAfterPre(EventDelegator<RenderStepEvent>::Listener& listener) {
    getDelegatorAfterPre().connect(listener);
}

void red::RenderStepEvent::subscribeBeforePost(EventDelegator<RenderStepEvent>::Listener& listener) {
    getDelegatorBeforePost().connect(listener);
}

void red::RenderStepEvent::subscribePost(EventDelegator<RenderStepEvent>::Listener& listener) {
    getDelegatorPost().connect(listener);
}

void red::RenderStepEvent::emitPre(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    getDelegatorPre().fire(event);
}

void red::RenderStepEvent::emitAfterPre(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    getDelegatorAfterPre().fire(event);
}

void red::RenderStepEvent::emitBeforePost(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    getDelegatorBeforePost().fire(event);
}

void red::RenderStepEvent::emitPost(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    getDelegatorPost().fire(event);
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
    // emit! (Pre)
    bl _ZN3red15RenderStepEvent7emitPreEPN3agl3lyr5LayerEbPNS2_10RenderStepEPNS2_10RenderInfoE;
    
    bctrl; // replaced instruction agl::lyr::Layer::preDraw(this, &renderInfo);
    
    // reload.
    // layer: r28
    mr r3, r28;
    // renderasdl: r27
    mr r4, r27;
    // renderstep: r29
    mr r5, r29;
    // renderinfo: sp+0x94
    addi r6, r2, 0x94;
    // emit! (AfterPre)
    bl _ZN3red15RenderStepEvent12emitAfterPreEPN3agl3lyr5LayerEbPNS2_10RenderStepEPNS2_10RenderInfoE;
    
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
    // emit! (BeforePost)
    bl _ZN3red15RenderStepEvent14emitBeforePostEPN3agl3lyr5LayerEbPNS2_10RenderStepEPNS2_10RenderInfoE;
    
    bctrl; // replaced instruction agl::lyr::Layer::postDraw(this, &renderInfo);
    
    // reload.
    // layer: r28
    mr r3, r28;
    // renderasdl: r27
    mr r4, r27;
    // renderstep: r29
    mr r5, r29;
    // renderinfo: sp+0x94
    addi r6, r2, 0x94;
    // emit! (Post)
    bl _ZN3red15RenderStepEvent8emitPostEPN3agl3lyr5LayerEbPNS2_10RenderStepEPNS2_10RenderInfoE;
    
    tRestoreVolatileRegisters;
    blr;
)
tBranch(0x02A39668, red::RenderStepEvent::hookPost, tk::BranchType::bl); // agl::lyr::Layer::draw_()
tBranch(0x02A395F4, red::RenderStepEvent::hookPost, tk::BranchType::bl); // agl::lyr::Layer::draw_()
