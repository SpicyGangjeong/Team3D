#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_Land final : public CState_Root
{
public:
    typedef struct tagLand : public CState_Root::STATE_ROOT_DESC {

    }STATE_LAND_DESC;
private:
    CState_Land();
    virtual ~CState_Land() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    HRESULT Initialize(STATE_LAND_DESC* pDesc);

public:
    static CState_Land* Create(STATE_LAND_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
