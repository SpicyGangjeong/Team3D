#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_Swipe final : public CState_Root
{
public:
    typedef struct tagSwipe : public CState_Root::STATE_ROOT_DESC {
        function<void(_float)> funcPriorityUpdate = { nullptr };
        function<void(_float, _bool&)> funcLateUpdate = { nullptr };
    }STATE_SWIPE_DESC;
private:
    CState_Swipe();
    virtual ~CState_Swipe() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float)> m_funcPriorityUpdate = { nullptr };
    function<void(_float, _bool&)> m_funcLateUpdate = { nullptr };
    _bool m_bPlayerHit = { false };

private:
    HRESULT Initialize(STATE_SWIPE_DESC* pDesc);

public:
    static CState_Swipe* Create(STATE_SWIPE_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
