#include "pch.h"
#include "MapObject_Manager.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "Layer.h"
#include "MapObject.h"
#include "MapObject_LOD.h"
#include "MapObject_Static.h"
#include "BuildingContainer.h"


CMapObject_Manager::CMapObject_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapObject_Manager::CMapObject_Manager(const CMapObject_Manager& rhs)
	: CGameObject(rhs)
	, m_ModelPrototypeTags{ rhs.m_ModelPrototypeTags }
	, m_LODModelPrototypeTags{ rhs.m_LODModelPrototypeTags }
{
}

HRESULT CMapObject_Manager::Initialize_Prototype(vector<_wstring>& ModelPrototypeTags)
{
	for (auto& Tag : ModelPrototypeTags)
	{
		if (_wstring::npos != Tag.find(L"Lod"))
		{
			m_LODModelPrototypeTags.push_back(Tag);
		}

		m_ModelPrototypeTags.push_back(Tag);

	}

	return S_OK;
}

HRESULT CMapObject_Manager::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pContainer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Building"))->Get_Object<CBuildingContainer>();

	SAFE_ADDREF(m_pContainer);

	return S_OK;
}

void CMapObject_Manager::Priority_Update(_float fTimeDelta)
{

}

void CMapObject_Manager::Update(_float fTimeDelta)
{
	GUI::Begin("File");

	if (GUI::Button("Save_XML"))
	{
		Save_MapData("Test");
	}

	GUI::End();

	Update_PrototypeList();

	Update_ObjectList();

	Update_Edit();

}

void CMapObject_Manager::Late_Update(_float fTimeDelta)
{

}

HRESULT CMapObject_Manager::Render()
{
	return S_OK;
}

HRESULT CMapObject_Manager::Ready_Components()
{
	return S_OK;
}

HRESULT CMapObject_Manager::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CMapObject_Manager::Save_MapData(const _char* pFileName)
{
	//const list<CGameObject*>* pObjectList = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObject"))->Get_Objects();

	//if (nullptr == pObjectList)
	//	return E_FAIL;

	//tinyxml2::XMLDocument doc;

	//doc.InsertFirstChild(doc.NewDeclaration());

	//tinyxml2::XMLElement* land = doc.NewElement("Land");
	//doc.InsertEndChild(land);

	//for (auto& pObjcet : *pObjectList)
	//{
	//	CMapObject* pMapObject = dynamic_cast<CMapObject*>(pObjcet);

	//	if (nullptr == pMapObject)
	//		continue;

	//	// <Model>
	//	tinyxml2::XMLElement* model = doc.NewElement("Model");
	//	land->InsertEndChild(model);

	//	// <PrototypeTag>텍스트</PrototypeTag>
	//	tinyxml2::XMLElement* proto = doc.NewElement("PrototypeTag");
	//	proto->SetText(CMyTools::ToString(pMapObject->Get_PrototypeTag()).c_str());
	//	model->InsertEndChild(proto);

	//	// <Position x="0.0" y="0.0" z="0.0"/>
	//	tinyxml2::XMLElement* PositionElem = doc.NewElement("Position");

	//	_float3 vPosition = {};
	//	XMStoreFloat3(&vPosition, pMapObject->Get_WorldPostion());
	//	
	//	PositionElem->SetAttribute("x", vPosition.x);
	//	PositionElem->SetAttribute("y", vPosition.y);
	//	PositionElem->SetAttribute("z", vPosition.z);
	//	model->InsertEndChild(PositionElem);

	//	// <Scale x="0.0" y="0.0" z="0.0"/>
	//	tinyxml2::XMLElement* ScaleElem = doc.NewElement("Scale");

	//	_float3 pScale = pMapObject->Get_Component<CTransform>()->Get_Scale();
	//	ScaleElem->SetAttribute("x", pScale.x);
	//	ScaleElem->SetAttribute("y", pScale.y);
	//	ScaleElem->SetAttribute("z", pScale.z);
	//	model->InsertEndChild(ScaleElem);

	//	// <Rotation x="0.0" y="0.0" z="0.0"/>
	//	tinyxml2::XMLElement* RotationElem = doc.NewElement("Rotation");

	//	_float3 vRotation = {};
	//	XMStoreFloat3(&vRotation, pMapObject->Get_Component<CTransform>()->Get_RollPitchYawVector());
	//	
	//	RotationElem->SetAttribute("x", vRotation.x);
	//	RotationElem->SetAttribute("y", vRotation.y);
	//	RotationElem->SetAttribute("z", vRotation.z);
	//	model->InsertEndChild(RotationElem);

	//}
	//string strPath = "../Bin/Resources/Data/Map/" + string(pFileName) + ".xml";

	//// 저장
	//if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
	//	return E_FAIL;
	//}

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
		return E_FAIL;
	}

	///* Map */
	//MAP_DATA Map_Data = {};

	//for (auto* land = root->FirstChildElement("Model"); land; land = land->NextSiblingElement("Model"))
	//{
	//	tinyxml2::XMLElement* protoElem = land->FirstChildElement("PrototypeTag");

	//	const _char* pPrototypeTag = protoElem->GetText();

	//	if (nullptr != pPrototypeTag)
	//	{
	//		string strPtotoypeTag = pPrototypeTag;
	//		Map_Data.strPrototypeTag = CMyTools::ToWstring(strPtotoypeTag);
	//	}
	//	else
	//	{
	//		Map_Data.strPrototypeTag = L"";
	//	}

	//	protoElem = land->FirstChildElement("Position");
	//	if (nullptr == protoElem)
	//		return E_FAIL;
	//	protoElem->QueryFloatAttribute("x", &Map_Data.vPosition.x);
	//	protoElem->QueryFloatAttribute("y", &Map_Data.vPosition.y);
	//	protoElem->QueryFloatAttribute("z", &Map_Data.vPosition.z);

	//	protoElem = land->FirstChildElement("Scale");
	//	if (nullptr == protoElem)
	//		return E_FAIL;
	//	protoElem->QueryFloatAttribute("x", &Map_Data.vScale.x);
	//	protoElem->QueryFloatAttribute("y", &Map_Data.vScale.y);
	//	protoElem->QueryFloatAttribute("z", &Map_Data.vScale.z);

	//	protoElem = land->FirstChildElement("Rotation");
	//	if (nullptr == protoElem)
	//		return E_FAIL;
	//	protoElem->QueryFloatAttribute("x", &Map_Data.vRotation.x);
	//	protoElem->QueryFloatAttribute("y", &Map_Data.vRotation.y);
	//	protoElem->QueryFloatAttribute("z", &Map_Data.vRotation.z);

	//	m_MapDatas.push_back(Map_Data);
	//}

	return S_OK;
}

void CMapObject_Manager::Update_PrototypeList()
{
	GUI::Begin("Model Prototype List");
	GUI::Checkbox("bLOD", &m_bLOD);
	for (auto& Tag : m_ModelPrototypeTags)
	{
		if (_wstring::npos != Tag.find(L"Lod"))
			continue;

		if (ImGui::Button(CMyTools::ToString(Tag).c_str()))
		{
			if(false == m_bLOD)
			{
				CMapObject_Static::MAPOBJECT_STATIC_DESC Desc = {};

				Desc.pModelPrototypeTag = Tag.c_str();
				Desc.pParentTransform = m_pContainer->Get_Component<CTransform>();

				CMapObject_Static* pMapObject = { nullptr };
				const string strKey = CMyTools::ToString(Tag);
				m_pContainer->Add_Part<CMapObject_Static>(strKey, 0, &pMapObject, &Desc);

				m_MapObjects.push_back(pMapObject);
			}
			else
			{
				CMapObject_LOD::MAPOBJECT_LOD_DESC Desc = {};

				vector<const _tchar*> PrototypeTags;

				PrototypeTags.push_back(Tag.c_str());
				Desc.iMaxLodLevel = 0;
				Desc.ModelPrototypeTags = PrototypeTags;
				Desc.pParentTransform = m_pContainer->Get_Component<CTransform>();

				CMapObject_Static* pMapObject = { nullptr };
				const string strKey = CMyTools::ToString(Tag);
				m_pContainer->Add_Part<CMapObject_LOD>(strKey, 0, nullptr, &Desc);

				m_MapObjects.push_back(pMapObject);
			}
		}
	}
	GUI::End();


	GUI::Begin("LOD Model Prototype List");
	for (auto& Tag : m_LODModelPrototypeTags)
	{
		if (ImGui::Button(CMyTools::ToString(Tag).c_str()))
		{
			if (nullptr != m_pSelectObject)
			{
				CMapObject_LOD* pLodObject = dynamic_cast<CMapObject_LOD*>(m_pSelectObject);

				if (nullptr != pLodObject)
				{
					pLodObject->Add_LodModel(Tag.c_str());
				}
				else
				{
					MSG_BOX("Failed to Add Lod Model");
				}

				if (m_pSelectObject->isDead())
					m_pSelectObject = nullptr;

			}
		}
	}
	GUI::End();
	
}

void CMapObject_Manager::Update_ObjectList()
{
	//CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObject"));

	//if (nullptr == pLayer)
	//	return;

	//m_pMapObjects = pLayer->Get_Objects();

	GUI::Begin("MapObject List");

	_uint iObjectIndex = {};
	string strSrc;

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

	GUI::End();
}

void CMapObject_Manager::Update_Edit()
{
	GUI::Begin("Edit");

	if (nullptr != m_pSelectObject)
	{
		m_pSelectObject->Describe_Entity();
		if (m_pSelectObject->isDead())
			m_pSelectObject = nullptr;

	}

	GUI::End();
}


CMapObject_Manager* CMapObject_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<_wstring>& ModelPrototypeTags)
{
	CMapObject_Manager* pInstance = new CMapObject_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(ModelPrototypeTags)))
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

	SAFE_RELEASE(m_pContainer);
}

void CMapObject_Manager::Describe_Entity()
{
}