#include "pch.h"
#include "GameObject_Manager.h"

#include "Layer.h"
#include "GameInstance.h"

CGameObject_Manager::CGameObject_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CGameObject_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pLayers = new map<const _wstring, CLayer*>[iNumLevels];
	
	return S_OK;
}

HRESULT CGameObject_Manager::Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, class CGameObject* pInstance)
{
	CLayer* pLayer = Get_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pInstance);

		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else {
		pLayer->Add_GameObject(pInstance);
	}

	return S_OK;
}

void CGameObject_Manager::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
#ifndef gimch
	Describe_Entity();
#endif // !gimch
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_PriorityUpdate"));
#endif // _DEBUG
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Update(fTimeDelta);
	}
#ifdef _DEBUG
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_PriorityUpdate"));
#endif // _DEBUG
}

void CGameObject_Manager::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_Update"));
#endif // _DEBUG
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}
#ifdef _DEBUG
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_Update"));
#endif // _DEBUG
}

void CGameObject_Manager::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_LateUpdate"));
#endif // _DEBUG
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}
#ifdef _DEBUG
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_LateUpdate"));
#endif // _DEBUG
}

void CGameObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Pair.second->Clear_Layer();
		SAFE_RELEASE(Pair.second);
	}

	m_pLayers[iLevelIndex].clear();
}

void CGameObject_Manager::Clear_DeadObj()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Clear_DeadObj();
		}
	}
}

CLayer* CGameObject_Manager::Get_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	assert(iLayerLevelIndex < m_iNumLevels);

	auto	iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLayerLevelIndex].end()) {
		return nullptr;
	}

	return iter->second;
}

CLayer* CGameObject_Manager::Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	if (iLayerLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLayerLevelIndex].end()){
		return nullptr;
	}

	return iter->second;
}

CGameObject_Manager* CGameObject_Manager::Create(_uint iNumLevels)
{
	CGameObject_Manager* pInstance = new CGameObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CGameObject_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGameObject_Manager::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pGameInstance);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i]){
			Pair.second->Clear_Layer();
			SAFE_RELEASE(Pair.second);
		}

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);
}

#ifdef _DEBUG

void CGameObject_Manager::Describe_Entity()
{
	GUI::Begin("SYSTEM");
	if (GUI::CollapsingHeader("Object_Manager")) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
		GUI::Text("Levels : "); GUI::SameLine();
		if (GUI::BeginTabBar("Levels")) {
			for (_uint i = 0; i < m_iNumLevels; ++i) {

				if (GUI::BeginTabItem(to_string(i).c_str(), nullptr)) { // Insert Here Level Names
					GUI::BeginChild("Object_Lists");
					if (GUI::BeginTabBar("Layers")) {
						string strlayerName;
						for (pair< const _wstring, CLayer*>& pairLayer : m_pLayers[i]) {
							size_t pos = pairLayer.first.find(TEXT("Layer_"));
							strlayerName = "NOT_STARTED_WITH_Layer_";
							if (pos != wstring::npos) {
								strlayerName = CMyTools::ToString(pairLayer.first.c_str() + pos + wcslen(TEXT("Layer_"))).c_str();
							}
							if (GUI::BeginTabItem(strlayerName.c_str(), nullptr)) {
								if (GUI::BeginChild("Items", ImVec2(0, 0), 0, window_flags)) {
									pairLayer.second->Describe_Entity();
									GUI::EndChild();
								}
								GUI::EndTabItem();
							}
						}
						GUI::EndTabBar();
					}
					GUI::EndChild();
					GUI::EndTabItem();
				}
			}
			GUI::EndTabBar();
		}
	}
	GUI::End();
}


#endif // _DEBUG

