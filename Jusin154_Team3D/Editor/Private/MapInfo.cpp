#include "pch.h"
#include "MapInfo.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "BuildingContainer.h"
#include "MapObject_Collision.h"
#include "MapElement_Light.h"
#include "Layer.h"
#include "MapElement_Static.h"
#include "MapElement_Interactable.h"
#include "MapObject_LOD.h"
#include "LightSpawner.h"
#include "DummyDecal.h"

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

HRESULT CMapInfo::Load_MapObjects(const _char* pFilePath)
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/" + string(pFilePath) + ".xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("Land");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	/* Container */
	for (auto* Container = root->FirstChildElement("Container"); Container; Container = Container->NextSiblingElement("Container"))
	{
		const _char* pName = {};

		Container->QueryStringAttribute("Name", &pName);
#pragma region ADD_CONTAINER_OBJECT

		CBuildingContainer* pContainerObject = { nullptr };
		CMapContainer::MAP_CONTAINER_DESC  ContainerDesc = {};

		auto* ContainerPosition = Container->FirstChildElement("Position");
		ContainerPosition->QueryFloatAttribute("x", &ContainerDesc.vPosition.x);
		ContainerPosition->QueryFloatAttribute("y", &ContainerDesc.vPosition.y);
		ContainerPosition->QueryFloatAttribute("z", &ContainerDesc.vPosition.z);

		auto* ContainerScale = Container->FirstChildElement("Scale");
		ContainerScale->QueryFloatAttribute("x", &ContainerDesc.vScale.x);
		ContainerScale->QueryFloatAttribute("y", &ContainerDesc.vScale.y);
		ContainerScale->QueryFloatAttribute("z", &ContainerDesc.vScale.z);

		auto* ContainerRotation = Container->FirstChildElement("Rotation");
		ContainerRotation->QueryFloatAttribute("x", &ContainerDesc.vRotation.x);
		ContainerRotation->QueryFloatAttribute("y", &ContainerDesc.vRotation.y);
		ContainerRotation->QueryFloatAttribute("z", &ContainerDesc.vRotation.z);

		ContainerDesc.strName = string(pName);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBuildingContainer>(
			g_iStaticLevel, g_iStaticLevel, TEXT("Layer_Building"), &ContainerDesc, nullptr, &pContainerObject)))
		{
			MSG_BOX("Faield Clone Building");
			return E_FAIL;
		}
#pragma endregion

#pragma region ADD_PARTOBJECT
		for (auto* PartObject = Container->FirstChildElement("PartObject"); PartObject; PartObject = PartObject->NextSiblingElement("PartObject"))
		{
			_uint iLodLevel = {};
			_uint iKeyIndex = {};

			PartObject->QueryUnsignedAttribute("Lod_Level", &iLodLevel);
			PartObject->QueryUnsignedAttribute("Key_Index", &iKeyIndex);

#pragma region MAPOBJECT_LOD
			MAPOBJECT_LOD_DESC Desc = {};
			string strTag = {};
			for (auto* PrototypeTag = PartObject->FirstChildElement("PrototypeTag"); PrototypeTag; PrototypeTag = PrototypeTag->NextSiblingElement("PrototypeTag"))
			{
				strTag = PrototypeTag->GetText();

				Desc.ModelPrototypeTags.push_back(CMyTools::ToWstring(strTag));
			}

			Desc.iMaxLodLevel = iLodLevel;
			Desc.pParentTransform = pContainerObject->Get_Component<CTransform>();
			const string strKey = CMyTools::ToString(Desc.ModelPrototypeTags.front()) + to_string(iKeyIndex);

			/* Position */
			tinyxml2::XMLElement* Position = PartObject->FirstChildElement("Position");
			Position->QueryFloatAttribute("x", &Desc.vPosition.x);
			Position->QueryFloatAttribute("y", &Desc.vPosition.y);
			Position->QueryFloatAttribute("z", &Desc.vPosition.z);

			/* Scale */
			tinyxml2::XMLElement* Scale = PartObject->FirstChildElement("Scale");
			Scale->QueryFloatAttribute("x", &Desc.vScale.x);
			Scale->QueryFloatAttribute("y", &Desc.vScale.y);
			Scale->QueryFloatAttribute("z", &Desc.vScale.z);

			/* Rotation */
			tinyxml2::XMLElement* Rotation = PartObject->FirstChildElement("Rotation");
			Rotation->QueryFloatAttribute("x", &Desc.vRotation.x);
			Rotation->QueryFloatAttribute("y", &Desc.vRotation.y);
			Rotation->QueryFloatAttribute("z", &Desc.vRotation.z);

			CMapObject_LOD* pMapObject;

			if (FAILED(pContainerObject->Add_Part<CMapObject_LOD>(strKey, g_iStaticLevel, &pMapObject, &Desc)))
				return E_FAIL;

			pMapObject->Set_KeyIndex(iKeyIndex);

			SAFE_RELEASE(pMapObject);
#pragma endregion

		}
#pragma endregion

		pContainerObject->Set_BoundingBox();

#pragma region ADD_COLLISION
		for (auto* Collision = Container->FirstChildElement("Collision"); Collision; Collision = Collision->NextSiblingElement("Collision"))
		{
			_uint iLodLevel = {};
			_uint iKeyIndex = {};

			Collision->QueryUnsignedAttribute("Lod_Level", &iLodLevel);
			Collision->QueryUnsignedAttribute("Key_Index", &iKeyIndex);

#pragma region MAPOBJECT_LOD
			MAPOBJECT_LOD_DESC Desc = {};
			string strTag = {};
			for (auto* PrototypeTag = Collision->FirstChildElement("PrototypeTag"); PrototypeTag; PrototypeTag = PrototypeTag->NextSiblingElement("PrototypeTag"))
			{
				strTag = PrototypeTag->GetText();

				Desc.ModelPrototypeTags.push_back(CMyTools::ToWstring(strTag));
			}
			Desc.iMaxLodLevel = iLodLevel;
			Desc.pParentTransform = pContainerObject->Get_Component<CTransform>();

			/* Position */
			tinyxml2::XMLElement* Position = Collision->FirstChildElement("Position");
			Position->QueryFloatAttribute("x", &Desc.vPosition.x);
			Position->QueryFloatAttribute("y", &Desc.vPosition.y);
			Position->QueryFloatAttribute("z", &Desc.vPosition.z);

			/* Scale */
			tinyxml2::XMLElement* Scale = Collision->FirstChildElement("Scale");
			Scale->QueryFloatAttribute("x", &Desc.vScale.x);
			Scale->QueryFloatAttribute("y", &Desc.vScale.y);
			Scale->QueryFloatAttribute("z", &Desc.vScale.z);

			/* Rotation */
			tinyxml2::XMLElement* Rotation = Collision->FirstChildElement("Rotation");
			Rotation->QueryFloatAttribute("x", &Desc.vRotation.x);
			Rotation->QueryFloatAttribute("y", &Desc.vRotation.y);
			Rotation->QueryFloatAttribute("z", &Desc.vRotation.z);

			pContainerObject->Add_Collision<CMapObject_Collision>(g_iStaticLevel, &Desc);
#pragma endregion
			SAFE_RELEASE(pContainerObject);
		}
#pragma endregion
	}

	for (auto* Object = root->FirstChildElement("Object"); Object; Object = Object->NextSiblingElement("Object"))
	{
		MAPOBJECT_LOD_DESC Desc = {};

		MAPOBJECT_TYPE eType = {};

		Object->QueryUnsignedAttribute("Type", (_uint*)(&eType));
		Object->QueryUnsignedAttribute("Lod_Level", &Desc.iMaxLodLevel);

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

		string strTag = {};

		for (auto* PrototypeTag = Object->FirstChildElement("PrototypeTag"); PrototypeTag; PrototypeTag = PrototypeTag->NextSiblingElement("PrototypeTag"))
		{
			strTag = PrototypeTag->GetText();

			Desc.ModelPrototypeTags.push_back(CMyTools::ToWstring(strTag));
		}

		//if (MAPOBJECT_TYPE::ELEMENT_STATIC == eType)
			m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Static>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Static"), &Desc);
		//else if (MAPOBJECT_TYPE::ELEMENT_INTERACT == eType)
			//m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Interactable>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Interactable"), &Desc);
	}


	return S_OK;
}

HRESULT CMapInfo::Load_PointLights(const _char* pFilePath)
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/Light/" + string(pFilePath) + ".xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("MapLightObjects");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	for (auto* Object = root->FirstChildElement("Object"); Object; Object = Object->NextSiblingElement("Object"))
	{
		CLightSpawner::LIGHTSPAWNER_DESC Desc = {};

		/* Transform */
		auto* Position = Object->FirstChildElement("Position");
		Position->QueryFloatAttribute("x", &Desc.vPosition.x);
		Position->QueryFloatAttribute("y", &Desc.vPosition.y);
		Position->QueryFloatAttribute("z", &Desc.vPosition.z);

		/* Light Desc */
		auto* Diffuse = Object->FirstChildElement("Diffuse");
		Diffuse->QueryFloatAttribute("x", &Desc.vDiffuse.x);
		Diffuse->QueryFloatAttribute("y", &Desc.vDiffuse.y);
		Diffuse->QueryFloatAttribute("z", &Desc.vDiffuse.z);
		Diffuse->QueryFloatAttribute("w", &Desc.vDiffuse.w);

		auto* Ambient = Object->FirstChildElement("Ambient");
		Ambient->QueryFloatAttribute("x", &Desc.vAmbient.x);
		Ambient->QueryFloatAttribute("y", &Desc.vAmbient.y);
		Ambient->QueryFloatAttribute("z", &Desc.vAmbient.z);

		auto* Specular = Object->FirstChildElement("Specular");
		Specular->QueryFloatAttribute("x", &Desc.vSpecular.x);
		Specular->QueryFloatAttribute("y", &Desc.vSpecular.y);
		Specular->QueryFloatAttribute("z", &Desc.vSpecular.z);

		auto* PosOffset = Object->FirstChildElement("PosOffset");
		PosOffset->QueryFloatAttribute("x", &Desc.vPosOffset.x);
		PosOffset->QueryFloatAttribute("y", &Desc.vPosOffset.y);
		PosOffset->QueryFloatAttribute("z", &Desc.vPosOffset.z);

		auto* Info = Object->FirstChildElement("Info");
		Info->QueryFloatAttribute("LightAttRange", &Desc.fLightAttRange);
		Info->QueryFloatAttribute("LightAttSpeed", &Desc.fLightAttSpeed);
		Info->QueryFloatAttribute("Range", &Desc.fRange);
		_uint iPow = {};
		Info->QueryUnsignedAttribute("Pow", &iPow);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLightSpawner>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_LightSpawer"), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapInfo::Load_Decal(const _char* pFilePath)
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/Decal/" + string(pFilePath) + ".xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("Decal");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	for (auto* Object = root->FirstChildElement("Object"); Object; Object = Object->NextSiblingElement("Object"))
	{
		CDummyDecal::DUMMY_DECAL_DESC Desc = {};

		/* Transform */
		auto* Rotation = Object->FirstChildElement("Scale");
		Rotation->QueryFloatAttribute("x", &Desc.vScale.x);
		Rotation->QueryFloatAttribute("y", &Desc.vScale.y);
		Rotation->QueryFloatAttribute("z", &Desc.vScale.z);

		auto* Scale = Object->FirstChildElement("Rotation");
		Scale->QueryFloatAttribute("x", &Desc.vRotation.x);
		Scale->QueryFloatAttribute("y", &Desc.vRotation.y);
		Scale->QueryFloatAttribute("z", &Desc.vRotation.z);

		auto* Position = Object->FirstChildElement("Position");
		Position->QueryFloatAttribute("x", &Desc.vPosition.x);
		Position->QueryFloatAttribute("y", &Desc.vPosition.y);
		Position->QueryFloatAttribute("z", &Desc.vPosition.z);

		/* Decal Desc */
		auto* Shader_Value = Object->FirstChildElement("Shader_Value");
		Shader_Value->QueryFloatAttribute("UVTiling", &Desc.fUVTiling);
		Shader_Value->QueryFloatAttribute("UVSpeedX", &Desc.vUVSpeed.x);
		Shader_Value->QueryFloatAttribute("UVSpeedY", &Desc.vUVSpeed.y);

		/* MaskRed */
		auto* MaskRed = Object->FirstChildElement("MaskRed");
		MaskRed->QueryFloatAttribute("r", &Desc.vMaskRed.x);
		MaskRed->QueryFloatAttribute("g", &Desc.vMaskRed.y);
		MaskRed->QueryFloatAttribute("b", &Desc.vMaskRed.z);
		MaskRed->QueryFloatAttribute("a", &Desc.vMaskRed.w);

		/* MaskGreen */
		auto* MaskGreen = Object->FirstChildElement("MaskGreen");
		MaskGreen->QueryFloatAttribute("r", &Desc.vMaskGreen.x);
		MaskGreen->QueryFloatAttribute("g", &Desc.vMaskGreen.y);
		MaskGreen->QueryFloatAttribute("b", &Desc.vMaskGreen.z);
		MaskGreen->QueryFloatAttribute("a", &Desc.vMaskGreen.w);

		/* MaskBlue */
		auto* MaskBlue = Object->FirstChildElement("MaskBlue");
		MaskBlue->QueryFloatAttribute("r", &Desc.vMaskBlue.x);
		MaskBlue->QueryFloatAttribute("g", &Desc.vMaskBlue.y);
		MaskBlue->QueryFloatAttribute("b", &Desc.vMaskBlue.z);
		MaskBlue->QueryFloatAttribute("a", &Desc.vMaskBlue.w);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummyDecal>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Decal"), &Desc)))
			return E_FAIL;
	}

	return S_OK;
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

	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);

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
