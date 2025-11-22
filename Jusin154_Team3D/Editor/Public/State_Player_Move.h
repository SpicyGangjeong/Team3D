#pragma once
#include "Editor_Define.h"
#include "State_Player.h"

NS_BEGIN(Editor)

class CState_Player_Move final : public CState_Player
{
public:
    typedef struct tagMove : public CState_Player::STATE_PLAYER_DESC {

    }STATE_PLAYER_MOVE_DESC;
private:
    CState_Player_Move();
    virtual ~CState_Player_Move() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    HRESULT Initialize(STATE_PLAYER_MOVE_DESC* pDesc);

public:
    static CState_Player_Move* Create(STATE_PLAYER_MOVE_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
