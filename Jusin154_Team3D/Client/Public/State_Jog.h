#pragma once
#include "Client_Define.h"
#include "State.h"


class CState_Jog final : public CState
{
public:
    CState_Jog();
    virtual ~CState_Jog() = default;

public:
    virtual void Enter();
    virtual void Update(_float fTimeDelta);
    virtual void Exit();
    virtual _bool CheckExitState();

public:
    virtual void Free() override;
    virtual void Describe_Entity();
};
