#pragma once
#include "Editor_Define.h"
#include "State.h"

class CState_Jump final : public CState
{
public:
    CState_Jump();
    virtual ~CState_Jump() = default;

public:
    virtual void Enter();
    virtual void Update(_float fTimeDelta);
    virtual void Exit();
    virtual _bool CheckExitState();

public:
    virtual void Free() override;
    virtual void Describe_Entity();
};
