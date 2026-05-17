#include <red/event/ActorCreateEvent.h>
#include <telkin/Hooks.h>
#include <red/public/ActorBase.h>

red::ActorCreateEvent::Delegator& red::ActorCreateEvent::getDelegator() {
    static Delegator sDelegator;
    return sDelegator;
}

void red::ActorCreateEvent::subscribe(EventDelegator<ActorCreateEvent>::Listener& listener) {
    getDelegator().connect(listener);
}

void red::ActorCreateEvent::emit(ActorBase* actor) {
    ActorCreateEvent event(actor);
    getDelegator().fire(event);
    
    static_cast<red::pub::ActorBase*>(actor)->create(); // replaced call
}
tBranch(0x020092B8, red::ActorCreateEvent::emit, tk::BranchType::bl); // ActorMgr::doCreate_
