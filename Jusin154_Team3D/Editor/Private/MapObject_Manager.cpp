#include "pch.h"
#include "MapObject_Manager.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "Layer.h"
#include "MapObject.h"
#include "MapObject_LOD.h"
#include "MapObject_Static.h"
#include "BuildingContainer.h"
#include "MapElement_Static.h"
#include "MapElement_Interactable.h"


CMapObject_Manager::CMapObject_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapObject_Manager::CMapObject_Manager(const CMapObject_Manager& rhs)
	: CGameObject(rhs)
	, m_ModelPrototypeTags{ rhs.m_ModelPrototypeTags }
	, m_LODModelPrototypeTags{ rhs.m_LODModelPrototypeTags }
	, m_ModelPrototypePaths{ rhs.m_ModelPrototypePaths }
	, m_PartObjectKeyCount{ rhs.m_PartObjectKeyCount }
{
}

HRESULT CMapObject_Manager::Initialize_Prototype(vector<_wstring>& ModelPrototypeTags, vector<filesystem::path>& ModelPrototypePaths)
{
	//for (auto& Tag : ModelPrototypeTags)
	for (_uint i = 0 ; i < ModelPrototypeTags.size(); ++i)
	{
		if (_wstring::npos != ModelPrototypeTags[i].find(L"Lod"))
		{
			m_LODModelPrototypeTags.push_back(ModelPrototypeTags[i]);
		}
		else
		{
			m_PartObjectKeyCount.emplace(ModelPrototypeTags[i], 0);
		}

		m_ModelPrototypeTags.push_back(ModelPrototypeTags[i]);
		m_ModelPrototypePaths.push_back(ModelPrototypePaths[i]);
	}

	return S_OK;
}

HRESULT CMapObject_Manager::Initialize(void* pArg)												
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iContainerObjectIndex = 99;
	//m_pContainer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Building"))->Get_Object<CBuildingContainer>();

	return S_OK;
}

void CMapObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (auto* pObject : m_MapObjects)
	{
		pObject->Priority_Update(fTimeDelta);
	}
}

void CMapObject_Manager::Update(_float fTimeDelta)
{
	GUI::Begin("File");
	const char* ModeNames[3] =
	{
		"CONTAINER",
		"ELEMENT_STATIC",
		"ELEMENT_INTERACT"
	};
	GUI::Combo("MODE", (_int*)(&m_eType), ModeNames, IM_ARRAYSIZE(ModeNames));

	GUI::InputText("MapFileName", m_szSaveFileName, MAX_PATH);
	if (GUI::Button("Save_Map"))
	{
		Save_MapData(m_szSaveFileName);
	
	}
	if (GUI::Button("Load_Map"))
	{
		Load_MapData(m_szSaveFileName);
	
	}
	
	GUI::InputText("Containter Name", m_szSaveContainerName, MAX_PATH);

	if (ADD_TYPE::CONTAINER == m_eType)
	{
		if (GUI::Button("Save_Container"))
		{
			Save_ContainerData("ContainerTest", m_szSaveContainerName);
		}
	}
	if (GUI::Button("Load_Container"))
	{
		Load_ContainerData("ContainerTest", m_szSaveContainerName);
	}
	GUI::End();

	Update_PrototypeList();

	Update_ObjectList();

	if (ADD_TYPE::CONTAINER != m_eType)
	{
		Update_Edit();
	}

	if (ADD_TYPE::CONTAINER == m_eType)
	{
		Update_ContainerObject();
	}

	for (auto* pObject : m_MapObjects)
	{
		pObject->Update(fTimeDelta);
	}
}

void CMapObject_Manager::Late_Update(_float fTimeDelta)
{
	auto iter = m_MapObjects.begin();
	for (; iter != m_MapObjects.end();)
	{
		if ((*iter)->isDead())
		{
			SAFE_RELEASE(*iter);
			iter = m_MapObjects.erase(iter);
		}
		else
		{
			(*iter)->Late_Update(fTimeDelta);
			++iter;
		}
		
	}
}

HRESULT CMapObject_Manager::Render()
{
	return S_OK;
}

const filesystem::path CMapObject_Manager::Get_PrototypePath(_uint iModelIndex)
{
	return m_ModelPrototypePaths[iModelIndex];
}

HRESULT CMapObject_Manager::Ready_Components()
{
	__super::Ready_Components(nullptr);

	return S_OK;
}

HRESULT CMapObject_Manager::Bind_ShaderResources()
{
	return S_OK;
}
#pragma region SAVE & LOAD
HRESULT CMapObject_Manager::Save_MapData(const _char* pFileName)
{
	tinyxml2::XMLDocument doc;
	string strPath = "../Bin/Resources/Data/Map/" + string(pFileName) + ".xml";

	tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

	doc.Clear();
	doc.InsertFirstChild(doc.NewDeclaration());

	tinyxml2::XMLElement* land = doc.NewElement("Land");
	doc.InsertEndChild(land);

#pragma region SAVE_BUILDING
	CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Building"));

	if (nullptr == pLayer)
		return E_FAIL;
	
	const list<CGameObject*>* pList = pLayer->Get_Objects();

	for (auto pGamObject : *pList)
	{
		CBuildingContainer* pContainerObject = dynamic_cast<CBuildingContainer*>(pGamObject);

		if (nullptr == pContainerObject)
			return E_FAIL;

		string strName = pContainerObject->Get_Name();

		tinyxml2::XMLElement* container = doc.NewElement("Container");
		container->SetAttribute("Name", strName.c_str());
		land->InsertEndChild(container);

		_float3 vPosition = {};
		XMStoreFloat3(&vPosition, pGamObject->Get_Component<CTransform>()->Get_State(STATE::POSITION));

		_float3 vScale = pGamObject->Get_Component<CTransform>()->Get_Scale();

		_float3 vRotation = {};
		XMStoreFloat3(&vRotation, pGamObject->Get_Component<CTransform>()->Get_RollPitchYawVector());
		vRotation.x = XMConvertToDegrees(vRotation.x);
		vRotation.y = XMConvertToDegrees(vRotation.y);
		vRotation.z = XMConvertToDegrees(vRotation.z);

#pragma region CONTAINER_TRANSFORM
		tinyxml2::XMLElement* Position = doc.NewElement("Position");
		Position->SetAttribute("x", vPosition.x);
		Position->SetAttribute("y", vPosition.y);
		Position->SetAttribute("z", vPosition.z);
		container->InsertEndChild(Position);

		tinyxml2::XMLElement* Scale = doc.NewElement("Scale");
		Scale->SetAttribute("x", vScale.x);
		Scale->SetAttribute("y", vScale.y);
		Scale->SetAttribute("z", vScale.z);
		container->InsertEndChild(Scale);

		tinyxml2::XMLElement* Rotation = doc.NewElement("Rotation");
		Rotation->SetAttribute("x", vRotation.x);
		Rotation->SetAttribute("y", vRotation.y);
		Rotation->SetAttribute("z", vRotation.z);
		container->InsertEndChild(Rotation);
#pragma endregion

#pragma region SAVE_PARTOBJECT

		if (FAILED(pContainerObject->Save_PartObjects(doc, container)))
			return E_FAIL;
#pragma endregion
	}
#pragma endregion

#pragma region ELEMENT_STATIC
	 pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Static"));

	if (nullptr != pLayer)
	{
		const list<CGameObject*>* pList = pLayer->Get_Objects();

		for (auto pGamObject : *pList)
		{
			CMapElement* pElement = dynamic_cast<CMapElement*>(pGamObject);

			if (nullptr == pElement)
				return E_FAIL;

			_uint iLodLevel = pElement->Get_LodLevel();

			tinyxml2::XMLElement* object = doc.NewElement("Object");
			object->SetAttribute("Type", ENUM_CLASS(MAPOBJECT_TYPE::ELEMENT_STATIC));
			object->SetAttribute("Lod_Level", iLodLevel);
			land->InsertEndChild(object);

#pragma region PROTOTYPETAG
			for (_uint i = 0; i < iLodLevel + 1; ++i)
			{
				tinyxml2::XMLElement* prototype = doc.NewElement("PrototypeTag");
				prototype->SetText(CMyTools::ToString(pElement->Get_PrototypeTag(i)).c_str());
				object->InsertEndChild(prototype);
			}
#pragma endregion


#pragma region TRANSFORM
			_float3 vPosition = {};
			XMStoreFloat3(&vPosition, pGamObject->Get_Component<CTransform>()->Get_State(STATE::POSITION));

			_float3 vScale = pGamObject->Get_Component<CTransform>()->Get_Scale();

			_float3 vRotation = {};
			XMStoreFloat3(&vRotation, pGamObject->Get_Component<CTransform>()->Get_RollPitchYawVector());
			vRotation.x = XMConvertToDegrees(vRotation.x);
			vRotation.y = XMConvertToDegrees(vRotation.y);
			vRotation.z = XMConvertToDegrees(vRotation.z);

			tinyxml2::XMLElement* Position = doc.NewElement("Position");
			Position->SetAttribute("x", vPosition.x);
			Position->SetAttribute("y", vPosition.y);
			Position->SetAttribute("z", vPosition.z);
			object->InsertEndChild(Position);

			tinyxml2::XMLElement* Scale = doc.NewElement("Scale");
			Scale->SetAttribute("x", vScale.x);
			Scale->SetAttribute("y", vScale.y);
			Scale->SetAttribute("z", vScale.z);
			object->InsertEndChild(Scale);

			tinyxml2::XMLElement* Rotation = doc.NewElement("Rotation");
			Rotation->SetAttribute("x", vRotation.x);
			Rotation->SetAttribute("y", vRotation.y);
			Rotation->SetAttribute("z", vRotation.z);
			object->InsertEndChild(Rotation);
#pragma endregion

			
		}
	}
#pragma endregion

#pragma region ELEMENT_INTERACTABEL
	pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Interactable"));

	if (nullptr != pLayer)
	{
		const list<CGameObject*>* pList = pLayer->Get_Objects();

		for (auto pGamObject : *pList)
		{
			CMapElement* pElement = dynamic_cast<CMapElement*>(pGamObject);

			if (nullptr == pElement)
				return E_FAIL;

			_uint iLodLevel = pElement->Get_LodLevel();

			tinyxml2::XMLElement* object = doc.NewElement("Object");
			object->SetAttribute("Type", ENUM_CLASS(MAPOBJECT_TYPE::ELEMENT_INTERACT));
			object->SetAttribute("Lod_Level", iLodLevel);
			land->InsertEndChild(object);

#pragma region PROTOTYPETAG
			for (_uint i = 0; i < iLodLevel + 1; ++i)
			{
				tinyxml2::XMLElement* prototype = doc.NewElement("PrototypeTag");
				prototype->SetText(CMyTools::ToString(pElement->Get_PrototypeTag(i)).c_str());
				object->InsertEndChild(prototype);
			}
#pragma endregion

#pragma region TRANSFORM
			_float3 vPosition = {};
			XMStoreFloat3(&vPosition, pGamObject->Get_Component<CTransform>()->Get_State(STATE::POSITION));

			_float3 vScale = pGamObject->Get_Component<CTransform>()->Get_Scale();

			_float3 vRotation = {};
			XMStoreFloat3(&vRotation, pGamObject->Get_Component<CTransform>()->Get_RollPitchYawVector());
			vRotation.x = XMConvertToDegrees(vRotation.x);
			vRotation.y = XMConvertToDegrees(vRotation.y);
			vRotation.z = XMConvertToDegrees(vRotation.z);

			tinyxml2::XMLElement* Position = doc.NewElement("Position");
			Position->SetAttribute("x", vPosition.x);
			Position->SetAttribute("y", vPosition.y);
			Position->SetAttribute("z", vPosition.z);
			object->InsertEndChild(Position);

			tinyxml2::XMLElement* Scale = doc.NewElement("Scale");
			Scale->SetAttribute("x", vScale.x);
			Scale->SetAttribute("y", vScale.y);
			Scale->SetAttribute("z", vScale.z);
			object->InsertEndChild(Scale);

			tinyxml2::XMLElement* Rotation = doc.NewElement("Rotation");
			Rotation->SetAttribute("x", vRotation.x);
			Rotation->SetAttribute("y", vRotation.y);
			Rotation->SetAttribute("z", vRotation.z);
			object->InsertEndChild(Rotation);
#pragma endregion

		}
	}
#pragma endregion


	// ÀúÀå
	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
		MSG_BOX("Failed to Save File");
	}
	else
	{
		MSG_BOX("Succeed to Save File");
	}

	return S_OK;
}

HRESULT CMapObject_Manager::Save_ContainerData(const _char* pFileName, const _char* pContainerName)
{
	if (true == m_MapObjects.empty())
		return E_FAIL;

	tinyxml2::XMLDocument doc;
	string strPath = "../Bin/Resources/Data/Map/" + string(pFileName) + ".xml";

	tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

	tinyxml2::XMLElement* land = {};
	tinyxml2::XMLElement* container = {};

	if (loadResult != tinyxml2::XML_SUCCESS)
	{
		doc.InsertFirstChild(doc.NewDeclaration());

		land = doc.NewElement("Land");
		doc.InsertEndChild(land);

		container = doc.NewElement("Container");
		container->SetAttribute("Name", pContainerName);
		land->InsertEndChild(container);
	}
	else
	{
		land = doc.FirstChildElement("Land");
		container = doc.NewElement("Container");
		container->SetAttribute("Name", pContainerName);
		land->InsertEndChild(container);
	}

#pragma region CONTAINER_DEFAULT_TRANSFORM
	tinyxml2::XMLElement* Position = doc.NewElement("Position");
	Position->SetAttribute("x", 0.f);
	Position->SetAttribute("y", 0.f);
	Position->SetAttribute("z", 0.f);
	container->InsertEndChild(Position);

	tinyxml2::XMLElement* Scale = doc.NewElement("Scale");
	Scale->SetAttribute("x", 1.f);
	Scale->SetAttribute("y", 1.f);
	Scale->SetAttribute("z", 1.f);
	container->InsertEndChild(Scale);

	tinyxml2::XMLElement* Rotation = doc.NewElement("Rotation");
	Rotation->SetAttribute("x", 0.f);
	Rotation->SetAttribute("y", 0.f);
	Rotation->SetAttribute("z", 0.f);
	container->InsertEndChild(Rotation);
#pragma endregion

#pragma region SAVE_PARTOBJECT
	for (auto& pMapObject : m_MapObjects)
	{
		// <PartObject>
		tinyxml2::XMLElement* PartObject = doc.NewElement("PartObject");
		_uint iLodLvel = pMapObject->Get_LodLevel();

		PartObject->SetAttribute("Lod_Level", iLodLvel);
		PartObject->SetAttribute("Key_Index", pMapObject->Get_iKeyIndex());
		container->InsertEndChild(PartObject);

		for (_uint i = 0; i < pMapObject->Get_LodLevel() + 1; ++i)
		{
			// <PrototypeTag>ÅØ½ºÆ®</PrototypeTag>
			tinyxml2::XMLElement* proto = doc.NewElement("PrototypeTag");
			proto->SetText(CMyTools::ToString(pMapObject->Get_PrototypeTag(i)).c_str());
			PartObject->InsertEndChild(proto);
		}

		// <Position x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* PositionElem = doc.NewElement("Position");

		_float3 vPosition = {};
		XMStoreFloat3(&vPosition, pMapObject->Get_Component<CTransform>()->Get_State(STATE::POSITION));

		PositionElem->SetAttribute("x", vPosition.x);
		PositionElem->SetAttribute("y", vPosition.y);
		PositionElem->SetAttribute("z", vPosition.z);
		PartObject->InsertEndChild(PositionElem);

		// <Scale x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* ScaleElem = doc.NewElement("Scale");

		_float3 pScale = pMapObject->Get_Component<CTransform>()->Get_Scale();
		ScaleElem->SetAttribute("x", pScale.x);
		ScaleElem->SetAttribute("y", pScale.y);
		ScaleElem->SetAttribute("z", pScale.z);
		PartObject->InsertEndChild(ScaleElem);

		// <Rotation x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* RotationElem = doc.NewElement("Rotation");

		_float3 vRotation = {};
		XMStoreFloat3(&vRotation, pMapObject->Get_Component<CTransform>()->Get_RollPitchYawVector());

		vRotation.x = XMConvertToDegrees(vRotation.x);
		vRotation.y = XMConvertToDegrees(vRotation.y);
		vRotation.z = XMConvertToDegrees(vRotation.z);

		RotationElem->SetAttribute("x", vRotation.x);
		RotationElem->SetAttribute("y", vRotation.y);
		RotationElem->SetAttribute("z", vRotation.z);
		PartObject->InsertEndChild(RotationElem);

		SAFE_RELEASE(pMapObject);
	}
	m_MapObjects.clear();
	m_pSelectObject = nullptr;
#pragma endregion

	// ÀúÀå
	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
		MSG_BOX("Failed to Save File");
	}
	else
	{
		MSG_BOX("Succeed to Save File");
	}

	return S_OK;
}

HRESULT CMapObject_Manager::Load_MapData(const _char* pFileName)
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/" + string(pFileName) + ".xml";

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

		m_pGameInstance->Add_GameObject_ToLayer<CBuildingContainer>(
			g_iStaticLevel, ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Building"), &ContainerDesc, nullptr, &pContainerObject);
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

		if(MAPOBJECT_TYPE::ELEMENT_STATIC  == eType)
			m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Static>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Static"), &Desc);
		else if(MAPOBJECT_TYPE::ELEMENT_INTERACT == eType)
			m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Interactable>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Interact"), &Desc);
	}

	return S_OK;
}

HRESULT CMapObject_Manager::Load_ContainerData(const _char* pFileName, const _char* pContainerName)
{
	tinyxml2::XMLDocument xmlDoc;
	
	string strPath = "../Bin/Resources/Data/Map/" + string(pFileName) + ".xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("Land");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	tinyxml2::XMLElement* TargetContainer = { nullptr };

	for (auto* Container = root->FirstChildElement("Container"); Container; Container = Container->NextSiblingElement("Container"))
	{
		const _char* pName = {};

		Container->QueryStringAttribute("Name", &pName);

		if (!strcmp(pContainerName, pName))
		{
			TargetContainer = Container;
			break;
		}
	}

	/* Failed to find Container */
	if (nullptr == TargetContainer)
	{
		xmlDoc.Clear();
		MSG_BOX("Failed to find Container");
		return S_OK;
	}
#pragma region ADD_CONTAINER_OBJECT

	CBuildingContainer* pContainerObject = { nullptr };
	CMapContainer::MAP_CONTAINER_DESC  ContainerDesc = {};

	auto* ContainerPosition = TargetContainer->FirstChildElement("Position");
	ContainerPosition->QueryFloatAttribute("x", &ContainerDesc.vPosition.x);
	ContainerPosition->QueryFloatAttribute("y", &ContainerDesc.vPosition.y);
	ContainerPosition->QueryFloatAttribute("z", &ContainerDesc.vPosition.z);

	auto* ContainerScale = TargetContainer->FirstChildElement("Scale");
	ContainerScale->QueryFloatAttribute("x", &ContainerDesc.vScale.x);
	ContainerScale->QueryFloatAttribute("y", &ContainerDesc.vScale.y);
	ContainerScale->QueryFloatAttribute("z", &ContainerDesc.vScale.z);

	auto* ContainerRotation = TargetContainer->FirstChildElement("Rotation");
	ContainerRotation->QueryFloatAttribute("x", &ContainerDesc.vRotation.x);
	ContainerRotation->QueryFloatAttribute("y", &ContainerDesc.vRotation.y);
	ContainerRotation->QueryFloatAttribute("z", &ContainerDesc.vRotation.z);

	ContainerDesc.strName = string(pContainerName);

	m_pGameInstance->Add_GameObject_ToLayer<CBuildingContainer>(g_iStaticLevel, ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Building"), &ContainerDesc, nullptr, (CBuildingContainer**)(&pContainerObject));
#pragma endregion

#pragma region ADD_PARTOBJECT
	for (auto* PartObject = TargetContainer->FirstChildElement("PartObject"); PartObject; PartObject = PartObject->NextSiblingElement("PartObject"))
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

			pContainerObject->Add_Part<CMapObject_LOD>(strKey, g_iStaticLevel, nullptr, &Desc);
#pragma endregion

	}
#pragma endregion

	return S_OK;
}
#pragma endregion


void CMapObject_Manager::Update_PrototypeList()
{
	GUI::Begin("Model Prototype List");

	_uint iMaxSize = (_uint)m_ModelPrototypeTags.size();
	for (_uint i = 0; i < iMaxSize; ++i)
	{
		auto& Tag = m_ModelPrototypeTags[i];

		if (_wstring::npos != Tag.find(L"Lod")) {
			continue;
		}

		if (ImGui::Button(CMyTools::ToString(Tag).c_str()))
		{
			switch (m_eType)
			{
			case Editor::CMapObject_Manager::ADD_TYPE::CONTAINER:
				Create_PartObject(Tag);
				break;

			case Editor::CMapObject_Manager::ADD_TYPE::ELEMENT_STATIC:
				Create_Elemnt_Static(Tag);
				break;

			case Editor::CMapObject_Manager::ADD_TYPE::ELEMENT_INTERACT:
				Create_Elemnt_Interact(Tag);
				break;
			default:
				break;
			}
	
		}
	}
	GUI::End();
	
}

void CMapObject_Manager::Update_ObjectList()
{
	CLayer* pLayer = { nullptr };

	//m_pMapObjects = pLayer->Get_Objects();

	GUI::Begin("MapObject List");

	_uint iObjectIndex = {};
	string strSrc;

	if(ADD_TYPE::CONTAINER == m_eType)
	{
		if (false == m_MapObjects.empty())
		{
			for (auto pMapObject : m_MapObjects)
			{
				CMapObject* pCurrentMapObject = static_cast<CMapObject*>(pMapObject);

				if (nullptr == pCurrentMapObject)
					continue;

				wstring strPrototypeTag = pCurrentMapObject->Get_PrototypeTag();

				strSrc = CMyTools::ToString(strPrototypeTag) + to_string(iObjectIndex) + "##" + to_string(iObjectIndex++);

				if (ImGui::Button(strSrc.c_str()))
				{
					m_pSelectObject = pCurrentMapObject;
				}
			}
		}
	}
	else 
	{
		if (ADD_TYPE::ELEMENT_STATIC == m_eType)
			pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Static"));
		else if (ADD_TYPE::ELEMENT_INTERACT == m_eType)
			pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Interact"));
		
		if (nullptr == pLayer)
		{
			GUI::End();
			return;
		}

		const list<class CGameObject*>* pObjectList = pLayer->Get_Objects();

		for (auto pObject : *pObjectList)
		{
			CMapElement* pCurrentMapObject = static_cast<CMapElement*>(pObject);

			if (nullptr == pCurrentMapObject)
				continue;

			wstring strPrototypeTag = pCurrentMapObject->Get_PrototypeTag(0);

			strSrc = CMyTools::ToString(strPrototypeTag) + to_string(iObjectIndex) + "##" + to_string(iObjectIndex++);

			if (ImGui::Button(strSrc.c_str()))
			{
				m_pSelectElemnt = pCurrentMapObject;
			}
		}
	}


	GUI::End();
}

void CMapObject_Manager::Update_Edit()
{
	GUI::Begin("Edit");

	if (ADD_TYPE::CONTAINER == m_eType)
	{
		if (m_pGameInstance->Mouse_Down(DIM_4))
			m_pSelectObject = nullptr;

		if (nullptr != m_pSelectObject)
		{
			m_pSelectObject->Describe_Entity();
			if (m_pSelectObject->isDead())
				m_pSelectObject = nullptr;

		}
	}
	else
	{
		if (m_pGameInstance->Mouse_Down(DIM_4))
			m_pSelectElemnt = nullptr;

		if (nullptr != m_pSelectElemnt)
		{
			m_pSelectElemnt->Describe_Entity();
			if (m_pSelectElemnt->isDead())
				m_pSelectElemnt = nullptr;
		}
	}

	GUI::End();
}

void CMapObject_Manager::Update_ContainerObject()
{
	GUI::Begin("Edit Container");

	CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Building"));

	if (nullptr == pLayer)
	{
		GUI::End();
		return;
	}
	const list<CGameObject*>* pList = pLayer->Get_Objects();

	_uint iIndex = {};

	if (false == (*pList).empty())
	{
		CGameObject* pSelectObject = { nullptr };

		for (auto& pObject : (*pList))
		{
			if (GUI::Button((dynamic_cast<CMapContainer*>(pObject)->Get_Name().c_str())))
			{
				m_iContainerObjectIndex = iIndex;
			}

			if (iIndex == m_iContainerObjectIndex)
				pSelectObject = pObject;

			++iIndex;
		}

		if (nullptr != pSelectObject)
			pSelectObject->Describe_Entity();
	}

	GUI::End();
}

void CMapObject_Manager::Create_PartObject(_wstring& strPrototypeTag)
{
	vector<_uint> LodModelIndices;

	MAPOBJECT_LOD_DESC Desc = {};

	vector<_wstring> PrototypeTags;

	Find_Lod_Prototype(strPrototypeTag, LodModelIndices);

	PrototypeTags.push_back(strPrototypeTag);

	for (_uint i = 0; i < LodModelIndices.size(); ++i)
	{
		_wstring strLodTag = strPrototypeTag + L"_Lod" + to_wstring(i + 1);
		PrototypeTags.push_back(strLodTag);
	}

	Desc.iMaxLodLevel = (_uint)LodModelIndices.size() - 1;
	Desc.ModelPrototypeTags = PrototypeTags;
	Desc.pParentTransform = m_pTransformCom;
	Desc.vPosition = _float3(0.f, 0.f, 0.f);
	Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	Desc.pModelPathIndices = &LodModelIndices;

	CMapObject_LOD* pMapObject = { nullptr };
	const string strKey = CMyTools::ToString(strPrototypeTag);

	pMapObject = m_pGameInstance->Clone_Prototype<CMapObject_LOD>(g_iStaticLevel, &Desc);
	pMapObject->Set_KeyIndex(Get_KeyCount(strPrototypeTag));

	m_MapObjects.push_back(pMapObject);
}

void CMapObject_Manager::Create_Elemnt_Static(_wstring& strPrototypeTag)
{
	vector<_uint> LodModelIndices;

	MAPOBJECT_LOD_DESC Desc = {};

	vector<_wstring> PrototypeTags;

	Find_Lod_Prototype(strPrototypeTag, LodModelIndices);

	PrototypeTags.push_back(strPrototypeTag);

	for (_uint i = 0; i < LodModelIndices.size(); ++i)
	{
		_wstring strLodTag = strPrototypeTag + L"_Lod" + to_wstring(i + 1);
		PrototypeTags.push_back(strLodTag);
	}

	Desc.iMaxLodLevel = (_uint)LodModelIndices.size() - 1;
	Desc.ModelPrototypeTags = PrototypeTags;
	Desc.pParentTransform = m_pTransformCom;
	Desc.vPosition = _float3(0.f, 0.f, 0.f);
	Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	Desc.pModelPathIndices = &LodModelIndices;

	CMapElement_Static* pMapObject = { nullptr };
	const string strKey = CMyTools::ToString(strPrototypeTag);

	m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Static>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Static"), &Desc);
}

void CMapObject_Manager::Create_Elemnt_Interact(_wstring& strPrototypeTag)
{
	vector<_uint> LodModelIndices;

	MAPOBJECT_LOD_DESC Desc = {};

	vector<_wstring> PrototypeTags;

	Find_Lod_Prototype(strPrototypeTag, LodModelIndices);

	PrototypeTags.push_back(strPrototypeTag);

	for (_uint i = 0; i < LodModelIndices.size(); ++i)
	{
		_wstring strLodTag = strPrototypeTag + L"_Lod" + to_wstring(i + 1);
		PrototypeTags.push_back(strLodTag);
	}

	Desc.iMaxLodLevel = (_uint)LodModelIndices.size() - 1;
	Desc.ModelPrototypeTags = PrototypeTags;
	Desc.pParentTransform = m_pTransformCom;
	Desc.vPosition = _float3(0.f, 0.f, 0.f);
	Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	Desc.pModelPathIndices = &LodModelIndices;

	CMapElement_Interactable* pMapObject = { nullptr };
	const string strKey = CMyTools::ToString(strPrototypeTag);

	m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Interactable>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Interact"), &Desc);
}

_bool CMapObject_Manager::Find_Lod_Prototype(_wstring strPrototypeTag, vector<_uint>& LodModelIndices)
{
	// for (auto& Tag : m_LODModelPrototypeTags)
	for(_uint i = 0 ; i < m_ModelPrototypeTags.size(); ++i)
	{
		if (_wstring::npos != m_ModelPrototypeTags[i].find(strPrototypeTag) && 5 > (m_ModelPrototypeTags[i].size() - strPrototypeTag.size()))
			LodModelIndices.push_back(i);
	}
	if (0 < LodModelIndices.size()) {
		return true;
	}
	return false;
	//if (0 < *iCount)
	//	return true;
}

_uint CMapObject_Manager::Get_KeyCount(_wstring strPrototypeTag)
{
	auto iter = m_PartObjectKeyCount.find(strPrototypeTag);

	if (iter == m_PartObjectKeyCount.end())
		return 0;

	return iter->second++;
}


CMapObject_Manager* CMapObject_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<_wstring>& ModelPrototypeTags, vector<filesystem::path>& ModelPrototypePaths)
{
	CMapObject_Manager* pInstance = new CMapObject_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(ModelPrototypeTags, ModelPrototypePaths)))
	{
		MSG_BOX("Failed to Created : CMapObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject_Manager::Clone(void* pArg, CGameObject* pOwner)
{
	CMapObject_Manager* pInstance = new CMapObject_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapObject_Manager::Free()
{
	__super::Free();

	for (auto& pObject : m_MapObjects)
	{
		SAFE_RELEASE(pObject);
	}

	SAFE_RELEASE(m_pContainer);
}

void CMapObject_Manager::Describe_Entity()
{
}