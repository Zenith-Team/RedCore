#pragma once

#include <container/seadTList.h>
#include <red/event_sys/EventBase.h>
#include <red/event_sys/Listener.h>
#include <telkin/Print.h>

namespace red {
    
    template <typename T> //requires std::derived_from<T, EventBase>
    class EventDelegator {
    public:
        using Callback = void (*)(T& event);

    public:
        class Listener {
        public:
            // higher priority = fires earlier
            Listener(Callback callback, s32 priority = 0)
                : mCallback(callback)
                , mPriority(priority)
                , mNode(this)
            { }
        
            [[clang::always_inline]]
            void invoke(T& event) const {
                return mCallback(event);
            }
            
            [[nodiscard]]
            sead::TListNode<Listener*>& getNode() { return mNode; }
            
            [[nodiscard]]
            s32 getPriority() const { return mPriority; }
        
        private:
            sead::TListNode<Listener*> mNode;
            Callback mCallback;
            s32 mPriority;
        };
        
    public:
        void connect(Listener& listener) {            
            mListeners.pushBack(&listener.getNode());
            mListeners.sort(&compare);
        }
        
        void fire(T& event) const {
            for (const Listener* listener : mListeners) {
                listener->invoke(event);
                
                if (event.isConsumed()) [[unlikely]]
                    return; // stop propagation
            }
        }
        
    private:
        static s32 compare(Listener* const* a, Listener* const* b) {
            if ((*a)->getPriority() < (*b)->getPriority())
                return 1;

            if ((*b)->getPriority() < (*a)->getPriority())
                return -1;

            return 0;
        }
    
    private:
        sead::TList<Listener*> mListeners;
    };
    
}
