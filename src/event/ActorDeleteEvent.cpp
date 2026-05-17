#include <red/event/ActorDeleteEvent.h>
#include <telkin/Hooks.h>
#include <red/public/ActorBase.h>

red::ActorDeleteEvent::Delegator& red::ActorDeleteEvent::getDelegator() {
    static Delegator sDelegator;
    return sDelegator;
}

void red::ActorDeleteEvent::subscribe(EventDelegator<ActorDeleteEvent>::Listener& listener) {
    getDelegator().connect(listener);
}

void red::ActorDeleteEvent::emit(ActorBase* actor) {
    ActorDeleteEvent event(actor);
    getDelegator().fire(event);
    
    static_cast<red::pub::ActorBase*>(actor)->doDelete(); // replaced call
}
tBranch(0x02008EE4, red::ActorDeleteEvent::emit, tk::BranchType::bl); // ActorMgr::doDeleteActors_
