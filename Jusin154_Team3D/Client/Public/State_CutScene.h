#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_CutScene final : public CState_Root
{
public:
    typedef struct tagCutScene : public CState_Root::STATE_ROOT_DESC {

    }STATE_CUTSCENE_DESC;
private:
    CState_CutScene();
    virtual ~CState_CutScene() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    HRESULT Initialize(STATE_CUTSCENE_DESC* pDesc);

public:
    static CState_CutScene* Create(STATE_CUTSCENE_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
