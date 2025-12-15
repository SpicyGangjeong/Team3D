#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CTroll_State_Rush final : public CState_Root
{
public:
    typedef struct tagTrollRush : public CState_Root::STATE_ROOT_DESC {
        _bool* pCollisionPlayer = { nullptr };
    }TROLL_STATE_RUSH_DESC;
public:
    CTroll_State_Rush();
    virtual ~CTroll_State_Rush() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    _bool* m_pRushPlayerHit = { nullptr };

private:
    HRESULT Initialize(TROLL_STATE_RUSH_DESC* pDesc);

public:
    static CTroll_State_Rush* Create(TROLL_STATE_RUSH_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
