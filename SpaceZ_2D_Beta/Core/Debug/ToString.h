#pragma once
#include <string>
#include <concepts>

namespace Core
{
    template<typename T>
    std::string ToString(const T& message) = delete;


    template<typename T>
    requires requires(const T& msg)
    {
        {std::to_string(msg)} -> std::same_as<std::string>;
    }
    std::string ToString(const T& message);

    template<typename T>
    requires requires(const T& msg)
    {
        {std::string{msg}} -> std::same_as<std::string>;
    }
    std::string ToString(const T& message);


    template<typename T>
    concept ToStringAbility = requires(const T& message)
    {
        {ToString(message)} -> std::same_as<std::string>;
    };
}
#include "Core/Debug/ToString.tpp"