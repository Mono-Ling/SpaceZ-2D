#include "Solver/SolverPair.h"
#include "Math/Vector2.h"
#include "Math/Math.h"
using namespace Core::Math;
using namespace Core::Collision;
using namespace std;

namespace Core::Solver
{
    SolverPair::SolverPair(const vector<CollisionInfo>& infos)
    {
        for(auto& info : infos)
        {
            float halfDepth = info.depth / 2.0f;
            auto normal = info.normal.Normalized();
            // 因depth为穿透深度，因此沿法线正方向半长才为first锚点
            pair<Vector2, Vector2> anchor
            {
                info.point + normal * halfDepth,
                info.point - normal * halfDepth,
            };
            SolverCollisionPoint point(info.point, normal, anchor);
            
            solverPoints.push_back(point);
        }
    }
    SolverPair::operator bool() const
    {
        return first && second && !solverPoints.empty();
    }
    void SolverPair::SetBody(SolverRigidbody* a, SolverRigidbody* b)
    {
        if(!a || !b)
            return;
        first = a;
        second = b;

        for(auto& point : solverPoints)
            point.SetBody(a, b);
    }
    void SolverPair::SetContact(const pair<float, float>& sf, const pair<float, float>& df, const pair<float, float>& e)
    {
        staticFriction = Min(sf.first, sf.second);
        dynamicFriction = Min(df.first, df.second);
        elasticity = Clamp01(Min(e.first, e.second));
    }
    void SolverPair::InitSolverPair()
    {
        if(!first || !second)
            return;
        for(auto& point : solverPoints)
            point.SetTargetRelatVelocity(elasticity, first, second);
    }
    void SolverPair::ApplyNormalImpulse()
    {
        if(!first || !second)
            return;

        vector<Vector2> impulseArr(solverPoints.size());
        for(int i = 0; i < solverPoints.size(); i++)
            impulseArr[i] = solverPoints[i].GetDeltaNormalImpulse(first, second);
        
        for(int i = 0; i < solverPoints.size(); i++)
        {
            first->ApplyImpulse(-impulseArr[i]);
            second->ApplyImpulse(impulseArr[i]);

            first->ApplyImpulseMoment(Cross(solverPoints[i].radiusVector.first, -impulseArr[i]));
            second->ApplyImpulseMoment(Cross(solverPoints[i].radiusVector.second, impulseArr[i]));
        }
    }
    void SolverPair::ApplyTangentImpulse()
    {
        if(!first || !second)
            return;

        vector<Vector2> impulseArr(solverPoints.size());
        for(int i = 0; i < solverPoints.size(); i++)
            impulseArr[i] = solverPoints[i].GetTangentImpulse(staticFriction, dynamicFriction, first, second);
        
        for(int i = 0; i < solverPoints.size(); i++)
        {
            first->ApplyImpulse(-impulseArr[i]);
            second->ApplyImpulse(impulseArr[i]);

            first->ApplyImpulseMoment(Cross(solverPoints[i].radiusVector.first, -impulseArr[i]));
            second->ApplyImpulseMoment(Cross(solverPoints[i].radiusVector.second, impulseArr[i]));
        }
    }
    void SolverPair::ApplyPositionCorrection()
    {
        if(!first || !second)
            return;

        vector<Vector2> offsetImpulseArr(solverPoints.size());
        for(int i = 0; i < solverPoints.size(); i++)
            offsetImpulseArr[i] = solverPoints[i].GetPositionCorrection(first, second);
        
        for(int i = 0; i < solverPoints.size(); i++)
        {
            first->ApplyPositionImpulse(-offsetImpulseArr[i]);
            second->ApplyPositionImpulse(offsetImpulseArr[i]);

            first->ApplyAngleImpulse(Cross(solverPoints[i].radiusVector.first, -offsetImpulseArr[i]));
            second->ApplyAngleImpulse(Cross(solverPoints[i].radiusVector.second, offsetImpulseArr[i]));
        }
    }
}