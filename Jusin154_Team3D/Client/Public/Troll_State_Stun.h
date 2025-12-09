#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CTroll_State_Stun final : public CState_Root
{
public:
    typedef struct tagTrollStun : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }TROLL_STATE_STUN;
public:
    CTroll_State_Stun();
    virtual ~CTroll_State_Stun() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(TROLL_STATE_STUN* pDesc);

public:
    static CTroll_State_Stun* Create(TROLL_STATE_STUN* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
