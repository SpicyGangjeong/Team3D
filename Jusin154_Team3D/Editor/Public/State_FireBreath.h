#pragma once
#include "Editor_Define.h"
#include "State_Root.h"

NS_BEGIN(Editor)

class CState_FireBreath final : public CState_Root
{
public:
    typedef struct tagFireBreath : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_FIREBREATH_DESC;
private:
    CState_FireBreath();
    virtual ~CState_FireBreath() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_FIREBREATH_DESC* pDesc);

public:
    static CState_FireBreath* Create(STATE_FIREBREATH_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
