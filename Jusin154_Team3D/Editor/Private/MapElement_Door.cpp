#include "pch.h"
#include "MapElement_Door.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"

CMapElement_Door::CMapElement_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Door::CMapElement_Door(const CMapElement_Door& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Door::Initialize(void* pArg)
{
	ELEMENT_DOOR_DESC* pDesc = static_cast<ELEMENT_DOOR_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

#ifdef _DEBUG
	m_isEdit = pDesc->isEdit;
	m_bSelected = false;
	m_vPosition = pDesc->vPosition;
	m_vScale = pDesc->vScale;
	m_vRotation = pDesc->vRotation;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
#endif // _DEBUG

	m_vBoxSize = _float3(1.f, 1.f, 1.f);/*
	m_pActor = static_cast<PSX::PxRigidDynamic*>(m_pRigidBody->Get_Actor());

	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Set_HalfGeometryInfo(pDesc->vBoxSize);
	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Move_LocalPos(_float4(0.f, 0.f, 0.f, 0.f), pDesc->vBoxLocalPosition);*/

	return S_OK;
}

void CMapElement_Door::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif    
}

void CMapElement_Door::Update(_float fTimeDelta)
{
	if (m_isEdit)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		m_pTransformCom->Set_Scale(m_vScale);
		m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	}
}

void CMapElement_Door::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if(m_isEdit)
		Describe_Entity();
#endif 

	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CMapElement_Door::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		if (m_bSelected && m_bUseSelectColor)
		{
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::MAPTOOL)))) {
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
				return E_FAIL;
			}
		}


		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	//if (FAILED(m_pRigidBody->Render())) {
	//	return E_FAIL;
	//}
#endif // _DEBUG

	return S_OK;
}

HRESULT CMapElement_Door::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_ModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	ELEMENT_DOOR_DESC* pPhysXDummyDesc = static_cast<ELEMENT_DOOR_DESC*>(pArg);

	//// RIGID_BODY
	//CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
	//Desc.iSubKind = ENUM_CLASS(PXOBJECT::BOX);//pPhysXDummyDesc->iSubKind;
	//if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
	//	return E_FAIL;
	//}


	return S_OK;
}

HRESULT CMapElement_Door::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}

	return S_OK;
}

CMapElement_Door* CMapElement_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Door* pInstance = new CMapElement_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Door");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Door::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Door* pInstance = new CMapElement_Door(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Door");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Door::Free()
{
	if (nullptr != m_pRigidBody) {
		m_pGameInstance->Release_Actor(*m_pRigidBody->Get_Actor());
	}
	__super::Free();

	//SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

void CMapElement_Door::Describe_Entity()
{
	if (m_bDead)
		return;
	if (nullptr == m_pGameInstance)
		return;

	GUI::Begin("Door");

	PSX::PxVec3 vDir = PSX::PxVec3(0.f, 1.f, 0.f);

	/*if (GUI::CollapsingHeader("Rigid Body"))
	{
		GUI::InputFloat("Power", &m_fPower);

		if (GUI::Button("Move Test"))
		{

			m_pActor = static_cast<PSX::PxRigidDynamic*>(m_pRigidBody->Get_Actor());
			if (nullptr != m_pActor)
			{
				m_pActor->wakeUp();
				m_pActor->addForce(vDir * m_fPower * 100000.f, PSX::PxForceMode::eFORCE);
			}
		}
		if (GUI::Button("OFF_Kinematic"))
		{
			m_pActor = static_cast<PSX::PxRigidDynamic*>(m_pRigidBody->Get_Actor());
			if (nullptr != m_pActor)
				m_pActor->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, false);
		}
		if (GUI::Button("ON_Kinematic"))
		{
			m_pActor = static_cast<PSX::PxRigidDynamic*>(m_pRigidBody->Get_Actor());
			if (nullptr != m_pActor)
				m_pActor->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, true);
		}

		if (GUI::InputFloat("Mass", &m_fMass))
			m_pActor->setMass(m_fMass);

		m_pRigidBody->Describe_Entity();
	}*/

	
	GUI::Text(CMyTools::ToString(m_ModelPrototypeTags[m_iLodIndex]).c_str());
	GUI::InputInt("Lod Level", (_int*)(&m_iLodIndex));
	m_iLodIndex = max(0, m_iLodIndex);
	m_iLodIndex = min(m_iMaxLodLevel, m_iLodIndex);

	if (GUI::Button("Save_DoorData"))
	{
		tinyxml2::XMLDocument doc;
		string strPath = "../Bin/Resources/Data/Map/Door/Element_Door_Info.xml";

		tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

		tinyxml2::XMLElement* root = {};

		if (loadResult == tinyxml2::XML_SUCCESS)
		{
			root = doc.FirstChildElement("Doors");
			Save_XML(doc, root);

			if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
				MSG_BOX("Failed to Save DoorInfo File");
			}
		}
		else
			MSG_BOX("Failed to Save Data");
	}

	GUI::Checkbox("Select Color ON / OFF", &m_bUseSelectColor);

	GUI::Text("----- Transfrom ----");
	_float3 vMove = {};
	GUI::InputFloat("Right", &vMove.x, 0.05f, 0.1f);
	GUI::InputFloat("Up", &vMove.y, 0.05f, 0.1f);
	GUI::InputFloat("Look", &vMove.z, 0.05f, 0.1f);

	m_pTransformCom->Move_Right(vMove.x);
	m_pTransformCom->Move_Up(vMove.y);
	m_pTransformCom->Move_Look(vMove.z);

	XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(STATE::POSITION));

	GUI::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));


	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			memcpy(&m_vPosition, &vPosition, sizeof(_float3));
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		}
	}

	GUI::Text("----- Rotation ----");
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 1.f, 15.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 1.f, 15.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 1.f, 15.f);

	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.05f, 0.1f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.05f, 0.1f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.05f, 0.1f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	if (GUI::Button("Delete"))
		m_bDead = true;

	GUI::End();

	//m_bSelected = true;
}

HRESULT CMapElement_Door::Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement* object = doc.NewElement("Object");
	object->SetAttribute("Lod_Level", m_iMaxLodLevel);
	root->InsertEndChild(object);

#pragma region PROTOTYPETAG
	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		tinyxml2::XMLElement* prototype = doc.NewElement("PrototypeTag");
		prototype->SetText(CMyTools::ToString(m_ModelPrototypeTags[i]).c_str());
		object->InsertEndChild(prototype);
	}
#pragma endregion

#pragma region TRANSFORM
	_float3 vPosition = {};
	XMStoreFloat3(&vPosition, m_pTransformCom->Get_State(STATE::POSITION));

	_float3 vScale = m_pTransformCom->Get_Scale();

	_float3 vRotation = {};
	XMStoreFloat3(&vRotation, m_pTransformCom->Get_RollPitchYawVector());
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

	//_float3 vHalfGeometryInfo = static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Get_HalfGeometryInfo();
	//tinyxml2::XMLElement* HalfGeometryInfo = doc.NewElement("HalfGeometryInfo");
	//HalfGeometryInfo->SetAttribute("x", vHalfGeometryInfo.x);
	//HalfGeometryInfo->SetAttribute("y", vHalfGeometryInfo.y);
	//HalfGeometryInfo->SetAttribute("z", vHalfGeometryInfo.z);
	//object->InsertEndChild(HalfGeometryInfo);

	//_float3 vLocalTranslation = static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Get_LocalTranslation();
	//tinyxml2::XMLElement* LocalTranslation = doc.NewElement("LocalTranslation");
	//LocalTranslation->SetAttribute("x", vLocalTranslation.x);
	//LocalTranslation->SetAttribute("y", vLocalTranslation.y);
	//LocalTranslation->SetAttribute("z", vLocalTranslation.z);
	//object->InsertEndChild(LocalTranslation);

#pragma endregion

	return S_OK;
}
