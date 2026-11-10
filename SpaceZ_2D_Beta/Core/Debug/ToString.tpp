#pragma once
#include "Core/Debug/ToString.h"

namespace Core
{
    template<typename T>
    requires requires(const T& msg)
    {
        {std::to_string(msg)} -> std::same_as<std::string>;
    }
    std::string ToString(const T& message)
    {
        return std::to_string(message);
    }

    template<typename T>
    requires requires(const T& msg)
    {
        {std::string{msg}} -> std::same_as<std::string>;
    }
    std::string ToString(const T& message)
    {
        return {message};
    }
}