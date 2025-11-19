#pragma once
#include "Editor_Define.h"
#include "State.h"

class CState_Idle_Turn final : public CState
{
public:
    CState_Idle_Turn();
    virtual ~CState_Idle_Turn() = default;

public:
    virtual void Enter();
    virtual void Update(_float fTimeDelta);
    virtual void Exit();
    virtual _bool CheckExitState();

public:
    virtual void Free() override;
    virtual void Describe_Entity();
};

