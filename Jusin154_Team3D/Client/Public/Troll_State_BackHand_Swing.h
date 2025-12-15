#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CTroll_State_BackHand_Swing final : public CState_Root
{
public:
    typedef struct tagTrollBackHandSwing : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float, _bool&)> funcLateUpdate = { nullptr };
    }TROLL_STATE_BACK_HAND_SWING_DESC;
public:
    CTroll_State_BackHand_Swing();
    virtual ~CTroll_State_BackHand_Swing() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float, _bool&)> m_funcLateUpdate = { nullptr };
    _bool m_bPlayerHit = false;

private:
    HRESULT Initialize(TROLL_STATE_BACK_HAND_SWING_DESC* pDesc);

public:
    static CTroll_State_BackHand_Swing* Create(TROLL_STATE_BACK_HAND_SWING_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
