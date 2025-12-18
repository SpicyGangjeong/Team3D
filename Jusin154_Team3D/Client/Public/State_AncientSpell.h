#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_AncientSpell final : public CState_Root
{
public:
    typedef struct tagAncientSpell : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_ANCIENTSPELL_DESC;
private:
    CState_AncientSpell();
    virtual ~CState_AncientSpell() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_ANCIENTSPELL_DESC* pDesc);

public:
    static CState_AncientSpell* Create(STATE_ANCIENTSPELL_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
