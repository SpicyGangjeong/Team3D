#pragma once
#include "Editor_Define.h"
#include "State_Root.h"

NS_BEGIN(Editor)

class CState_Ground final : public CState_Root
{
public:
    typedef struct tagGround : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_GROUND_DESC;
private:
    CState_Ground();
    virtual ~CState_Ground() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_GROUND_DESC* pDesc);

public:
    static CState_Ground* Create(STATE_GROUND_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
