#pragma once
#include "Base.h"

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

protected:
    CUnit*              m_pOwner = { nullptr };
    CModel*             m_pModel = { nullptr };
    CFSM*               m_pFSM = { nullptr };
    CGameInstance*      m_pGameInstance = { nullptr };

public:
    virtual void Free() override;
    virtual void Describe_Entity();
};

NS_END
