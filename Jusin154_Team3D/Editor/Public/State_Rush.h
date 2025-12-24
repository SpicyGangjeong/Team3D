#pragma once
#include "Editor_Define.h"
#include "State_Root.h"

NS_BEGIN(Editor)

class CState_Rush final : public CState_Root
{
public:
    typedef struct tagRush : public CState_Root::STATE_ROOT_DESC {
        _bool* pCollisionPlayer = { nullptr };
    }STATE_RUSH_DESC;
public:
    CState_Rush();
    virtual ~CState_Rush() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    _bool* m_pRushPlayerHit = { nullptr };

private:
    HRESULT Initialize(STATE_RUSH_DESC* pDesc);

public:
    static CState_Rush* Create(STATE_RUSH_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
