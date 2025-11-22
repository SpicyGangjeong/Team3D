#pragma once
#include "Editor_Define.h"
#include "State.h"

class CState_Root final : public CState
{
public:
    CState_Root();
    virtual ~CState_Root() = default;

public:
    virtual void Enter();
    virtual void Update(_float fTimeDelta);
    virtual void Exit();
    virtual _bool CheckExitState();
public:
    virtual void Free() override;
    virtual void Describe_Entity();
};
