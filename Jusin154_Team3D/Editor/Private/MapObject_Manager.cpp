#include "pch.h"
#include "MapObject_Manager.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Camera_Debug.h"
#include "Layer.h"
#include "MapObject.h"
#include "MapObject_LOD.h"
#include "MapObject_Collision.h"
#include "BuildingContainer.h"
#include "MapElement_Static.h"
#include "MapElement_Interactable.h"
#include "MapElement_Light.h"
#include "MapElement_Lake.h"
#include "LightSpawner.h"


CMapObject_Manager::CMapObject_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext), m_pInfoInstance{CInfoInstance::GetInstance()}
{
	SAFE_ADDREF(m_pInfoInstance);
}

CMapObject_Manager::CMapObject_Manager(const CMapObject_Manager& rhs)
	: CGameObject(rhs)
	, m_pInfoInstance{ rhs.m_pInfoInstance }
	, m_ModelPrototypeTags{ rhs.m_ModelPrototypeTags }
	, m_LODModelPrototypeTags{ rhs.m_LODModelPrototypeTags }
	, m_ModelPrototypePaths{ rhs.m_ModelPrototypePaths }
	, m_PartObjectKeyCount{ rhs.m_PartObjectKeyCount }
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CMapObject_Manager::Initialize_Prototype(vector<_wstring>& ModelPrototypeTags, vector<filesystem::path>& ModelPrototypePaths)
{
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

	//if (FAILED(Load_MapData("Hogsmeade_MapContainer_Data", LAYER_HOGSMEADE)))
	//	return E_FAIL;
	if (FAILED(Load_MapData("Dungeon_Map_Data", L"LAYER_BACKGOURN")))
		return E_FAIL;

	//m_pContainer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Building"))->Get_Object<CBuildingContainer>();
	
	/*if (FAILED(Load_WaterObject("Element_Water_Info")))
		return E_FAIL;*/

	/*if (FAILED(Load_LightObject("LightElement")))
		return E_FAIL;*/
	//
	//if (FAILED(Load_InteractObject("E_INTER_Barrel")))
	//	return E_FAIL;
	//if (FAILED(Load_InteractObject("E_INTER_PostPackage_B")))
	//	return E_FAIL;
	//if (FAILED(Load_InteractObject("E_INTER_PostPackage_F")))
	//	return E_FAIL;
	//if (FAILED(Load_InteractObject("E_INTER_TeaShopTable")))
	//	return E_FAIL;
	//if (FAILED(Load_InteractObject("E_INTER_TeaShopChair")))
	//	return E_FAIL;


#pragma region Light
	m_Light_Desc.eType = LIGHT::POINT;

	m_Light_Desc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	m_Light_Desc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	m_Light_Desc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	m_Light_Desc.vPosOffset = _float4(1.f, 1.f, 1.f, 1.f);

	m_Light_Desc.fRange = 5.f;
	m_Light_Desc.iLevel = NEXT_LEVEL;
	m_Light_Desc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);
#pragma endregion


	return S_OK;
}

void CMapObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (auto* pObject : m_MapObjects)
	{
		pObject->Priority_Update(fTimeDelta);
	}
	for (auto* pObject : m_Collision)
	{
		pObject->Priority_Update(fTimeDelta);
	}
}

void CMapObject_Manager::Update(_float fTimeDelta)
{
	GUI::Begin("Map Manager");
	const char* ModeNames[4] =
	{
		"CONTAINER",
		"ELEMENT_STATIC",
		"ELEMENT_INTERACT",
		"ELEMENT_LIGHT"
	};
	GUI::Combo("MODE", (_int*)(&m_eType), ModeNames, IM_ARRAYSIZE(ModeNames));
	GUI::Checkbox("Model View", &m_bModelVisable);
	GUI::Checkbox("COL View", &m_bCollisionVisable);
	GUI::InputText("MapFileName", m_szSaveFileName, MAX_PATH);
	if (GUI::Button("Save_Map"))
	{
		/*if (ADD_TYPE::ELEMENT_STATIC == m_eType)
		{
			Save_StaticElements(m_szSaveFileName);
		}
		else
		{*/
			Save_MapData(m_szSaveFileName);
		//}
	
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
			Save_ContainerData("HogwartContainer", m_szSaveContainerName);
		}
	}
	else if (ADD_TYPE::ELEMENT_LIGHT == m_eType)
	{
		GUI::InputInt("GlassIndex", &m_iGlassIndex);
		m_iGlassIndex = min(1, m_iGlassIndex);
		if (GUI::Button("Save_LightElements"))
		{
			Save_LightObject(m_szSaveContainerName);
		}
		if (GUI::Button("Load_LightElements"))
		{
			Load_LightObject(m_szSaveContainerName);
		}
	}
	else if (ADD_TYPE::ELEMENT_INTERACT == m_eType)
	{
		if (GUI::Button("Save_InteractElements"))
		{
			Save_InteractObject(m_szSaveContainerName);
		}
		if (GUI::Button("Load_InteractElements"))
		{
			Load_InteractObject(m_szSaveContainerName);
		}
	}

	if (GUI::Button("Load_Container"))
	{
		Load_ContainerData("HogwartContainer", m_szSaveContainerName);
	}
	if (GUI::Button("Load_Container To MapObject"))
	{
		Load_ContainerToMapObject("HogwartContainer", m_szSaveContainerName);
	}
	GUI::End();

	Update_PrototypeList();

	Update_ObjectList();

	Update_Edit();

	Update_LightSpawer();

	Update_Unified();

	if (ADD_TYPE::CONTAINER == m_eType)
	{
		Update_ContainerObject();
	}

	for (auto* pObject : m_MapObjects)
	{
		pObject->Update(fTimeDelta);
	}
	for (auto* pObject : m_Collision)
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
			if(m_bModelVisable)
				(*iter)->Late_Update(fTimeDelta);
			++iter;
		}
		
	}

	auto Col_iter = m_Collision.begin();
	for (; Col_iter != m_Collision.end();)
	{
		if ((*Col_iter)->isDead())
		{
			SAFE_RELEASE(*Col_iter);
			Col_iter = m_Collision.erase(Col_iter);
		}
		else
		{
			if (m_bCollisionVisable)
				(*Col_iter)->Late_Update(fTimeDelta);
			++Col_iter;
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

	if (nullptr != pLayer)
	{
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

#pragma region BOUNDING_BOX
			tinyxml2::XMLElement* MapContainerType = doc.NewElement("MapContainerType");
			if (pContainerObject->Is_OcclusionPassed())
				MapContainerType->SetAttribute("type", 0); // Building
			else
				MapContainerType->SetAttribute("type", 1); // Road

			container->InsertEndChild(MapContainerType);

			_float3 vMinPosition = dynamic_cast<CBuildingContainer*>(pGamObject)->Get_BoundingBox_Min();
			_float3 vMaxPosition = dynamic_cast<CBuildingContainer*>(pGamObject)->Get_BoundingBox_Max();

			tinyxml2::XMLElement* BoundingBox_Min = doc.NewElement("BoundingBox_Min");
			BoundingBox_Min->SetAttribute("x", vMinPosition.x);
			BoundingBox_Min->SetAttribute("y", vMinPosition.y);
			BoundingBox_Min->SetAttribute("z", vMinPosition.z);
			container->InsertEndChild(BoundingBox_Min);

			tinyxml2::XMLElement* BoundingBox_Max = doc.NewElement("BoundingBox_Max");
			BoundingBox_Max->SetAttribute("x", vMaxPosition.x);
			BoundingBox_Max->SetAttribute("y", vMaxPosition.y);
			BoundingBox_Max->SetAttribute("z", vMaxPosition.z);
			container->InsertEndChild(BoundingBox_Max);
#pragma endregion


#pragma region SAVE_PARTOBJECT

			if (FAILED(pContainerObject->Save_PartObjects(doc, container)))
				return E_FAIL;
#pragma endregion

#pragma endregion
		}
	}
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

			_float3 vRotation = pElement->Get_Rotation();

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
#pragma endregion // PROTOTYPETAG

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
#pragma endregion // TRANSFORM

		}
	}
#pragma endregion // ELEMENT_INTERACTABEL


	// ?ú?å
	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
		MSG_BOX("Failed to Save File");
	}
	else
	{
#ifndef 기무리
		MSG_BOX("Succeed to Save File");
#endif // !기무리
	}

	return S_OK;
}

HRESULT CMapObject_Manager::Save_StaticElements(const _char* pFileName)
{
	tinyxml2::XMLDocument doc;
	string strPath = "../Bin/Resources/Data/Map/" + string(pFileName) + ".xml";

	tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

	doc.Clear();
	doc.InsertFirstChild(doc.NewDeclaration());

	tinyxml2::XMLElement* land = doc.NewElement("Land");
	doc.InsertEndChild(land);

#pragma region ELEMENT_STATIC
	 CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Static"));

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
			_float3 vRotation = pElement->Get_Rotation();

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

	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
		MSG_BOX("Failed to Save File");
	}

	return S_OK;
}

HRESULT CMapObject_Manager::Save_ContainerData(const _char* pFileName, const _char* pContainerName)
{
	if (true == m_MapObjects.empty()&& true == m_Collision.empty())
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

		/* Set Render Type*/
		_wstring ProrotypeTag = pMapObject->Get_PrototypeTag(0);

		if(_wstring::npos != ProrotypeTag.find(L"decal") || _wstring::npos != ProrotypeTag.find(L"Decal"))
			PartObject->SetAttribute("Render_Type", 1);
		else if(_wstring::npos != ProrotypeTag.find(L"glass") || _wstring::npos != ProrotypeTag.find(L"Glass"))
			PartObject->SetAttribute("Render_Type", 2);
		else
			PartObject->SetAttribute("Render_Type", 0);

		container->InsertEndChild(PartObject);

		for (_uint i = 0; i < pMapObject->Get_LodLevel() + 1; ++i)
		{
			// <PrototypeTag>ProrotypeTag</PrototypeTag>
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

		_float3 vRotation = pMapObject->Get_Rotation();

		RotationElem->SetAttribute("x", vRotation.x);
		RotationElem->SetAttribute("y", vRotation.y);
		RotationElem->SetAttribute("z", vRotation.z);
		PartObject->InsertEndChild(RotationElem);

		SAFE_RELEASE(pMapObject);
	}
	m_MapObjects.clear();
	m_pSelectObject = nullptr;
#pragma endregion

#pragma region SAVE_COLLISION
	for (auto& pColObject : m_Collision)
	{
		// <Collision>
		tinyxml2::XMLElement* Collision = doc.NewElement("Collision");
		_uint iLodLvel = pColObject->Get_LodLevel();

		Collision->SetAttribute("Lod_Level", iLodLvel);
		Collision->SetAttribute("Key_Index", pColObject->Get_iKeyIndex());
		container->InsertEndChild(Collision);

		for (_uint i = 0; i < pColObject->Get_LodLevel() + 1; ++i)
		{
			// <PrototypeTag>ÅØ½ºÆ®</PrototypeTag>
			tinyxml2::XMLElement* proto = doc.NewElement("PrototypeTag");
			proto->SetText(CMyTools::ToString(pColObject->Get_PrototypeTag(i)).c_str());
			Collision->InsertEndChild(proto);
		}

		// <Position x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* PositionElem = doc.NewElement("Position");

		_float3 vPosition = {};
		XMStoreFloat3(&vPosition, pColObject->Get_Component<CTransform>()->Get_State(STATE::POSITION));

		PositionElem->SetAttribute("x", vPosition.x);
		PositionElem->SetAttribute("y", vPosition.y);
		PositionElem->SetAttribute("z", vPosition.z);
		Collision->InsertEndChild(PositionElem);

		// <Scale x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* ScaleElem = doc.NewElement("Scale");

		_float3 pScale = pColObject->Get_Component<CTransform>()->Get_Scale();
		ScaleElem->SetAttribute("x", pScale.x);
		ScaleElem->SetAttribute("y", pScale.y);
		ScaleElem->SetAttribute("z", pScale.z);
		Collision->InsertEndChild(ScaleElem);

		// <Rotation x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* RotationElem = doc.NewElement("Rotation");

		_float3 vRotation = {};
		XMStoreFloat3(&vRotation, pColObject->Get_Component<CTransform>()->Get_RollPitchYawVector());

		vRotation.x = XMConvertToDegrees(vRotation.x);
		vRotation.y = XMConvertToDegrees(vRotation.y);
		vRotation.z = XMConvertToDegrees(vRotation.z);

		RotationElem->SetAttribute("x", vRotation.x);
		RotationElem->SetAttribute("y", vRotation.y);
		RotationElem->SetAttribute("z", vRotation.z);
		Collision->InsertEndChild(RotationElem);

		SAFE_RELEASE(pColObject);
	}
	m_Collision.clear();
#pragma endregion


	// ÀúÀå
	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
		MSG_BOX("Failed to Save File");
	}
	else
	{
#ifndef 기무리
		MSG_BOX("Succeed to Save File");
#endif // !기무리
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
			m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Interactable>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Interactable"), &Desc);
	}

	return S_OK;
}

HRESULT CMapObject_Manager::Load_MapData(const _char* pFileName, const _wchar* pLayerTag)
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
			g_iStaticLevel, ENUM_CLASS(LEVEL::MAP), pLayerTag, &ContainerDesc, nullptr, &pContainerObject);
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

		if (MAPOBJECT_TYPE::ELEMENT_STATIC == eType)
			m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Static>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Static"), &Desc);
		else if (MAPOBJECT_TYPE::ELEMENT_INTERACT == eType)
			m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Interactable>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Interactable"), &Desc);
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

			CMapObject_LOD* pMapObjectLOD = { nullptr };

			if (FAILED(pContainerObject->Add_Part<CMapObject_LOD>(strKey, g_iStaticLevel, &pMapObjectLOD, &Desc)))
			{
				MSG_BOX("Failed to Add PartObject");
				return E_FAIL;
			}
			pMapObjectLOD->Set_KeyIndex(iKeyIndex);

			SAFE_RELEASE(pMapObjectLOD);
#pragma endregion

	}
#pragma endregion

#pragma region ADD_COLLISION
	for (auto* Collision = TargetContainer->FirstChildElement("Collision"); Collision; Collision = Collision->NextSiblingElement("Collision"))
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

	}
#pragma endregion


	return S_OK;
}

HRESULT CMapObject_Manager::Load_ContainerToMapObject(const _char* pFileName, const _char* pContainerName)
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
		Desc.pParentTransform = m_pTransformCom;
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

		CMapObject_LOD* pMapObject = { nullptr };

		pMapObject = m_pGameInstance->Clone_Prototype<CMapObject_LOD>(g_iStaticLevel, &Desc);
		pMapObject->Set_KeyIndex(Get_KeyCount(CMyTools::ToWstring(strKey)));

		m_MapObjects.push_back(pMapObject);

#pragma endregion

	}
#pragma endregion

#pragma region ADD_COLLISION
	for (auto* Collision = TargetContainer->FirstChildElement("Collision"); Collision; Collision = Collision->NextSiblingElement("Collision"))
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
		Desc.pParentTransform = m_pTransformCom;

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

		CMapObject_Collision* pMapObject = { nullptr };

		pMapObject = m_pGameInstance->Clone_Prototype<CMapObject_Collision>(g_iStaticLevel, &Desc);

		if (nullptr != pMapObject)
			m_Collision.push_back(pMapObject);
#pragma endregion

	}
#pragma endregion


	return S_OK;
}

HRESULT CMapObject_Manager::Save_LightObject(const _char* pFileName)
{
	CLayer* pLayer;

	pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Light"));

	if (nullptr == pLayer)
		return S_OK;

	tinyxml2::XMLDocument doc;
	string strPath = "../Bin/Resources/Data/Map/Light/" + string(pFileName) + ".xml";

	tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

	doc.Clear();
	doc.InsertFirstChild(doc.NewDeclaration());

	tinyxml2::XMLElement* root = doc.NewElement("MapLightObjects");
	doc.InsertEndChild(root);

	if (nullptr != pLayer)
	{
		const list<CGameObject*>* pList = pLayer->Get_Objects();

		for (auto pGamObject : *pList)
		{
			CMapElement* pElement = dynamic_cast<CMapElement*>(pGamObject);

			if (nullptr == pElement)
				return E_FAIL;

			if (FAILED(pElement->Save_XML(doc, root)))
				return E_FAIL;
		}
	}

	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
		MSG_BOX("Failed to Save File");
	}
	else
	{
#ifndef 기무리
		MSG_BOX("Succeed to Save File");
#endif // !기무리
	}

	return S_OK;
}
HRESULT CMapObject_Manager::Load_LightObject(const _char* pFileName)
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/Light/" + string(pFileName) + ".xml";

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
		CMapElement_Light::MAPELEMENT_LIGHT_DESC Desc = {};

		/* Model Prototypes */
		Object->QueryUnsignedAttribute("Lod_Level", &Desc.iMaxLodLevel);

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

		/* Light Desc */
		auto* Diffuse = Object->FirstChildElement("Diffuse");
		Diffuse->QueryFloatAttribute("x", &Desc.vDiffuse.x);
		Diffuse->QueryFloatAttribute("y", &Desc.vDiffuse.y);
		Diffuse->QueryFloatAttribute("z", &Desc.vDiffuse.z);

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
		Info->QueryUnsignedAttribute("GlassIndex", &Desc.iGlassMeshIndex);
		Info->QueryFloatAttribute("BloomStrength", &Desc.fBloomStregth);
		Info->QueryFloatAttribute("Range", &Desc.fRange);
		_uint iPow = {};
		Info->QueryUnsignedAttribute("Pow", &iPow);

		Desc.isPow = 0 == iPow ? false : true;
	
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Light>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Light"), &Desc)))
			return E_FAIL;
	}
#ifndef 기무리
	MSG_BOX("Successed to Load File");
#endif

	return S_OK;
}
HRESULT CMapObject_Manager::Save_InteractObject(const _char* pFileName)
{
	CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Interactable"));

	if (nullptr == pLayer)
		return S_OK;

	tinyxml2::XMLDocument doc;
	string strPath = "../Bin/Resources/Data/Map/Interactable/" + string(pFileName) + ".xml";

	tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

	doc.Clear();
	doc.InsertFirstChild(doc.NewDeclaration());

	tinyxml2::XMLElement* Element_Interactable = doc.NewElement("Element_Interactable");
	doc.InsertEndChild(Element_Interactable);

#pragma region ELEMENT_INTERACTABEL

	if (nullptr != pLayer)
	{
		const list<CGameObject*>* pList = pLayer->Get_Objects();

		for (auto pGamObject : *pList)
		{
			CMapElement* pElement = dynamic_cast<CMapElement*>(pGamObject);

			if (nullptr == pElement)
				return E_FAIL;

			if (FAILED(pElement->Save_XML(doc, Element_Interactable)))
				return E_FAIL;
		}
	}
#pragma endregion // ELEMENT_INTERACTABEL

	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
		MSG_BOX("Failed to Save File");
	}


	return S_OK;
}
HRESULT CMapObject_Manager::Load_InteractObject(const _char* pFileName)
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/Interactable/" + string(pFileName) + ".xml";

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
		Object->QueryUnsignedAttribute("Lod_Level", &Desc.iMaxLodLevel);
		Object->QueryUnsignedAttribute("Lod_Level", &Desc.iInteractableID);
		
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

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Interactable>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Interactable_Test"), &Desc)))
			return E_FAIL;
	}
	//MSG_BOX("Successed to Create Interactalbe Element");

	return S_OK;
}
HRESULT CMapObject_Manager::Load_WaterObject(const _char* pFileName)
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/Water/" + string(pFileName) + ".xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("MapObjects_Water");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	for (auto* Object = root->FirstChildElement("Object"); Object; Object = Object->NextSiblingElement("Object"))
	{
		CMapElement_Lake::MAPOBJECT_LAKE_DESC Desc = {};

		/* Model Prototypes */
		Object->QueryUnsignedAttribute("Lod_Level", &Desc.iMaxLodLevel);

		string strTag = {};
		for (auto* PrototypeTag = Object->FirstChildElement("PrototypeTag"); PrototypeTag; PrototypeTag = PrototypeTag->NextSiblingElement("PrototypeTag"))
		{
			strTag = PrototypeTag->GetText();

			Desc.ModelPrototypeTags.push_back(CMyTools::ToWstring(strTag));
		}
		for (auto* PrototypeTag = Object->FirstChildElement("ShollowPrototypeTag"); PrototypeTag; PrototypeTag = PrototypeTag->NextSiblingElement("ShollowPrototypeTag"))
		{
			strTag = PrototypeTag->GetText();

			Desc.ShallowModelPrototypeTags.push_back(CMyTools::ToWstring(strTag));
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


		auto* TimeSpeed = Object->FirstChildElement("TimeSpeed");
		TimeSpeed->QueryFloatAttribute("value", &Desc.fTimeSpeed);

		auto* RefractionStrength = Object->FirstChildElement("RefractionStrength");
		RefractionStrength->QueryFloatAttribute("value", &Desc.fRefractionStrength);

		auto* RefractionPow = Object->FirstChildElement("RefractionPow");
		RefractionPow->QueryFloatAttribute("value", &Desc.fRefractionPow);

		auto* UVValue1 = Object->FirstChildElement("UVValue1");
		UVValue1->QueryFloatAttribute("value", &Desc.fUVValue1);

		auto* UVValue2 = Object->FirstChildElement("UVValue2");
		UVValue2->QueryFloatAttribute("value", &Desc.fUVValue2);

		auto* UVValue3 = Object->FirstChildElement("UVValue3");
		UVValue3->QueryFloatAttribute("value", &Desc.fUVValue3);

		// float2 값들
		auto* UVSpeed = Object->FirstChildElement("UVSpeed");
		UVSpeed->QueryFloatAttribute("x", &Desc.vUVSpeed.x);
		UVSpeed->QueryFloatAttribute("y", &Desc.vUVSpeed.y);

		auto* LargeUVSpeed = Object->FirstChildElement("LargeUVSpeed");
		LargeUVSpeed->QueryFloatAttribute("x", &Desc.vLargeUVSpeed.x);
		LargeUVSpeed->QueryFloatAttribute("y", &Desc.vLargeUVSpeed.y);

		auto* SubUVSpeed3 = Object->FirstChildElement("SubUVSpeed3");
		SubUVSpeed3->QueryFloatAttribute("x", &Desc.vSubUVSpeed3.x);
		SubUVSpeed3->QueryFloatAttribute("y", &Desc.vSubUVSpeed3.y);

		// float4 값들
		auto* RefractionColor = Object->FirstChildElement("RefractionColor");
		RefractionColor->QueryFloatAttribute("x", &Desc.vRefractionColor.x);
		RefractionColor->QueryFloatAttribute("y", &Desc.vRefractionColor.y);
		RefractionColor->QueryFloatAttribute("z", &Desc.vRefractionColor.z);
		RefractionColor->QueryFloatAttribute("w", &Desc.vRefractionColor.w);

		auto* SurfaceColor = Object->FirstChildElement("SurfaceColor");
		SurfaceColor->QueryFloatAttribute("x", &Desc.vSurfaceColor.x);
		SurfaceColor->QueryFloatAttribute("y", &Desc.vSurfaceColor.y);
		SurfaceColor->QueryFloatAttribute("z", &Desc.vSurfaceColor.z);
		SurfaceColor->QueryFloatAttribute("w", &Desc.vSurfaceColor.w);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Lake>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Lake"), &Desc)))
			return E_FAIL;
	}
	//MSG_BOX("Successed to Create Interactalbe Element");

	return S_OK;
}

HRESULT CMapObject_Manager::Save_PointLightObject(const _char* pFileName)
{
	CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_LightSpawer"));

	if (nullptr == pLayer)
		return S_OK;

	tinyxml2::XMLDocument doc;
	string strPath = "../Bin/Resources/Data/Map/Light/" + string(pFileName) + ".xml";

	tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

	doc.Clear();
	doc.InsertFirstChild(doc.NewDeclaration());

	tinyxml2::XMLElement* root = doc.NewElement("MapLightObjects");
	doc.InsertEndChild(root);

	if (nullptr != pLayer)
	{
		const list<CGameObject*>* pList = pLayer->Get_Objects();

		for (auto pGamObject : *pList)
		{
			CLightSpawner* pLightSpawner = dynamic_cast<CLightSpawner*>(pGamObject);

			if (nullptr == pLightSpawner)
				return E_FAIL;

			if (FAILED(pLightSpawner->Save_XML(doc, root)))
				return E_FAIL;
		}
	}

	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
		MSG_BOX("Failed to Save File");
	}
	else
	{
		MSG_BOX("Succeed to Save PointLightObjects");
	}

	return S_OK;
}

void CMapObject_Manager::Update_PrototypeList()
{
	GUI::Begin("Model Prototype List");

	_uint iMaxSize = (_uint)m_ModelPrototypeTags.size();
	for (_uint i = 0; i < iMaxSize; ++i)
	{
		auto& Tag = m_ModelPrototypeTags[i];

		if (_wstring::npos != Tag.find(L"Lod") || _wstring::npos != Tag.find(L"COL")) {
			continue;
		}

		if (ImGui::Button(CMyTools::ToString(Tag).c_str()))
		{
			if (ADD_TYPE::CONTAINER == m_eType)
				Create_PartObject(Tag);
			else
				Create_Elemnt(Tag);
		}
	}
	GUI::End();
	
	/* COL Mesh를 Container에 추가 */
	if (ADD_TYPE::CONTAINER == m_eType)
	{
		GUI::Begin("Collision Model");
		_uint iMaxSize = (_uint)m_ModelPrototypeTags.size();
		for (_uint i = 0; i < iMaxSize; ++i)
		{
			auto& Tag = m_ModelPrototypeTags[i];

			if (_wstring::npos != Tag.find(L"COL") && _wstring::npos == Tag.find(L"Lod")) {
				if (ImGui::Button(CMyTools::ToString(Tag).c_str()))
				{
					vector<_uint> LodModelIndices;

					MAPOBJECT_LOD_DESC Desc = {};

					vector<_wstring> PrototypeTags;

					Find_Lod_Prototype(Tag, LodModelIndices);

					PrototypeTags.push_back(Tag);

					for (_uint i = 0; i < LodModelIndices.size(); ++i)
					{
						_wstring strLodTag = Tag + L"_Lod" + to_wstring(i + 1);
						PrototypeTags.push_back(strLodTag);
					}

					Desc.iMaxLodLevel = (_uint)LodModelIndices.size() - 1;
					Desc.ModelPrototypeTags = PrototypeTags;
					Desc.pParentTransform = m_pTransformCom;
					Desc.vPosition = _float3(0.f, 0.f, 0.f);
					Desc.vRotation = _float3(0.f, 0.f, 0.f);
					Desc.vScale = _float3(1.f, 1.f, 1.f);
					Desc.pModelPathIndices = &LodModelIndices;

					CMapObject_Collision* pMapObject = { nullptr };
					const string strKey = CMyTools::ToString(Tag);

					pMapObject = m_pGameInstance->Clone_Prototype<CMapObject_Collision>(g_iStaticLevel, &Desc);

					if(nullptr != pMapObject)
						m_Collision.push_back(pMapObject);
				}
			}

			
		}
		GUI::End();
	}
}

void CMapObject_Manager::Update_ObjectList()
{
	CLayer* pLayer = { nullptr };

	//m_pMapObjects = pLayer->Get_Objects();

	GUI::Begin("MapObject List");

	_uint iObjectIndex = {};
	string strSrc;

	if (ADD_TYPE::CONTAINER == m_eType)
	{
		if (ImGui::CollapsingHeader("Map Objects", ImGuiTreeNodeFlags_DefaultOpen))
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
		if (ImGui::CollapsingHeader("COL", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto pMapObject : m_Collision)
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
			pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Interactable"));
		else if (ADD_TYPE::ELEMENT_LIGHT == m_eType)
			pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Element_Light"));
		
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

	if (GUI::Button("Set Box"))
	{
		if (false == (*pList).empty())
		{
			for (auto& pObject : (*pList))
			{
				dynamic_cast<CBuildingContainer*>(pObject)->Set_BoundingBox();
			}
		}
	}

	if (false == (*pList).empty())
	{
		CGameObject* pSelectObject = { nullptr };

		for (auto& pObject : (*pList))
		{
			if (GUI::Button((dynamic_cast<CMapContainer*>(pObject)->Get_Name() + to_string(iIndex)).c_str()))
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

void CMapObject_Manager::Update_Unified()
{
	GUI::Begin("Unifieds");
	CLayer * pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Unified"));

	if (nullptr != pLayer)
	{
		for (auto& pUnified : *pLayer->Get_Objects())
		{
			pUnified->Describe_Entity();
		}
	}
		
	GUI::End();
}

void CMapObject_Manager::Update_LightSpawer()
{
	GUI::Begin("Light Spawer");
	if (GUI::Button("Save Lights"))
	{
		Save_PointLightObject("Duengon_PointLight_Data");
	}
	if (GUI::Button("Load Lights"))
	{
		m_pInfoInstance->Load_PointLights("Duengon_PointLight_Data");
	}
	if(GUI::Button("Add Light Spawer"))
	{
		m_pGameInstance->Add_GameObject_ToLayer<CLightSpawner>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_LightSpawer"));
	}
	
	GUI::InputInt("Index : ", (_int*)&m_iSelectedIndex);
	CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_LightSpawer"));

	if(nullptr != pLayer)
	{
		_uint iIndex = {};
		for (auto& pLightSpawner : *pLayer->Get_Objects())
		{
			if(m_iSelectedIndex == iIndex)
				pLightSpawner->Describe_Entity();

			++iIndex;
		}
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

	for (_uint i = 0; i < LodModelIndices.size() - 1; ++i)
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

void CMapObject_Manager::Create_Elemnt(_wstring& strPrototypeTag)
{
	MAPOBJECT_LOD_DESC Desc = {};
	CMapElement_Light::MAPELEMENT_LIGHT_DESC Light_Desc = {};
	CMapElement_Interactable::ELEMENT_INTERACTABLE_DESC Interactable_Desc = {};
	vector<_wstring> PrototypeTags;
	vector<_uint> LodModelIndices;

	Find_Lod_Prototype(strPrototypeTag, LodModelIndices);

	PrototypeTags.push_back(strPrototypeTag);

	for (_uint i = 0; i < LodModelIndices.size(); ++i)
	{
		_wstring strLodTag = strPrototypeTag + L"_Lod" + to_wstring(i + 1);
		PrototypeTags.push_back(strLodTag);
	}

	Interactable_Desc.iMaxLodLevel = Light_Desc.iMaxLodLevel = Desc.iMaxLodLevel = (_uint)LodModelIndices.size() - 1;
	Interactable_Desc.ModelPrototypeTags = Light_Desc.ModelPrototypeTags = Desc.ModelPrototypeTags = PrototypeTags;
	Interactable_Desc.pParentTransform = Light_Desc.pParentTransform = Desc.pParentTransform = m_pTransformCom;
	Interactable_Desc.vPosition = Light_Desc.vPosition = Desc.vPosition = _float3(0.f, 0.f, 0.f);
	Interactable_Desc.vRotation = Light_Desc.vRotation = Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Interactable_Desc.vScale = Light_Desc.vScale = Desc.vScale = _float3(1.f, 1.f, 1.f);
	Interactable_Desc.pModelPathIndices = Light_Desc.pModelPathIndices  =Desc.pModelPathIndices = &LodModelIndices;

	if (ADD_TYPE::ELEMENT_STATIC == m_eType)
	{
		m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Static>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Static"), &Desc);
	}
	else if (ADD_TYPE::ELEMENT_INTERACT == m_eType)
	{
		Interactable_Desc.iInteractableID = 1; 
		Interactable_Desc.vBoxSize = _float3(0.3f, 0.33f, 0.27f);
		Interactable_Desc.vBoxLocalPosition = _float3(-0.01f, 0.455f, 0.f);
		

		m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Interactable>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Interactable"), &Interactable_Desc);
	}
	else if (ADD_TYPE::ELEMENT_LIGHT == m_eType)
	{
		Light_Desc.isPow = false;
		Light_Desc.iGlassMeshIndex = m_iGlassIndex;
		Light_Desc.fBloomStregth = 4.5f;
		Light_Desc.fRange = 7.f;
		Light_Desc.vDiffuse = _float4(1.f, 0.6f, 0.2f, 1.f);
		Light_Desc.vAmbient = _float4(1.f, 0.64f, 0.2f, 1.f);
		Light_Desc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
		Light_Desc.vPosOffset = _float4(0.f, 3.3f, 0.f, 1.f);
		
		CMapElement_Light* pElement = { nullptr };

		m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Light>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Element_Light"), &Light_Desc, nullptr, &pElement);

		m_pSelectElemnt = pElement;
	}
	
}

_bool CMapObject_Manager::Find_Lod_Prototype(_wstring strPrototypeTag, vector<_uint>& LodModelIndices)
{
	// for (auto& Tag : m_LODModelPrototypeTags)
	for(_uint i = 0 ; i < m_ModelPrototypeTags.size(); ++i)
	{
		if (m_ModelPrototypeTags[i] == strPrototypeTag)
			LodModelIndices.push_back(i);
		 
		_wstring strPt = strPrototypeTag + L"_Lod";
		if (_wstring::npos != m_ModelPrototypeTags[i].find(strPt)) //&& 5 > (m_ModelPrototypeTags[i].size() - strPrototypeTag.size()))
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
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject_Manager::Clone(void* pArg, CGameObject* pOwner)
{
	CMapObject_Manager* pInstance = new CMapObject_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapObject_Manager::Free()
{
	__super::Free();

	m_ModelPrototypeTags.clear();
	m_PartObjectKeyCount.clear();
	m_LODModelPrototypeTags.clear();
	m_ModelPrototypePaths.clear();

	for (auto& pObject : m_MapObjects)
	{
		SAFE_RELEASE(pObject);
	}
	m_MapObjects.clear();

	for (auto& pObject : m_Collision)
	{
		SAFE_RELEASE(pObject);
	}
	m_Collision.clear();

	SAFE_RELEASE(m_pContainer);

	SAFE_RELEASE(m_pInfoInstance);
}

void CMapObject_Manager::Describe_Entity()
{
}
