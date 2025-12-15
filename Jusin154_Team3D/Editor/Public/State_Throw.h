#pragma once
#include "Editor_Macro.h"
#include "State_Root.h"

NS_BEGIN(Editor)

class CState_Throw final : public CState_Root
{
public:
    typedef struct tagThrow : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_THROW_DESC;
private:
    CState_Throw();
    virtual ~CState_Throw() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_THROW_DESC* pDesc);

public:
    static CState_Throw* Create(STATE_THROW_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
