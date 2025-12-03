#include "pch.h"
#include "MapInfo.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "BuildingContainer.h"
#include "MapObject_Collision.h"
#include "MapElement_Light.h"
#include "Layer.h"

CMapInfo::CMapInfo()
{
}

void CMapInfo::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

	static _bool s_bConverted = { false };
	static _bool s_bReadyToCreate = { false };

	if (true != s_bConverted || true != s_bReadyToCreate) {
		CLayer* pLayer = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_BACKGROUND);
		if (nullptr == pLayer) {
			return;
		}
		const list<CGameObject*>* pObjects = pLayer->Get_Objects();

		if (false == s_bReadyToCreate) {
				s_bReadyToCreate = true;
				list<CGameObject*>::const_iterator iter = pObjects->begin();
				for (; iter != pObjects->end(); ++iter) {
					CMapContainer* pContainer = dynamic_cast<CMapContainer*>(*iter);
					if (nullptr != pContainer) {
						pContainer->ReadyForPhysX();
					}
			}
		}
		else if (false == s_bConverted) {
				s_bConverted = true;
				list<CGameObject*>::const_iterator iter = pObjects->begin();
				for (; iter != pObjects->end(); ++iter) {
					CMapContainer* pContainer = dynamic_cast<CMapContainer*>(*iter);
					if (nullptr != pContainer) {
						pContainer->ConvertToPhysX();
					}
				}
		}
	}
}

void CMapInfo::Change_Level()
{
}


HRESULT CMapInfo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pInfoInstance = CInfoInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContex;

	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pInfoInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);


	return S_OK;
}

CMapInfo* CMapInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	CMapInfo* pInstance = new CMapInfo();

	if (FAILED(pInstance->Initialize(pDevice, pContex))) {
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapInfo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);

}
#ifdef _DEBUG

void CMapInfo::Describe_Entity()
{
	CLayer* pLayer = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_BACKGROUND);
	if (nullptr == pLayer) {
		return;
	}
	static _bool s_bConverted = { false };
	static _bool s_bReadyToCreate = { false };

	const list<CGameObject*>* pObjects = pLayer->Get_Objects();

	GUI::Begin("MapInfoDesc");
	GUI::SetNextItemWidth(80.f);
	GUI::BeginChild("CollisonPartObjects");
	GUI::SetNextItemWidth(80.f);
	if (false == s_bReadyToCreate) {
		if (GUI::Button("Ready_StaticMeshesForPhyX_ALL")) {
			s_bReadyToCreate = true;
			list<CGameObject*>::const_iterator iter = pObjects->begin();
			for (; iter != pObjects->end(); ++iter) {
				CMapContainer* pContainer = dynamic_cast<CMapContainer*>(*iter);
				if (nullptr != pContainer) {
					pContainer->ReadyForPhysX();
				}
			}
		}
	}
	else if (false == s_bConverted) {
		if (GUI::Button("Convert_StaticMeshesForPhyX_ALL")) {
			s_bConverted = true;
			list<CGameObject*>::const_iterator iter = pObjects->begin();
			for (; iter != pObjects->end(); ++iter) {
				CMapContainer* pContainer = dynamic_cast<CMapContainer*>(*iter);
				if (nullptr != pContainer) {
					pContainer->ConvertToPhysX();
				}
			}
		}
	}
	GUI::EndChild();
	GUI::End();
}

#endif // _DEBUG
