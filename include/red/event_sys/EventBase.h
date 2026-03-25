#pragma once

namespace red {
    
    class EventBase {
    public:
        void consume() { mConsumed = true; }
        
        [[nodiscard]]
        bool isConsumed() const { return mConsumed; }
    
    private:
        bool mConsumed = false;
    };
    
}
