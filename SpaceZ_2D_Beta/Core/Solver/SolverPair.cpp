#include "Core/Solver/SolverPair.h"
#include "Core/Math/Math.h"
using namespace Core::Collision;
using namespace Core::Math;
using namespace std;

namespace Core::Solver
{
    SolverPair::SolverPair(const vector<CollisionInfo>& infos, SolverRigidbody* f, SolverRigidbody* s)
    {
        if(!f || !s)
            return;
            
        first = f;
        second = s;

        for(auto& info : infos)
        {
            auto point = SolverCollisionPoint(info, f, s);

            _points.emplace_back(point);
        }
        _deltaImpulseList.resize(_points.size());
    }
    SolverPair::operator bool() const
    {
        return first != nullptr && second != nullptr && !_points.empty();
    }
    void SolverPair::SetContact(const pair<float, float>& sf, const pair<float, float>& df, const pair<float, float>& e)
    {
        staticFriction = Min(sf.first, sf.second);
        dynamicFriction = Min(df.first, df.second);
        elasticity = Clamp01(Min(e.first, e.second));
    }
    void SolverPair::Precompute()
    {
        if(!first || !second)
            return;
        for(auto& point : _points)
            point.SetTargetRelatVelocity(elasticity, first, second);
    }

    void SolverPair::ApplyNormalImpulse()
    {
        if(!first || !second)
            return;
        for(int i = 0; i < _points.size(); i++)
            _deltaImpulseList[i] = _points[i].GetDeltaNormalImpulse(first, second);

        for(int i = 0; i < _points.size(); i++)
        {
            first->ApplyImpulse(-_deltaImpulseList[i]);
            second->ApplyImpulse(_deltaImpulseList[i]);

            first->ApplyImpulseMoment(Cross(_points[i].radiusVector.first, -_deltaImpulseList[i]));
            second->ApplyImpulseMoment(Cross(_points[i].radiusVector.second, _deltaImpulseList[i]));
        }
    }
    void SolverPair::ApplyTangentImpulse()
    {
        if(!first || !second)
            return;
        for(int i = 0; i < _points.size(); i++)
            _deltaImpulseList[i] = _points[i].GetDeltaTangentImpulse(staticFriction, dynamicFriction, first, second);

        for(int i = 0; i < _points.size(); i++)
        {
            first->ApplyImpulse(-_deltaImpulseList[i]);
            second->ApplyImpulse(_deltaImpulseList[i]);

            first->ApplyImpulseMoment(Cross(_points[i].radiusVector.first, -_deltaImpulseList[i]));
            second->ApplyImpulseMoment(Cross(_points[i].radiusVector.second, _deltaImpulseList[i]));
        }
    }
    void SolverPair::ApplyPositionCorrection()
    {
        if(!first || !second)
            return;
        for(int i = 0; i < _points.size(); i++)
            _deltaImpulseList[i] = _points[i].GetPositionCorrection(first, second);

        for(int i = 0; i < _points.size(); i++)
        {
            first->ApplyPositionImpulse(-_deltaImpulseList[i]);
            second->ApplyPositionImpulse(_deltaImpulseList[i]);

            first->ApplyAngleImpulse(Cross(_points[i].radiusVector.first, -_deltaImpulseList[i]));
            second->ApplyAngleImpulse(Cross(_points[i].radiusVector.second, _deltaImpulseList[i]));
        }
    }
}