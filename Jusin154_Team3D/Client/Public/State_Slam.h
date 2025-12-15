#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_Slam final : public CState_Root
{
public:
    typedef struct tagSlam : public CState_Root::STATE_ROOT_DESC {
        function<void(_float)> funcPriorityUpdate = { nullptr };
        function<void(_float, _bool&)> funcLateUpdate = { nullptr };
    }STATE_SLAM_DESC;
private:
    CState_Slam();
    virtual ~CState_Slam() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float)> m_funcPriorityUpdate = { nullptr };
    function<void(_float, _bool&)> m_funcLateUpdate = { nullptr };
    _bool m_bPlayerHit = { false };

private:
    HRESULT Initialize(STATE_SLAM_DESC* pDesc);

public:
    static CState_Slam* Create(STATE_SLAM_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
