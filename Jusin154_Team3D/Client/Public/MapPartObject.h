#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CMapPartObject abstract : public CPartObject
{
public:
	typedef struct tagMapObject_Desc {
		_bool					hasCollisionMesh{};
		_uint					iRenderType{};
		CTransform*				pParentTransform = { nullptr };
		_float3					vScale;
		_float3					vRotation;
		_float3					vPosition;
		_uint					iMaxLodLevel{};
		vector<_wstring>		ModelPrototypeTags;
		vector<_uint>*			pModelPathIndices = { nullptr };
	}MAPOBJECT_DESC;

protected:
	CMapPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapPartObject(const CMapPartObject& Prototype);
	virtual ~CMapPartObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float4		m_vExtentPosition = {};
	_uint		m_iMaxLodLevel = {};

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();

private:
	HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif
};

NS_END
