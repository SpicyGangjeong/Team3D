#pragma once

#include "Editor_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Editor)

class CMapContainer abstract : public CContainerObject
{
public:
	CMapContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
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

public:
	virtual void Free() override;
};

NS_END