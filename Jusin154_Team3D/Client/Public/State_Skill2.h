#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_Skill2 final : public CState_Root
{
public:
    typedef struct tagSkill2 : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_SKILL2_DESC;
private:
    CState_Skill2();
    virtual ~CState_Skill2() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_SKILL2_DESC* pDesc);

public:
    static CState_Skill2* Create(STATE_SKILL2_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
