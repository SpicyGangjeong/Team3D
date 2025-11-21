#pragma once
#include "Client_Define.h"
#include "State.h"

class CState_Move final : public CState
{
public:
    CState_Move();
    virtual ~CState_Move() = default;

public:
    virtual void Enter();
    virtual void Update(_float fTimeDelta);
    virtual void Exit();
    virtual _bool CheckExitState();
public:
    virtual void Free() override;
    virtual void Describe_Entity();
};
