#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CBroom_State_Hover final : public CState_Root
{
public:
    typedef struct tagHover : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_HOVER_DESC;
private:
    CBroom_State_Hover();
    virtual ~CBroom_State_Hover() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_HOVER_DESC* pDesc);

public:
    static CBroom_State_Hover* Create(STATE_HOVER_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
