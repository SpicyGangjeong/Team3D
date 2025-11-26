#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_Jump final : public CState_Root
{
public:
    typedef struct tagJump : public CState_Root::STATE_ROOT_DESC {

    }STATE_JUMP_DESC;
private:
    CState_Jump();
    virtual ~CState_Jump() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    HRESULT Initialize(STATE_JUMP_DESC* pDesc);

public:
    static CState_Jump* Create(STATE_JUMP_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
