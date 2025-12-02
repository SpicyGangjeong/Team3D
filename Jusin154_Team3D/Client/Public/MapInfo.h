#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMapInfo final : public CBase
{
private:
	CMapInfo();
	~CMapInfo() = default;
public:
	void Update(_float fTimeDelta);
	void Change_Level();
	HRESULT Load_MapObjects(const _char* pFilePath);
	HRESULT Load_BuildingContainer(tinyxml2::XMLElement* Container, class CMapContainer** ppContainerObject);
	HRESULT Load_StreetContainer(tinyxml2::XMLElement* Container, class CMapContainer** ppContainerObject);
	HRESULT Load_MapRenderObjects(tinyxml2::XMLElement* Container, class CMapContainer* pContainerObject);
	HRESULT Load_MapCollisionObjects(tinyxml2::XMLElement* Container, class CMapContainer* pContainerObject);
	HRESULT Load_LightElements(const _char* pFileName);

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CInfoInstance*	m_pInfoInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);

public:
	static CMapInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
};

NS_END
