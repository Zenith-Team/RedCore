#include <red/event/StaticInitEvent.h>
#include <telkin/Hooks.h>
#include <red/public/ActorBase.h>

red::StaticInitEvent::Delegator& red::StaticInitEvent::getDelegator() {
    static Delegator sDelegator;
    return sDelegator;
}

void red::StaticInitEvent::subscribe(EventDelegator<StaticInitEvent>::Listener& listener) {
    getDelegator().connect(listener);
}

void red::StaticInitEvent::emit() {
    // guard because __call_ctors is called twice for some reason
    static bool inited = false;
    if (inited) {
        return;
    }
    inited = true;

    StaticInitEvent event;
    getDelegator().fire(event);
}
tBranch(0x02AFA050, red::StaticInitEvent::emit, tk::BranchType::b); // __call_ctors
