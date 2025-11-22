#pragma once
#include "Editor_Define.h"
#include "State.h"

NS_BEGIN(Editor)

class CState_Player abstract : public CState
{
protected:
    typedef struct tagStatePlayer : public CState::STATEDESC {
        function<HRESULT(_float fTimedelta)> funcExitCheck = { nullptr };
        function<void()> funcEnterEvent = { nullptr };
        function<void()> funcExitEvent = { nullptr };
    }STATE_PLAYER_DESC;

protected:
    CState_Player();
    virtual ~CState_Player() = default;

protected:
    virtual void Enter() override;
    virtual HRESULT Update(_float fTimeDelta) override;
    virtual void Exit() override;

protected:
    function<HRESULT(_float fTimedelta)> m_funcExitCheck = { nullptr };
    function<void()> m_funcEnterEvent = { nullptr };
    function<void()> m_funcExitEvent = { nullptr };

protected:
    HRESULT Initialize(STATE_PLAYER_DESC* pArg);

public:
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
