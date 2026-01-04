#pragma once

#include "Client_Define.h"
#include "Unit.h"
#include "NPC_Ollivander.h"

NS_BEGIN(Client)

class CRandomNpc final : public CUnit
{
public:
	typedef struct tagNpcInitDesc {
		_float4 vPos;
		_float4 vRotQ;
		_int iIndex;
	}NPCDESC;
protected:
	CRandomNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRandomNpc(const CRandomNpc& Prototype);
	virtual ~CRandomNpc() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual void OnRayCollision(CGameObject* pCaster, _uint iCastedOrder, _float fDistance, _float3 vCastedWorldPos)override;
	
	virtual _wstring Get_Name() override;
protected:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	CUnit* m_pPlayerAllyUnit = { nullptr };
	CRandomNpc* m_pNPC_Ollivander = { nullptr };

	/*class CNPCInteraction* m_pNPCInteraction = { nullptr };*/

	CNPCStat* m_pNpcStat = { nullptr };
	_int									m_iEntered = { 0 };
	_float2									m_vEnteringTimer = { 0.f, 1.f };
	_float									m_fEncounterDistance = { 8.f };
	_int									m_iIndex = {};
protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources() override;

public:
	static CRandomNpc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr);
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG
};

NS_END
