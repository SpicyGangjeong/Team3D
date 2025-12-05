#pragma once

#include "Editor_Define.h"
#include "MapElement.h"

NS_BEGIN(Editor)

class CMapElement_Interactable final : public CMapElement
{
public:
	typedef struct tagElement_Interactable_Desc : MAPOBJECT_LOD_DESC
	{
		_uint iInteractableID = {};
		_uint iSubKind = {};
	}ELEMENT_INTERACTABLE_DESC;

private:
	CMapElement_Interactable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Interactable(const CMapElement_Interactable& rhs);
	virtual ~CMapElement_Interactable() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	ELEMENT_INTERACTABLE_ID			m_eInteractableID = { ELEMENT_INTERACTABLE_ID::END};

	CRigidBody*						m_pRigidBody = { nullptr };
	PSX::PxRigidDynamic*			m_pActor = { nullptr };

#ifdef _DEBUG
	_bool m_bUseSelectColor = { true };

	_float m_fPower = {};
	_float m_fMass = {};
	_float3 m_vBoxSize = {};
#endif // _DEBUG


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CMapElement_Interactable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
	virtual HRESULT Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)override;
};

NS_END
