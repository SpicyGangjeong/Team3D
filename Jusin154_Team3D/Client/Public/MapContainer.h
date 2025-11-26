#pragma once

#include "Client_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Client)

class CMapContainer abstract : public CContainerObject
{
public:
	typedef struct tagMapContainerDesc
	{
		_float3			vPosition;
		_float3			vScale;
		_float3			vRotation;
	}MAP_CONTAINER_DESC;

protected:
	CMapContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapContainer(const CMapContainer& rhs);
	~CMapContainer() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Ready_Components(void* pArg) override;

protected:
	_uint		m_iMaxLodLevel = {};

	vector<class CPartObject*>			m_ColiisonPartObjects;

public:
	virtual void Free() override;
};

NS_END
