#pragma once

#include "Editor_Define.h"
#include "MapElement.h"

NS_BEGIN(Editor)

class CMapElement_Chest final : public CMapElement
{
public:
	enum class CHEST_STATE { IDLE, FOUND, OPENED, END };

public:
	typedef struct tagElement_Chest_Desc : MAPOBJECT_LOD_DESC
	{
		_uint					iInteractableID = {};
		_uint					iSubKind = {};
		_float3					vBoxLocalPosition;
		_float3					vBoxSize;

		_uint					iMaxLodLevel_Lid{};
		vector<_wstring>		ModelPrototypeTags_Lid;
		_float3					vLid_Offset = {};
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


private:
	ELEMENT_INTERACTABLE_ID			m_eInteractableID = { ELEMENT_INTERACTABLE_ID::END };
	CHEST_STATE						m_ePreState = {};
	CHEST_STATE						m_eCurState = {};

	_uint							m_iShaderPass_Index = {};

	_float							m_fTimeAcc = {};
	_float							m_fRimLightPower = { 0.7f };
	_float							m_fRimLightStrength = { 3.7f };
	_float							m_fCurRimLightStrength = { 0.f };
	_float4							m_vRimLightColor = {0.71f, 0.39f, 0.02f, 1.f};

	class CMapElement_Chest_Lid*	m_pLid = { nullptr };
	CRigidBody*						m_pRigidBody = { nullptr };
	PSX::PxRigidDynamic*			m_pActor = { nullptr };

	CTexture*						m_pNoiseTextureCom = { nullptr };
#ifdef _DEBUG
	_bool		m_bUseSelectColor = { true };

	_float		m_fPower = {};
	_float		m_fMass = {};
	_float3		m_vBoxSize = {};
#endif // _DEBUG


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
	void Describe_Entity() override;
	virtual HRESULT Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)override;
};

NS_END
