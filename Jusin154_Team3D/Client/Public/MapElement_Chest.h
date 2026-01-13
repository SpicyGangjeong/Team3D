#pragma once

#include "Client_Define.h"
#include "MapElement.h"

NS_BEGIN(Client)

class CMapElement_Chest final : public CMapElement
{
public:
	enum class CHEST_STATE { IDLE, FOUND, OPENED, END };

public:
	typedef struct tagElement_Chest_Desc : MAPELEMENT_DESC
	{
		_uint					iInteractableID = {};
		_uint					iSubKind = {};
		_float3					vBoxLocalPosition;
		_float3					vBoxSize;

		_uint					iMaxLodLevel_Lid{};
		vector<_wstring>		ModelPrototypeTags_Lid;
		_float3					vLid_Offset = {};
		_float					fRotaitionAngle = {};

		_float					fRimLightPow;
		_float					fRimLightStrength;
		_float4					vRimLightColor;
	}ELEMENT_CHEST_DESC;

private:
	CMapElement_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Chest(const CMapElement_Chest& rhs);
	virtual ~CMapElement_Chest() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void OnRayCollision(CGameObject* pCaster, _uint iCastedOrder, _float fDistance, _float3 vCastedWorldPos)override;
	_bool IsScannable(_fvector vPosition);
	_bool Get_Chest() { return m_bChest; }

private:
	ELEMENT_INTERACTABLE_ID			m_eInteractableID = { ELEMENT_INTERACTABLE_ID::END };
	CHEST_STATE						m_ePreState = {};
	CHEST_STATE						m_eCurState = {};
	CInfoInstance*					m_pInfoInstance = { nullptr };
	_uint							m_iShaderPass_Index = {};

	_int							m_iEntered = { 0 };
	_float2							m_vEnteringTimer = { 0.f, 1.f };
	_float							m_fEncounterDistance = { 8.f };
	_float							m_fTimeAcc = {};
	_float							m_fRimLightPower = { 0.7f };
	_float							m_fRimLightStrength = { 3.7f };
	_float							m_fCurRimLightStrength = { 0.f };
	_float4							m_vRimLightColor = { 0.71f, 0.39f, 0.02f, 1.f };

	_bool							m_bChest = { false };

	CTexture*						m_pNoiseTextureCom = { nullptr };
	CRigidBody_Dynamic*				m_pRigidBody = { nullptr };
	PSX::PxRigidDynamic*			m_pActor = { nullptr };

	class CMapElement_Chest_Lid*	m_pLid = { nullptr };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;
	HRESULT Ready_Lid(ELEMENT_CHEST_DESC* pArg);

	void Chage_State();

public:
	static CMapElement_Chest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
