#pragma once

#include <red/event_sys/EventDelegator.h>

namespace red {
    
    template <typename T>
    class EventBase {
    public:
        using Delegator = EventDelegator<T>;
        using ListenerFunc = void (*)(T&);
        
    public:
        void consume() { mConsumed = true; }
        
        [[nodiscard]]
        bool isConsumed() const { return mConsumed; }
    
    protected:
        class BasicListener {
        public:
            BasicListener(ListenerFunc f, s32 priority = 0)
                : mListener(f, priority)
            {
                T::subscribe(mListener);
            }
            
        private:
            EventDelegator<T>::Listener mListener;
        };
    
    private:
        bool mConsumed = false;
    };
    
}
