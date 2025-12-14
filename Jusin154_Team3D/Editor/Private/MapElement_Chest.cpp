#include "pch.h"
#include "MapElement_Chest.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "Layer.h"
#include "MapElement_Chest_Lid.h"
#include "Player.h"

CMapElement_Chest::CMapElement_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Chest::CMapElement_Chest(const CMapElement_Chest& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Chest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Chest::Initialize(void* pArg)
{
	ELEMENT_CHEST_DESC* pDesc = static_cast<ELEMENT_CHEST_DESC*>(pArg);

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

	if (FAILED(Ready_Lid(pDesc)))
		return E_FAIL;

	m_eCurState = m_ePreState = CHEST_STATE::IDLE;

#ifdef _DEBUG
	m_bSelected = false;
	m_vPosition = pDesc->vPosition;
	m_vScale = pDesc->vScale;
	m_vRotation = pDesc->vRotation;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
#endif // _DEBUG

	m_vBoxSize = _float3(1.f, 1.f, 1.f);
	m_pActor = static_cast<PSX::PxRigidDynamic*>(m_pRigidBody->Get_Actor());

	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Set_HalfGeometryInfo(pDesc->vBoxSize);
	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Move_LocalPos(_float4(0.f, 0.f, 0.f, 0.f), pDesc->vBoxLocalPosition);

	return S_OK;
}

void CMapElement_Chest::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif    

	m_pLid->Priority_Update(fTimeDelta);
}

void CMapElement_Chest::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	/*if(CHEST_STATE::OPENED != m_eCurState)
	{
		_vector vPlayerPos = m_pGameInstance->Get_Layer(g_iStaticLevel, LAYER_PLAYER)->Get_Object<CPlayer>()->Get_WorldPostion();

		_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - m_pTransformCom->Get_State(STATE::POSITION)));

		if (CHEST_STATE::IDLE == m_eCurState && fDistance < 10.f)
			m_eCurState = CHEST_STATE::FOUND;
		else if (fDistance < 1.5f)
			m_eCurState = CHEST_STATE::OPENED;
	}*/
#endif // _DEBUG

	switch (m_eCurState)
	{
	case Editor::CMapElement_Chest::CHEST_STATE::IDLE:
		break;

	case Editor::CMapElement_Chest::CHEST_STATE::FOUND:
		m_fTimeAcc += fTimeDelta * 0.03f;

		if (0.f <= m_fTimeAcc && 0.06f > m_fTimeAcc)
		{
			m_fCurRimLightStrength = m_fRimLightStrength * m_fTimeAcc / 0.06f;
		}
		else if (0.3f <= m_fTimeAcc)
		{
			m_fTimeAcc = 0.f;
			m_fCurRimLightStrength = 0.f;
			m_eCurState = CHEST_STATE::IDLE;
		}
		else
		{
			m_fCurRimLightStrength = m_fRimLightStrength;
		}
		break;

	case Editor::CMapElement_Chest::CHEST_STATE::OPENED:
		break;

	default:
		break;
	}

	Chage_State();

	m_pLid->Update(fTimeDelta);
}

void CMapElement_Chest::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
	m_pLid->Describe_Entity();
#endif 

	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius())) {

		if(CMapElement_Chest::CHEST_STATE::FOUND ==  m_eCurState)
		{
			m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
			m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
		}
		else
			m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

		m_pLid->Late_Update(fTimeDelta);
	}
}

HRESULT CMapElement_Chest::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iShaderPass_Index))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	if (FAILED(m_pRigidBody->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG

	if (FAILED(m_pLid->Render(m_iShaderPass_Index)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapElement_Chest::Ready_Components(void* pArg)
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

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Levioso_Noise"),
		reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;

	ELEMENT_CHEST_DESC* pPhysXDummyDesc = static_cast<ELEMENT_CHEST_DESC*>(pArg);

	// RIGID_BODY
	CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
	Desc.iSubKind = ENUM_CLASS(PXOBJECT::BOX);//pPhysXDummyDesc->iSubKind;
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMapElement_Chest::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimLightPower, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimStrength", &m_fCurRimLightStrength, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimLightColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_SRV("g_NoiseTexture", m_pNoiseTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CMapElement_Chest::Ready_Lid(ELEMENT_CHEST_DESC* pArg)
{
	m_pLid = m_pGameInstance->Clone_Prototype<CMapElement_Chest_Lid>(g_iStaticLevel, pArg, this);

	if (nullptr == m_pLid)
		return E_FAIL;

	return S_OK;
}

void CMapElement_Chest::Chage_State()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Editor::CMapElement_Chest::CHEST_STATE::IDLE:
			m_fCurRimLightStrength = 0.f;
			m_fTimeAcc = 0.f;
			m_iShaderPass_Index = ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);
			break;

		case Editor::CMapElement_Chest::CHEST_STATE::FOUND:
			m_iShaderPass_Index = 21; //ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);
			break;

		case Editor::CMapElement_Chest::CHEST_STATE::OPENED:
			m_iShaderPass_Index = ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);
			m_pLid->Open();
			break;

		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CMapElement_Chest* CMapElement_Chest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Chest* pInstance = new CMapElement_Chest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Chest");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Chest::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Chest* pInstance = new CMapElement_Chest(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Chest");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Chest::Free()
{
	if (nullptr != m_pRigidBody) {
		m_pGameInstance->Release_Actor(*m_pRigidBody->Get_Actor());
	}
	__super::Free();

	if (m_bCloned)
		SAFE_RELEASE(m_pLid);

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pNoiseTextureCom);
	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

void CMapElement_Chest::Describe_Entity()
{
	if (m_bDead)
		return;
	if (nullptr == m_pGameInstance)
		return;

	PSX::PxVec3 vDir = PSX::PxVec3(0.f, 1.f, 0.f);
	GUI::Begin("Chest");
	if (GUI::CollapsingHeader("Rigid Body"))
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
	}


	if (GUI::Button("Save_ChestData"))
	{
		tinyxml2::XMLDocument doc;
		string strPath = "../Bin/Resources/Data/Map/Hiden/Element_Chest_Info.xml";

		tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

		tinyxml2::XMLElement* root = {};

		if (loadResult == tinyxml2::XML_SUCCESS)
		{
			root = doc.FirstChildElement("Chests");

			Save_XML(doc, root);

			if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
				MSG_BOX("Failed to Save DoorInfo File");
			}
		}
		else
			MSG_BOX("Failed to Save Data");
	}

	GUI::Text(CMyTools::ToString(m_ModelPrototypeTags[m_iLodIndex]).c_str());
	GUI::InputInt("Lod Level", (_int*)(&m_iLodIndex));
	m_iLodIndex = max(0, m_iLodIndex);
	m_iLodIndex = min(m_iMaxLodLevel, m_iLodIndex);

	GUI::InputFloat("m_fRimLightPower", &m_fRimLightPower, 0.1f, 1.f);
	GUI::InputFloat("m_fRimLightStrength", &m_fRimLightStrength, 0.1f, 1.f);
	GUI::ColorEdit4("m_vRimLightColor", (_float*) & m_vRimLightColor);

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

	if (GUI::Button("IDLE"))
	{
		m_eCurState = CHEST_STATE::IDLE;
	}
	if (GUI::Button("FOUND"))
	{
		m_eCurState = CHEST_STATE::FOUND;
	}
	if (GUI::Button("OPENED"))
	{
		m_eCurState = CHEST_STATE::OPENED;
	}

	if (GUI::Button("Delete"))
		m_bDead = true;

	GUI::End();

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
}

HRESULT CMapElement_Chest::Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement* object = doc.NewElement("Object");
	object->SetAttribute("Type", ENUM_CLASS(MAPOBJECT_TYPE::ELEMENT_INTERACT));
	object->SetAttribute("ID", ENUM_CLASS(m_eInteractableID));
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

	_float3 vHalfGeometryInfo = static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Get_HalfGeometryInfo();
	tinyxml2::XMLElement* HalfGeometryInfo = doc.NewElement("HalfGeometryInfo");
	HalfGeometryInfo->SetAttribute("x", vHalfGeometryInfo.x);
	HalfGeometryInfo->SetAttribute("y", vHalfGeometryInfo.y);
	HalfGeometryInfo->SetAttribute("z", vHalfGeometryInfo.z);
	object->InsertEndChild(HalfGeometryInfo);

	_float3 vLocalTranslation = static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Get_LocalTranslation();
	tinyxml2::XMLElement* LocalTranslation = doc.NewElement("LocalTranslation");
	LocalTranslation->SetAttribute("x", vLocalTranslation.x);
	LocalTranslation->SetAttribute("y", vLocalTranslation.y);
	LocalTranslation->SetAttribute("z", vLocalTranslation.z);
	object->InsertEndChild(LocalTranslation);

	tinyxml2::XMLElement* RimLight = doc.NewElement("RimLight");
	RimLight->SetAttribute("pow", m_fRimLightPower);
	RimLight->SetAttribute("strength", m_fRimLightStrength);
	RimLight->SetAttribute("colorR", m_vRimLightColor.x);
	RimLight->SetAttribute("colorG", m_vRimLightColor.y);
	RimLight->SetAttribute("colorB", m_vRimLightColor.z);
	RimLight->SetAttribute("colorA", m_vRimLightColor.w);
	object->InsertEndChild(RimLight);

#pragma endregion

	m_pLid->Save_XML(doc, object);

	return S_OK;
}
