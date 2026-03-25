#pragma once

#include <types.h>
#include <red/event_sys/EventStage.h>

namespace red {
    
    template <typename T>
    class Listener {
    public:
        Listener(void (*func)(T&), s32 priority = 0)
            : mFunc(func)
            , mListener(func, priority)
        {
            T::subscribe(mListener);
        }
    
        Listener(red::EventStage stage, void (*func)(T&), s32 priority = 0)
            : mFunc(func)
            , mListener(func, priority)
        {
            T::subscribe(mListener, stage);
        }
        
        void (*mFunc)(T&);
        typename T::Listener mListener;
    };
    
}
