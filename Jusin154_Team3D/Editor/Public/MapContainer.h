#pragma once

#include "Editor_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Editor)

class CMapContainer abstract : public CContainerObject
{
public:
	typedef struct tagMapContainerDesc
	{
		_string			strName;
		_float3			vPosition;
		_float3			vScale;
		_float3			vRotation;
	}MAP_CONTAINER_DESC;
public:
	CMapContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CMapContainer(const CMapContainer& rhs);
	~CMapContainer() = default;
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _string& Get_Name() { return m_strName; }
	virtual HRESULT Save_PartObjects(tinyxml2::XMLDocument& Doc, tinyxml2::XMLElement* Container);

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Ready_Components(void* pArg) override;

protected:
	_string		m_strName = {};
	_bool		m_bSelected = {};

	_uint		m_iMaxLodLevel = {};

	_float3		m_vPosition = {};
	_float3		m_vRotation = {};
	_float3		m_vScale = {};

public:
	virtual void Free() override;
};

NS_END