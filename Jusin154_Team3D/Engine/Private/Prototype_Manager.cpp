#include "pch.h"
#include "Prototype_Manager.h"
#include "GameObject.h"

CPrototype_Manager::CPrototype_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : 
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CPrototype_Manager::Clear_Resource(_uint iLevelindex)
{
	for (auto& pBase : m_pPrototypes[iLevelindex]) {
		SAFE_RELEASE(pBase);
	} m_pPrototypes[iLevelindex].clear();

	for (auto& Pair : m_pAssets[iLevelindex]) {
		SAFE_RELEASE(Pair.second);
	} m_pAssets[iLevelindex].clear();

	return S_OK;
}

CComponent* CPrototype_Manager::Clone_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag, void* pArg, CGameObject* pOwner)
{
	CComponent* pAsset = { nullptr };

	pAsset = Find_Asset_Prototype(iTargetLevel, strPrototypeTag);

	if (pAsset == nullptr) {
		return nullptr;
	}

	return pAsset->Clone(pArg, pOwner);
}

CComponent* CPrototype_Manager::Find_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag)
{
	auto    iter = m_pAssets[iTargetLevel].find(strPrototypeTag);

	if (iter == m_pAssets[iTargetLevel].end()){
		return nullptr;
	}

	return iter->second;
}

HRESULT CPrototype_Manager::Add_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (pPrototype == nullptr){
		return E_FAIL;
	}

	if (Find_Asset_Prototype(iTargetLevel, strPrototypeTag) != nullptr)
	{
		SAFE_RELEASE(pPrototype);
		return S_OK;
	}

	m_pAssets[iTargetLevel].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}


HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	m_iLevelNumber = iNumLevels;

	m_pAssets		= new map<_wstring, CComponent*>[iNumLevels];
	m_pPrototypes	= new vector<CBase*>[iNumLevels];

	if (FAILED(Ready_StaticComponents())) {
		return E_FAIL;
	}
	if (FAILED(Ready_EngineAssets())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPrototype_Manager::Ready_StaticComponents()
{
	return S_OK;
}

HRESULT CPrototype_Manager::Ready_EngineAssets()
{
	if (FAILED(Add_Asset_Prototype(g_iStaticLevel, TEXT("FX_DEFERRED"), 
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_Deferred.hlsl"), 
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}
	return S_OK;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPrototype_Manager* pInstance = new CPrototype_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iLevelNumber; i++)
	{
		for (auto& Pair : m_pAssets[i]) {
			SAFE_RELEASE(Pair.second);
		} m_pAssets[i].clear();


		for (auto& pBase : m_pPrototypes[i]) {
			SAFE_RELEASE(pBase);
		} m_pPrototypes[i].clear();
	} 
	Safe_Delete_Array(m_pPrototypes);
	Safe_Delete_Array(m_pAssets);

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}
