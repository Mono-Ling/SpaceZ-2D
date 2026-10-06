#include "Debug/NativeDebug.h"

extern "C" __declspec(dllexport) int Add(int a, int b)
{
    Debug::Log("Add function called with arguments: " + std::to_string(a) + ", " + std::to_string(b));
    return a + b;
}