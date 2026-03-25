#pragma once

#include <red/event_sys/EventDelegator.h>

namespace red {
    
    template <typename T>
    class EventBase {
    public:
        using Delegator = EventDelegator<T>;
        using Listener = Delegator::Listener;
        
    public:
        void consume() { mConsumed = true; }
        
        [[nodiscard]]
        bool isConsumed() const { return mConsumed; }
    
    private:
        bool mConsumed = false;
    };
    
}
