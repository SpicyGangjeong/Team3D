#include "pch.h"
#include "MapObject_Manager.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "Layer.h"
#include "MapObject.h"


CMapObject_Manager::CMapObject_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapObject_Manager::CMapObject_Manager(const CMapObject_Manager& Prototype)
	: CGameObject(Prototype)
	, m_ModelPrototypeTags{ Prototype.m_ModelPrototypeTags }
{
}

HRESULT CMapObject_Manager::Initialize_Prototype(vector<_wstring>& ModelPrototypeTags)
{
	m_ModelPrototypeTags = ModelPrototypeTags;

	return S_OK;
}

HRESULT CMapObject_Manager::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}

void CMapObject_Manager::Priority_Update(_float fTimeDelta)
{

}

void CMapObject_Manager::Update(_float fTimeDelta)
{
	GUI::Begin("Model Prototype List");

	for (auto& Tag : m_ModelPrototypeTags)
	{
		if (ImGui::Button(CMyTools::ToString(Tag).c_str()))
		{
			CMapObject::MAPOBJECT_DESC Desc = {};
			Desc.pModelPrototypeTag = Tag.c_str();

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapObject>(g_iStaticLevel, ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObject"), &Desc)))
			{
			};

		}
	}

	GUI::End();

	CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObject"));

	if (nullptr == pLayer)
		return;

	m_pMapObjects = pLayer->Get_Objects();

	GUI::Begin("MapObject List");

	_uint iObjectIndex = {};
	string strSrc;

	if (nullptr != m_pMapObjects)
	{
		for (auto pMapObject : (*m_pMapObjects))
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

	GUI::Begin("Edit");

	if (nullptr != m_pSelectObject)
	{
		m_pSelectObject->Describe_Entity();
		if (m_pSelectObject->isDead())
			m_pSelectObject = nullptr;
	}

	GUI::End();

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
	const list<CGameObject*>* pObjectList = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObject"))->Get_Objects();

	if (nullptr == pObjectList)
		return E_FAIL;

	tinyxml2::XMLDocument doc;

	doc.InsertFirstChild(doc.NewDeclaration());

	tinyxml2::XMLElement* land = doc.NewElement("Land");
	doc.InsertEndChild(land);

	for (auto& pObjcet : *pObjectList)
	{
		CMapObject* pMapObject = dynamic_cast<CMapObject*>(pObjcet);

		if (nullptr == pMapObject)
			continue;

		// <Model>
		tinyxml2::XMLElement* model = doc.NewElement("Model");
		land->InsertEndChild(model);

		// <PrototypeTag>텍스트</PrototypeTag>
		tinyxml2::XMLElement* proto = doc.NewElement("PrototypeTag");
		proto->SetText(CMyTools::ToString(pMapObject->Get_PrototypeTag()).c_str());
		model->InsertEndChild(proto);

		// <Position x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* PositionElem = doc.NewElement("Position");

		_float3 vPosition = {};
		XMStoreFloat3(&vPosition, pMapObject->Get_WorldPostion());
		
		PositionElem->SetAttribute("x", vPosition.x);
		PositionElem->SetAttribute("y", vPosition.y);
		PositionElem->SetAttribute("z", vPosition.z);
		model->InsertEndChild(PositionElem);

		// <Scale x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* ScaleElem = doc.NewElement("Scale");

		_float3 pScale = pMapObject->Get_Component<CTransform>()->Get_Scale();
		ScaleElem->SetAttribute("x", pScale.x);
		ScaleElem->SetAttribute("y", pScale.y);
		ScaleElem->SetAttribute("z", pScale.z);
		model->InsertEndChild(ScaleElem);

		// <Rotation x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* RotationElem = doc.NewElement("Rotation");

		_float3 vRotation = {};
		XMStoreFloat3(&vRotation, pMapObject->Get_Component<CTransform>()->Get_RollPitchYawVector());
		
		RotationElem->SetAttribute("x", vRotation.x);
		RotationElem->SetAttribute("y", vRotation.y);
		RotationElem->SetAttribute("z", vRotation.z);
		model->InsertEndChild(RotationElem);

	}
	string strPath = "../Bin/Resources/Data/" + string(pFileName) + ".xml";

	// 저장
	if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {

		return E_FAIL;
	}

	return S_OK;
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

}

void CMapObject_Manager::Describe_Entity()
{
}