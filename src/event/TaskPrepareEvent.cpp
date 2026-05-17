#include <red/event/TaskPrepareEvent.h>
#include <telkin/Hooks.h>

template <red::TaskPrepareEvent::Stage S>
red::TaskPrepareEvent::Delegator& red::TaskPrepareEvent::getDelegator() {
    static Delegator sDelegator;
    return sDelegator;
}

template red::TaskPrepareEvent::Delegator& red::TaskPrepareEvent::getDelegator<red::TaskPrepareEvent::Stage::Before>();
template red::TaskPrepareEvent::Delegator& red::TaskPrepareEvent::getDelegator<red::TaskPrepareEvent::Stage::After>();

void red::TaskPrepareEvent::hook(sead::TaskBase* task) {
    emit<Stage::Before>(task);
    task->prepare();
    emit<Stage::After>(task);
}
tBranch(0x02A01AA0, red::TaskPrepareEvent::hook, tk::BranchType::bl); // sead::TaskMgr::prepare_
tBranch(0x02A01194, red::TaskPrepareEvent::hook, tk::BranchType::bl); // sead::TaskMgr::createTaskSync
