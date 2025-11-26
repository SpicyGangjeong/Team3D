#pragma once
#include "Base.h"

NS_BEGIN(STATEANIM)
enum ESTATE
{
    IDLE, IDLE_TURN_L, IDLE_TURN_R, IDLE_TURN_BWD,
    MOVE, 
    WALK, WALK_FWD, WALK_BWD,WALK_STOP,
    JOG,JOG_FWD, JOG_BWD,JOG_STOP,
    SPRINT,
    JUMP,JUMP_JOG,JUMP_SPRINT,
    LAND,
    DODGE,DODGE_BLINK, DODGE_LEFT, DODGE_RIGHT,
    COMBAT, LIGHT_ATTACK, SPELL, SKILL, SKILL2,
    ACCIO,DESCENDO,DIFFINDO,DEPULSO,LUMOS,DISILLUSION_ENTER,DISILLUSION_EXIT,
    HIT, HIT_FWD, HIT_BWD, HIT_BWD2, HIT_BWD3, HIT_BWD4, HIT_L, HIT_R,
    KNOCKDOWN, KNOCKDOWN_FWD, KNOCKDOWN_FWD_SPLT, KNOCKDOWN_FWD_SPLT_HOLD, KNOCKDOWN_BWD, KNOCKDOWN_BWD_SPLT, KNOCKDOWN_BWD_SPLT_HOLD,
    GETUP, GETUP_BWD, GETUP_FWD, GETUP_L, GETUP_L_FD, GETUP_L_FU, GETUP_R, GETUP_R_FD, GETUP_R_FU, GETUP_SLOUCH,
    DEAD, DEAD_FWD, DEAD_FWD2, DEAD_BWD, DEAD_BWD2, DEAD_L, DEAD_L2, DEAD_R, DEAD_R2,
    KNOCKBACK, KNOCKBACK2,
    TUMBLE, TUMBLE2,
    PETRIFICUSED_START,

    MAPHELP,
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
    typedef struct tagStateDesc {
        class CUnit* pOwner = { nullptr };
    }STATEDESC;

protected:
    CState();
    virtual ~CState() = default;

public:
    virtual void Enter()PURE;
    virtual HRESULT Update(_float fTimeDelta)PURE;
    virtual void Exit()PURE;

protected:
    class CUnit*              m_pOwner = { nullptr };
    CModel*             m_pModel = { nullptr };
    CFSM*               m_pFSM = { nullptr };
    CGameInstance*      m_pGameInstance = { nullptr };

protected:
    HRESULT Initialize(STATEDESC* pArg);

protected:
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
