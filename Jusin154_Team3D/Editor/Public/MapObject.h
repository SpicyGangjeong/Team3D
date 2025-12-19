#pragma once

#include "Editor_Define.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CMapObject abstract : public CPartObject
{
protected:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject(const CMapObject& Prototype);
	virtual ~CMapObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _wstring	Get_PrototypeTag(_uint iLodIndex = 0);
	virtual _float3&	Get_Rotation() { return m_vRotation; }

	_uint				Get_LodLevel();
	_uint				Get_iKeyIndex() const { return m_iKeyIndex; }

protected:
	_float4		m_vExtentPosition = {};

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();

#ifdef _DEBUG
protected:
	_bool		m_bSelected = {};

	_uint		m_iKeyIndex = {};
	_uint		m_iMaxLodLevel = {};

	_float3		m_vPosition = {};
	_float3		m_vRotation = {};
	_float3		m_vScale = {};
#endif // _DEBUG

private:
	HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
