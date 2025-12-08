#pragma once
#include "Editor_Define.h"
#include "State_Root.h"

NS_BEGIN(Editor)

class CState_Swing final : public CState_Root
{
public:
    typedef struct tagSwing : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_SWING_DESC;
private:
    CState_Swing();
    virtual ~CState_Swing() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_SWING_DESC* pDesc);

public:
    static CState_Swing* Create(STATE_SWING_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
