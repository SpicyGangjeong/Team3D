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
	HRESULT Load_MapObjects(const _char* pFilePath, const _wchar* pLayerTag);
	HRESULT Load_BuildingContainer(tinyxml2::XMLElement* Container, class CMapContainer** ppContainerObject, const _wchar* pLayerTag);
	HRESULT Load_StreetContainer(tinyxml2::XMLElement* Container, class CMapContainer** ppContainerObject, const _wchar* pLayerTag);
	HRESULT Load_MapRenderObjects(tinyxml2::XMLElement* Container, class CMapContainer* pContainerObject, const _wchar* pLayerTag);
	HRESULT Load_MapCollisionObjects(tinyxml2::XMLElement* Container, class CMapContainer* pContainerObject, const _wchar* pLayerTag);
	HRESULT Load_LightElements(const _char* pFileName, const _wchar* pLayerTag);
	HRESULT Load_InteractableElements(const _char* pFileName, const _wchar* pLayerTag);
	HRESULT Load_WaterElemet(const _char* pFileName);
	HRESULT Load_DoorElemet(const _char* pFileName, const _wchar* pLayerTag);
	HRESULT Load_ChestElemet(const _char* pFileName, const _wchar* pLayerTag);

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
#ifdef _DEBUG
	virtual void Describe_Entity();
#endif // _DEBUG

};

NS_END
