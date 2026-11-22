#include "Core/System/Buffer/CollisionPairBuffer.h"
using namespace Core::Collision;
using namespace std;

namespace Core
{
    void CollisionPairBuffer::ResetVisible()
    {
        for(auto& obj : _exitList)
            _visibleMap.erase(obj);

        for(auto& obj : _enterMap)
            _visibleMap[obj.first] = false;

        for(auto& obj : _stayMap)
            _visibleMap[obj.first] = false;

        _enterMap.clear();
        _stayMap.clear();
        _exitList.clear();
    }
    void CollisionPairBuffer::CopyPair(const CollisionPair& p)
    {
        if(!p)
            return;
        CollisionObj obj(p.colliders, p.bodies);
        
        auto it = _visibleMap.find(obj);
        if(it == _visibleMap.end())
        {
            _enterMap[obj] = p.infos;
            return;
        }
        _stayMap[obj] = p.infos;
        it->second = true;
    }
    void CollisionPairBuffer::MovePair(CollisionPair& p)
    {
        if(!p)
            return;
        CollisionObj obj(p.colliders, p.bodies);
        
        auto it = _visibleMap.find(obj);
        if(it == _visibleMap.end())
        {
            _enterMap[obj] = move(p.infos);
            return;
        }
        _stayMap[obj] = move(p.infos);
        it->second = true;
    }
    void CollisionPairBuffer::DiffExitPairs()
    {
        for(auto& p : _visibleMap)
            if(!p.second)
                _exitList.emplace_back(p.first);
    }
    size_t CollisionPairBuffer::EnterPairsCount() const
    {
        return _enterMap.size();
    }
    size_t CollisionPairBuffer::StayPairsCount() const
    {
        return _stayMap.size();
    }
    size_t CollisionPairBuffer::ExitPairsCount() const
    {
        return _exitList.size();
    }
    void CollisionPairBuffer::ForeachEnterPairs(const function<void(const CollisionObj&, const vector<CollisionInfo>&)>& func) const
    {
        for(auto& p : _enterMap)
            func(p.first, p.second);
    }
    void CollisionPairBuffer::ForeachStayPairs(const function<void(const CollisionObj&, const vector<CollisionInfo>&)>& func) const
    {
        for(auto& p : _stayMap)
            func(p.first, p.second);
    }
    void CollisionPairBuffer::ForeachExitPairs(const function<void(const CollisionObj&)>& func) const
    {
        for(auto& obj : _exitList)
            func(obj);
    }
}