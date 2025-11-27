#pragma once
#include "Base.h"

NS_BEGIN(STATEANIM)
enum ESTATE
{
    IDLE, IDLE_TURN_L, IDLE_TURN_R, IDLE_TURN_BWD,
    MOVE, 
    WALK, WALK_FWD, WALK_BWD,WALK_STOP,
    JOG,JOG_FWD,JOG_RIGHT,JOG_LEFT, JOG_BWD,JOG_STOP,
    SPRINT,
    JUMP,JUMP_JOG,JUMP_SPRINT,
    LAND,
    DODGE,DODGE_BLINK,
    COMBAT, LIGHT_ATTACK, SPELL, SKILL, SKILL2,
    ACCIO,DESCENDO,DIFFINDO,DEPULSO,LUMOS,DISILLUSION_ENTER,DISILLUSION_EXIT, ANCIENT_THROW,
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
