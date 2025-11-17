#pragma once
#include "Editor_Define.h"
#include "State.h"


class CState_Sprint final : public CState
{
public:
    CState_Sprint();
    virtual ~CState_Sprint() = default;

public:
    virtual void Enter();
    virtual void Update(_float fTimeDelta);
    virtual void Exit();
    virtual _bool CheckExitState();

public:
    virtual void Free() override;
    virtual void Describe_Entity();
};
