#pragma once
#include "Editor_Define.h"
#include "State_Root.h"

NS_BEGIN(Editor)

class CState_Dead final : public CState_Root
{
public:
    typedef struct tagLand : public CState_Root::STATE_ROOT_DESC {
        _float2 vDeadTimer = { 0.f, 2.f };
        function<void(_float)> funcLateUpdate = { nullptr };
    }STATE_DEAD_DESC;
private:
    CState_Dead();
    virtual ~CState_Dead() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    HRESULT Initialize(STATE_DEAD_DESC* pDesc);
    _float2     m_vDeadTimer = { 0.f, 2.f };
    function<void(_float)> m_funcLateUpdate = { nullptr };
public:
    static CState_Dead* Create(STATE_DEAD_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
