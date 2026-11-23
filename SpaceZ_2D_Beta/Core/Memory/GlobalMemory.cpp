#include "Core/Memory/MemoryMgr.h"
using namespace Core::Memory;
using namespace std;

MemoryMgr _memoryMgr;

void* ::operator new(size_t size)
{
    return _memoryMgr.Alloc(size);
}
void ::operator delete(void* ptr)
{
    _memoryMgr.Free(ptr);
}

void* ::operator new[](size_t size)
{
    return _memoryMgr.Alloc(size);
}
void  ::operator delete[](void* ptr)
{
    _memoryMgr.Free(ptr);
}