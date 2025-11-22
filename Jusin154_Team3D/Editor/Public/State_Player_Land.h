#pragma once
#include "Editor_Define.h"
#include "State_Player.h"

NS_BEGIN(Editor)

class CState_Player_Land final : public CState_Player
{
public:
    typedef struct tagLand : public CState_Player::STATE_PLAYER_DESC {

    }STATE_PLAYER_LAND_DESC;
private:
    CState_Player_Land();
    virtual ~CState_Player_Land() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    HRESULT Initialize(STATE_PLAYER_LAND_DESC* pDesc);

public:
    static CState_Player_Land* Create(STATE_PLAYER_LAND_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
