#pragma once

#include "Client_Define.h"
#include "MapContainer.h"

NS_BEGIN(Client)

class CStreetContainer final : public CMapContainer // NON OCCLUSION CONTAINER
{
private:
	CStreetContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStreetContainer(const CStreetContainer& rhs);
	~CStreetContainer() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;

private:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Ready_Components(void* pArg) override;
	virtual HRESULT		Bind_ShaderResources()override;

public:
	static CStreetContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner = nullptr)override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity()override;
#endif // _DEBUG

};

NS_END
