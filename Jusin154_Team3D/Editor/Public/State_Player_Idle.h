#pragma once
#include "Editor_Define.h"
#include "State_Player.h"

class CState_Player_Idle final : public CState_Player
{
public:
    typedef struct tagIdle : public CState_Player::STATE_PLAYER_DESC {

    }STATE_PLAYER_IDLE_DESC;
public:
    CState_Player_Idle();
    virtual ~CState_Player_Idle() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    HRESULT Initialize(STATE_PLAYER_IDLE_DESC* pDesc);

public:
    static CState_Player_Idle* Create(STATE_PLAYER_IDLE_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

