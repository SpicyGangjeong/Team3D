#pragma once
#include "Editor_Define.h"
#include "State.h"

NS_BEGIN(Editor)

class CState_Root abstract : public CState
{
protected:
    typedef struct tagStateRoot : public CState::STATEDESC {
        function<HRESULT(_float fTimedelta)> funcExitCheck = { nullptr };
        function<void()> funcEnterEvent = { nullptr };
        function<void()> funcExitEvent = { nullptr };
    }STATE_ROOT_DESC;

protected:
    CState_Root();
    virtual ~CState_Root() = default;

protected:
    virtual void Enter() override;
    virtual HRESULT Update(_float fTimeDelta) override;
    virtual void Exit() override;

protected:
    function<HRESULT(_float fTimedelta)> m_funcExitCheck = { nullptr };
    function<void()> m_funcEnterEvent = { nullptr };
    function<void()> m_funcExitEvent = { nullptr };

protected:
    HRESULT Initialize(STATE_ROOT_DESC* pArg);

protected:
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
