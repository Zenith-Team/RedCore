#pragma once

#include <red/event_sys/EventBase.h>
#include <red/event_sys/EventDelegator.h>
#include <heap/seadHeap.h>

namespace red {
    
    class ResourceLoadEvent : public EventBase<ResourceLoadEvent> {
    public:
        enum class Stage {
            Boot,
            Course,
            CourseSelect
        };
        
        template <Stage S>
        class Listener {
        public:
            Listener(ListenerFunc f, s32 priority = 0)
                : mListener(f, priority)
            {
                ResourceLoadEvent::subscribe<S>(mListener);
            }
            
        private:
            EventDelegator<ResourceLoadEvent>::Listener mListener;
        };
    
    private:
        explicit ResourceLoadEvent(sead::Heap* heap)
            : mHeap(heap)
        { }
    
    public:
        [[nodiscard]]
        sead::Heap* getHeap() const { return mHeap; }
        
    private:
        template <Stage S>
        [[nodiscard]]
        static Delegator& getDelegator();
    
        template <Stage S>
        static void subscribe(EventDelegator<ResourceLoadEvent>::Listener& listener) {
            getDelegator<S>().connect(listener);
        }
        
    public: //! TODO: Make private
        // 0x0200A8FC
        static void emitCSBoot(sead::Heap* heap, Stage type);
        // 0x0200A884
        static void emitCourse(sead::Heap* heap);
    
    private:
        sead::Heap* mHeap;
    };
    
}
