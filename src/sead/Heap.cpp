#include <heap/seadExpHeap.h>
#include <heap/seadFrameHeap.h>
#include <heap/seadSeparateHeap.h>
#include <heap/seadUnitHeap.h>
#include <telkin/Telkin.h>

const void* sead::UnitHeap::getStartAddress() const { return mStart; }
tPointer(0x101914A4, sead::UnitHeap::getStartAddress, false);

const void* sead::SeparateHeap::getStartAddress() const { return mStart; }
tPointer(0x101A4658, sead::SeparateHeap::getStartAddress, false);

size_t sead::ExpHeap::getSize() const { return mSize; }
tPointer(0x10191094, sead::ExpHeap::getSize, false);

size_t sead::FrameHeap::getSize() const { return mSize; }
tPointer(0x101911C4, sead::FrameHeap::getSize, false);

size_t sead::UnitHeap::getSize() const { return mSize; }
tPointer(0x101914B4, sead::UnitHeap::getSize, false);
