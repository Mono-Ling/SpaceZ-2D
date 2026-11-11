#pragma once
#include <string>
#include <concepts>

namespace Core
{
    template<typename T>
    std::string ToString(const T& message) = delete;

    template<typename T>
    concept StdToStringAbility = requires(const T& msg)
    {
        {std::to_string(msg)} -> std::same_as<std::string>;
    };
    template<typename T>
    concept CreateStringAbility = requires(const T& msg)
    {
        {std::string{msg}} -> std::same_as<std::string>;
    };


    template<typename T>
    requires StdToStringAbility<T>
    std::string ToString(const T& message);

    template<typename T>
    requires CreateStringAbility<T> && (!StdToStringAbility<T>)
    std::string ToString(const T& message);


    template<typename T>
    concept ToStringAbility = requires(const T& message)
    {
        {ToString(message)} -> std::same_as<std::string>;
    };


    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    std::string StringFormat(Args... args);
}
#include "Core/Debug/ToString.tpp"