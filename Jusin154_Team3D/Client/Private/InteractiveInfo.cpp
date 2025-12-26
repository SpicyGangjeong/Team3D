#include "pch.h"
#include "InteractiveInfo.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "MapElement_Interactable.h"
#include "Monster.h"
#include "Layer.h"
#include "Player.h"

CInteractiveInfo::CInteractiveInfo()
{
}

void CInteractiveInfo::Update(_float fTimeDelta)
{
	Refresh_LockOnTarget();

	auto iter = m_PoolingActiveInteractive.begin();

	for (; iter != m_PoolingActiveInteractive.end();)
	{
		if (false == (*iter)->Get_Visible())
		{
			m_PoolingInteractive.push_back(*iter);
			iter = m_PoolingActiveInteractive.erase(iter);
		}
		else
			++iter;
	}
}
void CInteractiveInfo::Change_Level()
{
	SAFE_RELEASE(m_pLockOnInteractive);
	for (CMapElement_Interactable* pInteractive : m_ActiveInteractive) {
		SAFE_RELEASE(pInteractive);
	} m_ActiveInteractive.clear();

	for (CMapElement_Interactable* pInteractive : m_PoolingInteractive) {
		SAFE_RELEASE(pInteractive);
	} m_PoolingInteractive.clear();

	for (CMapElement_Interactable* pInteractive : m_PoolingActiveInteractive) {
		SAFE_RELEASE(pInteractive);
	} m_PoolingActiveInteractive.clear();
}

HRESULT CInteractiveInfo::Regist_ActiveInteractive(CMapElement_Interactable* pUnit)
{
	for (CMapElement_Interactable* pInteractive : m_ActiveInteractive) {
		if (pInteractive == pUnit) {
			return E_ACCESSDENIED;
		}
	} m_ActiveInteractive.push_back(pUnit);
	SAFE_ADDREF(pUnit);
	return S_OK;
}

HRESULT CInteractiveInfo::Deregist_ActiveInteractive(CMapElement_Interactable* pUnit)
{
	for (list<CMapElement_Interactable*>::iterator iter = m_ActiveInteractive.begin(); iter != m_ActiveInteractive.end();) {
		if (*iter == pUnit) {
			SAFE_RELEASE(*iter);
			iter = m_ActiveInteractive.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	return E_FAIL;
}

CMapElement_Interactable* CInteractiveInfo::Get_LockOnUnit()
{
	return m_pLockOnInteractive;
}

HRESULT CInteractiveInfo::Refresh_LockOnTarget()
{
	SAFE_RELEASE(m_pLockOnInteractive);
	_vector vCameraLook = m_pGameInstance->Get_CameraLook();
	_vector vCameraPos = m_pGameInstance->Get_CamXMPosition();
	_float fMaxDot = { 0.f };

	_vector vInteractablePos;
	_vector vToMonsterDir;
	_bool bAim = false;
	_float fFovy = 0.f;
	if (m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER) != nullptr)
	{
		bAim = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Get_Aim();
	}
	if (true == bAim)
	{
		fFovy = max(m_pGameInstance->Get_CameraFov() - XMConvertToRadians(50.f), XMConvertToRadians(5.f));
	}
	else {
		fFovy = max(m_pGameInstance->Get_CameraFov() - XMConvertToRadians(30.f), XMConvertToRadians(20.f));
	}

	{ // 뷰프러스텀 순회해서 가장 중앙에 근접한 물체 찾기
		for (list<CMapElement_Interactable*>::iterator iter = m_ActiveInteractive.begin(); iter != m_ActiveInteractive.end(); ++iter) {

			// (카메라의 룩)과 (카메라 -> 몬스터 방향 벡터)를 내적해서 가장 큰 크기가 나온 몬스타가 락온 대상
			CMapElement_Interactable* pInteractive = (*iter);

			vInteractablePos = pInteractive->Get_LockOnPos();
			vToMonsterDir = vInteractablePos - vCameraPos;
			if (40.f <= XMVectorGetX(XMVector4Length(vToMonsterDir))) {
				continue;
			}
			_float fDotResult = CMyTools::DirectionCompare(vCameraLook, vToMonsterDir);
			if (fDotResult <= cosf(fFovy)) {
				continue;
			}

			if (nullptr == m_pLockOnInteractive) {
				m_pLockOnInteractive = pInteractive;
				fMaxDot = fDotResult;
				continue;
			}

			if (fMaxDot < fDotResult) {
				m_pLockOnInteractive = pInteractive;
				fMaxDot = fDotResult;
			}
		}
	}
	if (nullptr != m_pLockOnInteractive) {
		SAFE_ADDREF(m_pLockOnInteractive);
	}
	return S_OK;
}

HRESULT CInteractiveInfo::Ready_PoolingInteractive()
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/Interactable/E_INTER_EffectObejct.xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("Element_Interactable");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	for (auto* Object = root->FirstChildElement("Object"); Object; Object = Object->NextSiblingElement("Object"))
	{
		CMapElement_Interactable::ELEMENT_INTERACTABLE_DESC Desc = {};

		/* Model Prototypes */
		Desc.isPooled = true;
		Object->QueryUnsignedAttribute("Lod_Level", &Desc.iMaxLodLevel);
		Object->QueryUnsignedAttribute("ID", &Desc.iInteractableID);

		string strTag = {};
		for (auto* PrototypeTag = Object->FirstChildElement("PrototypeTag"); PrototypeTag; PrototypeTag = PrototypeTag->NextSiblingElement("PrototypeTag"))
		{
			strTag = PrototypeTag->GetText();

			Desc.ModelPrototypeTags.push_back(CMyTools::ToWstring(strTag));
		}

		/* Transform */
		auto* Position = Object->FirstChildElement("Position");
		Position->QueryFloatAttribute("x", &Desc.vPosition.x);
		Position->QueryFloatAttribute("y", &Desc.vPosition.y);
		Position->QueryFloatAttribute("z", &Desc.vPosition.z);

		auto* Scale = Object->FirstChildElement("Scale");
		Scale->QueryFloatAttribute("x", &Desc.vScale.x);
		Scale->QueryFloatAttribute("y", &Desc.vScale.y);
		Scale->QueryFloatAttribute("z", &Desc.vScale.z);

		auto* Rotation = Object->FirstChildElement("Rotation");
		Rotation->QueryFloatAttribute("x", &Desc.vRotation.x);
		Rotation->QueryFloatAttribute("y", &Desc.vRotation.y);
		Rotation->QueryFloatAttribute("z", &Desc.vRotation.z);

		auto* HalfGeometryInfo = Object->FirstChildElement("HalfGeometryInfo");
		HalfGeometryInfo->QueryFloatAttribute("x", &Desc.vBoxSize.x);
		HalfGeometryInfo->QueryFloatAttribute("y", &Desc.vBoxSize.y);
		HalfGeometryInfo->QueryFloatAttribute("z", &Desc.vBoxSize.z);

		auto* LocalTranslation = Object->FirstChildElement("LocalTranslation");
		LocalTranslation->QueryFloatAttribute("x", &Desc.vBoxLocalPosition.x);
		LocalTranslation->QueryFloatAttribute("y", &Desc.vBoxLocalPosition.y);
		LocalTranslation->QueryFloatAttribute("z", &Desc.vBoxLocalPosition.z);

		CMapElement_Interactable* pInteractable = { nullptr };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Interactable>(g_iStaticLevel, NEXT_LEVEL, LAYER_INTERACTABLE, &Desc,nullptr, &pInteractable)))
			return E_FAIL;

		if (nullptr != pInteractable)
		{
			SAFE_ADDREF(pInteractable);
			m_PoolingInteractive.push_back(pInteractable);
		}
		else
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInteractiveInfo::ActiveAt_Interactive(_fvector vPosition)
{
	_uint iSize = (_uint)m_PoolingInteractive.size();

	_uint iRandomIndex = m_pGameInstance->Real_Random_Int(0, max(0, iSize - 1));

	_uint iIndex = {};

	auto iter = m_PoolingInteractive.begin();

	for (; iter != m_PoolingInteractive.end();)
	{
		if (iIndex == iRandomIndex)
		{
			(*iter)->ActivateAt(vPosition);
			m_PoolingActiveInteractive.push_back(*iter);
			iter = m_PoolingInteractive.erase(iter);
			break;
		}
		else
		{
			++iter;
			++iIndex;
		}
	}

	return S_OK;
}

HRESULT CInteractiveInfo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
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

CInteractiveInfo* CInteractiveInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	CInteractiveInfo* pInstance = new CInteractiveInfo();

	if (FAILED(pInstance->Initialize(pDevice, pContex))) {
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CInteractiveInfo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLockOnInteractive);
	for (CMapElement_Interactable* pInteractive : m_ActiveInteractive) {
		SAFE_RELEASE(pInteractive);
	} m_ActiveInteractive.clear();

	for (CMapElement_Interactable* pInteractive : m_PoolingInteractive) {
		SAFE_RELEASE(pInteractive);
	} m_ActiveInteractive.clear();

	for (CMapElement_Interactable* pInteractive : m_PoolingActiveInteractive) {
		SAFE_RELEASE(pInteractive);
	} m_ActiveInteractive.clear();


	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
