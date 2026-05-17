#pragma once

#include <red/event_sys/EventBase.h>
#include <red/event_sys/EventDelegator.h>
#include <framework/seadTaskBase.h>

namespace red {
    
    /**
     * @brief Event fired when a task is being prepared.
     * @warning This event may be fired on other threads.
     */
    class TaskPrepareEvent : public EventBase<TaskPrepareEvent> {
    public:
        enum class Stage {
            Before,
            After
        };
        
        template <Stage S>
        class Listener {
        public:
            Listener(ListenerFunc f, s32 priority = 0)
                : mListener(f, priority)
            {
                TaskPrepareEvent::subscribe<S>(mListener);
            }
            
        private:
            EventDelegator<TaskPrepareEvent>::Listener mListener;
        };
        
    public:
        [[nodiscard]]
        sead::TaskBase* getTask() const { return mTask; }
        
    private:
        explicit TaskPrepareEvent(sead::TaskBase* task)
            : mTask(task)
        { }
        
        template <Stage S>
        [[nodiscard]]
        static Delegator& getDelegator();
        
    private:
        template <Stage S>
        static void subscribe(EventDelegator<TaskPrepareEvent>::Listener& listener) {
            getDelegator<S>().connect(listener);
        }
        
        // Off-thread: 0x02A01AA0, On-thread: 0x02A01194
        template <Stage S>
        static void emit(sead::TaskBase* task) {
            TaskPrepareEvent event(task);
            getDelegator<S>().fire(event);
        }
        
    public: //! TODO: Make private
        static void hook(sead::TaskBase* task);
        
    private:
        sead::TaskBase* mTask;
    };
    
}
