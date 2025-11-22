#pragma once
#include "Editor_Define.h"
#include "State_Player.h"

class CState_Player_Dodge final : public CState_Player
{
public:
    typedef struct tagDodge : public CState_Player::STATE_PLAYER_DESC {

    }STATE_PLAYER_DODGE_DESC;
public:
    CState_Player_Dodge();
    virtual ~CState_Player_Dodge() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    HRESULT Initialize(STATE_PLAYER_DODGE_DESC* pDesc);

public:
    static CState_Player_Dodge* Create(STATE_PLAYER_DODGE_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};
