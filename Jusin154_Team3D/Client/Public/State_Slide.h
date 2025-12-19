#pragma once
#include "Client_Define.h"
#include "State_Root.h"

NS_BEGIN(Client)

class CState_Slide final : public CState_Root
{
public:
    typedef struct tagSlide : public CState_Root::STATE_ROOT_DESC {
        function<void(_float fTimeDelta)> funcPriorityUpdate = { nullptr };
        function<void(_float fTimeDelta)> funcLateUpdate = { nullptr };
    }STATE_SLIDE_DESC;
public:
    CState_Slide();
    virtual ~CState_Slide() = default;

public:
    virtual void Enter();
    virtual HRESULT Update(_float fTimeDelta);
    virtual void Exit();

private:
    function<void(_float fTimeDelta)> m_funcPriorityUpdate = { nullptr };
    function<void(_float fTimeDelta)> m_funcLateUpdate = { nullptr };

private:
    HRESULT Initialize(STATE_SLIDE_DESC* pDesc);

public:
    static CState_Slide* Create(STATE_SLIDE_DESC* pDesc);
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
