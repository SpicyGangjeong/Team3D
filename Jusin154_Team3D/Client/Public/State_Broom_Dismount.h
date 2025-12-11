#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_Broom_Dismount final : public CState_Root
{
public:
    typedef struct tagBroomDismount : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_BROOM_DISMOUNT_DESC;
private:
    CState_Broom_Dismount();
    virtual ~CState_Broom_Dismount() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_BROOM_DISMOUNT_DESC* pDesc);

public:
    static CState_Broom_Dismount* Create(STATE_BROOM_DISMOUNT_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
