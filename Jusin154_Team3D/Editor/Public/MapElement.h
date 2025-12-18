#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

class CMapElement abstract : public CGameObject
{
protected:
	CMapElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement(const CMapElement& rhs);
	virtual ~CMapElement() = default;

public:
	_uint Get_LodLevel() const{ return m_iMaxLodLevel; }
	const _wstring& Get_PrototypeTag(_uint iIndex);
	virtual _float3& Get_Rotation() { return m_vRotation; }

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_bool				m_bSelected = { false };
	_uint				m_iMaxLodLevel = {};
	_uint				m_iLodIndex = {};

	CShader*			m_pShaderCom = { nullptr };
	vector<CModel*>		m_pModelComs;
	vector<_wstring>	m_ModelPrototypeTags;
	

#ifdef _DEBUG
	_float3		m_vPosition = {};
	_float3		m_vRotation = {};
	_float3		m_vScale = {};
#endif // _DEBUG

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) PURE;
	virtual void Free() override;
	void Describe_Entity() override;
	virtual HRESULT Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root);

};

NS_END
