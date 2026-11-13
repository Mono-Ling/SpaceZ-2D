#pragma once
#include "Core/Debug/ToString.h"

namespace Core::Collision
{
    struct LayerMask
    {
        static constexpr unsigned int MAX_LAYER_COUNT = 64;

        unsigned long long layer = 0;

        constexpr LayerMask() = default;
        constexpr explicit LayerMask(unsigned long long layer) : layer(layer) {}

        constexpr explicit operator bool() const { return layer ^ Null.layer; }
        constexpr LayerMask operator|(LayerMask other) const { return Union(other); }
        constexpr LayerMask operator&(LayerMask other) const { return LayerMask{layer & other.layer}; }
        constexpr bool Has(const LayerMask& other) const
        {
            return !(other.layer ^ (other.layer & layer));
        }
        constexpr bool Intersect(const LayerMask& other) const
        {
            return other.layer & layer;
        }

        constexpr LayerMask& UnionSelf(const LayerMask& other)
        {
            layer |= other.layer;
            return *this;
        }
        constexpr LayerMask& DiffSelf(const LayerMask& other)
        {
            layer &= ~other.layer;
            return *this;
        }
        constexpr LayerMask Union(const LayerMask& other) const
        {
            return LayerMask(layer | other.layer);
        }
        constexpr LayerMask Diff(const LayerMask& other) const
        {
            return LayerMask(layer & ~other.layer);
        }

        constexpr void AddLayer(unsigned int layerId)
        {
            if (layerId >= MAX_LAYER_COUNT)
                return;
            layer |= 1ULL << layerId;
        }
        constexpr void RemoveLayer(unsigned int layerId)
        {
            if (layerId >= MAX_LAYER_COUNT)
                return;
            layer &= ~(1ULL << layerId);
        }

        static const LayerMask Default;
        static const LayerMask Null;
        static const LayerMask All;
    };

    inline constexpr const LayerMask LayerMask::Default(1 << 0);
    inline constexpr const LayerMask LayerMask::Null(0);
    inline constexpr const LayerMask LayerMask::All(~(unsigned long long)0);

    inline constexpr bool operator==(const LayerMask& a, const LayerMask& b)
    {
        return !(a.layer ^ b.layer);
    }
    inline constexpr bool operator!=(const LayerMask& a, const LayerMask& b)
    {
        return !(a == b);
    }
    inline constexpr bool IsIntersect(const LayerMask& a, const LayerMask& b)
    {
        return a.Intersect(b);
    }
}
namespace Core
{
    template<>
    inline std::string ToString<Collision::LayerMask>(const Collision::LayerMask& mask)
    {
        return ToString(mask.layer);
    }
}