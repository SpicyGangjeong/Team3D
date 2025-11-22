#pragma once
#include "Base.h"

NS_BEGIN(STATEANIM)
enum ESTATE
{
    IDLE, IDLE_TURN_L, IDLE_TURN_R, IDLE_TURN_BWD,
    MOVE, 
    WALK, WALK_FWD, WALK_BWD,WALK_STOP,
    JOG,JOG_FWD, JOG_STOP,
    SPRINT,
    JUMP,JUMP_JOG,JUMP_SPRINT,
    LAND,
    DODGE,
    COMBAT, LIGHT_ATTACK, CAST, SKILL, SKILL2,
    END
};
NS_END

NS_BEGIN(Engine)
class CUnit;
class CModel;
class CFSM;
class CGameInstance;
class ENGINE_DLL CState abstract : public CBase
{
protected:
    CState();
    virtual ~CState() = default;

public:
    virtual void Enter();
    virtual void Update(_float fTimeDelta);
    virtual void Exit();
    virtual _bool CheckExitState();
    void Set_Owner(class CGameObject* pOwner);
    virtual void Set_Component();

    void Set_State(_uint State) { m_eState = State; }
    _uint Get_State() { return m_eState; }
    void Set_Parent(CState* State) { m_pParent = State; }
    CState* Get_Parent() { return m_pParent; }
protected:
    CUnit*              m_pOwner = { nullptr };
    CModel*             m_pModel = { nullptr };
    CFSM*               m_pFSM = { nullptr };
    CState*             m_pParent = { nullptr };
    CGameInstance*      m_pGameInstance = { nullptr };
    _uint               m_eState = {};



public:
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
