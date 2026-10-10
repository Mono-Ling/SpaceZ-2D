#pragma once

namespace Core::Solver
{
    namespace PhyQuantities
    {
        /// @brief 松弛系数
        constexpr float RELAXATION = 0.2f;

        /// @brief 最大位置修正量
        constexpr float MAX_POS_CORRECTION = 0.2f;
        
        /// @brief 允许穿透
        constexpr float PENETRATE_SLOP = 0.005f;
    }
}
