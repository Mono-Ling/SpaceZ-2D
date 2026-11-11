#pragma once
#include "Core/Debug/ToString.h"

namespace Core
{
    template<typename T>
    requires StdToStringAbility<T>
    std::string ToString(const T& message)
    {
        return std::to_string(message);
    }

    template<typename T>
    requires CreateStringAbility<T> && (!StdToStringAbility<T>)
    std::string ToString(const T& message)
    {
        return {message};
    }


    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    std::string StringFormat(Args... args)
    {
        std::string str;
        (str.append(ToString(args)), ...);
        return str;
    }
}