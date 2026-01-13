#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CNPC_EleazarFig final : public CUnit
{
public:
	typedef struct tagNpcInitDesc {
		_float4 vPos;
		_float4 vRotQ;
	}NPCDESC;
protected:
	CNPC_EleazarFig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_EleazarFig(const CNPC_EleazarFig& Prototype);
	virtual ~CNPC_EleazarFig() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual void OnRayCollision(CGameObject* pCaster, _uint iCastedOrder, _float fDistance, _float3 vCastedWorldPos)override;
	void Set_Pos(_vector vPos);
protected:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	class CCallBack_NonPlayable_Behavior* m_pCallBack_Behavior = { nullptr };
	class CCallBack_NonPlayable_HitReport* m_pCallBack_HitReport = { nullptr };
	CUnit* m_pPlayerAllyUnit = { nullptr };


	_int				m_iEntered = { 0 };
	_float2				m_vEnteringTimer = { 0.f, 1.f };
	_float				m_fEncounterDistance = { 8.f };
protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources() override;

public:
	static CNPC_EleazarFig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr);
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG
};

NS_END
