#pragma once

#include <heap/seadExpHeap.h>

namespace red {

    class RedCoreHeap {
    public:
        static sead::Heap* instance() { return sInstance; }
        
    public: // TODO: Make private
        static void create(sead::Heap* parentHeap);
    
    private:
        static sead::ExpHeap* sInstance;
    };
    
}
