#include "pch.h"
#include "MapElement_Lake.h"

#include "GameInstance.h"

#include "Terrain.h"
#include "Layer.h"

CMapElement_Lake::CMapElement_Lake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Lake::CMapElement_Lake(const CMapElement_Lake& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Lake::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Lake::Initialize(void* pArg)
{
	MAPOBJECT_LAKE_DESC* pDesc = static_cast<MAPOBJECT_LAKE_DESC*>(pArg);

	m_bEdit = pDesc->bEdit;
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
	m_bSelected = false;
	m_vPosition = pDesc->vPosition;
	m_vScale = pDesc->vScale;
	m_vRotation = pDesc->vRotation;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
#endif // _DEBUG

	m_fTimeSpeed = pDesc->fTimeSpeed;

	m_fRefractionStrength = pDesc->fRefractionStrength;
	m_fRefractionPow = pDesc->fRefractionPow;

	m_fUVValue1 = pDesc->fUVValue1;
	m_fUVValue2 = pDesc->fUVValue2;
	m_fUVValue3 = pDesc->fUVValue3;

	m_vUVSpeed = pDesc->vUVSpeed;
	m_vLargeUVSpeed = pDesc->vLargeUVSpeed;
	m_vSubUVSpeed3 = pDesc->vSubUVSpeed3;

	memcpy(&m_vRefractionColor, &pDesc->vRefractionColor, sizeof(_float4));
	memcpy(&m_vSurfaceColor, &pDesc->vSurfaceColor, sizeof(_float4));

	return S_OK;
}

void CMapElement_Lake::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif 
}

void CMapElement_Lake::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta * m_fTimeSpeed;

	if(m_bEdit)
		Describe_Entity();
}

void CMapElement_Lake::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	
#endif 
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapElement_Lake::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::WATER)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelComs[0]->Render(0))) {
		return E_FAIL;
	}

	if (FAILED(m_pShallowModels[0]->Render(0))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapElement_Lake::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	MAPOBJECT_LAKE_DESC* pDesc = static_cast<MAPOBJECT_LAKE_DESC*>(pArg);

#pragma region EDITOR
	m_ShallowModelPrototypeTags = pDesc->ShallowModelPrototypeTags;
#pragma endregion

	m_fTimeSpeed = pDesc->fTimeSpeed;

	m_fRefractionStrength = pDesc->fRefractionStrength;
	m_fRefractionPow = pDesc->fRefractionPow;

	m_fUVValue1 = pDesc->fUVValue1;
	m_fUVValue2 = pDesc->fUVValue2;
	m_fUVValue3 = pDesc->fUVValue3;

	m_vUVSpeed = pDesc->vUVSpeed;
	m_vLargeUVSpeed = pDesc->vLargeUVSpeed;
	m_vSubUVSpeed3 = pDesc->vSubUVSpeed3;

	m_vRefractionColor = pDesc->vRefractionColor;
	m_vSurfaceColor = pDesc->vSurfaceColor;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_ModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->ShallowModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pShallowModels.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Noises_D"),
		reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Water_Normal_Large_N"),
		reinterpret_cast<CComponent**>(&m_pNormalLargeTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Water_Normal_Subtle_N"),
		reinterpret_cast<CComponent**>(&m_pNormalSubTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Caustics_D"),
		reinterpret_cast<CComponent**>(&m_pCausticsTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Water_N"),
		reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Lake_Cube_D"),
		reinterpret_cast<CComponent**>(&m_pCubeMapTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Lake_Refraction"),
		reinterpret_cast<CComponent**>(&m_pRefractionTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapElement_Lake::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalValue1", &m_fUVValue1, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalValue2", &m_fUVValue2, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalValue3", &m_fUVValue3, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpeed1", &m_vUVSpeed, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpeed2", &m_vLargeUVSpeed, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpeed3", &m_vSubUVSpeed3, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRefractionStrength", &m_fRefractionStrength, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRefractionPow", &m_fRefractionPow, sizeof(_float)))) {
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseColor", &m_vRefractionColor, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSurfaceColor", &m_vSurfaceColor, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pRefractionTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalTexture", m_pNormalTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalLargeTexture", m_pNormalLargeTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_CausticsTexture", m_pCausticsTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_SRV("g_NoiseTexture", m_pNoiseTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pCubeMapTextureCom->Bind_ShaderResource(m_pShaderCom, "g_CubeTexture", 0))) {
		return E_FAIL;
	}


	return S_OK;
}

CMapElement_Lake* CMapElement_Lake::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Lake* pInstance = new CMapElement_Lake(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Lake");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Lake::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Lake* pInstance = new CMapElement_Lake(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Lake");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Lake::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRefractionTextureCom);
	SAFE_RELEASE(m_pCubeMapTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pNormalLargeTextureCom);
	SAFE_RELEASE(m_pNormalSubTextureCom);
	SAFE_RELEASE(m_pCausticsTextureCom);
	SAFE_RELEASE(m_pNoiseTextureCom);
	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);

	for (auto& pModel : m_pShallowModels)
		SAFE_RELEASE(pModel);
}

void CMapElement_Lake::Describe_Entity()
{
	if (m_bDead)
		return;
	if (nullptr == m_pGameInstance)
		return;
	GUI::Begin("Lake");
	GUI::InputText("Containter Name", m_szSaveName, MAX_PATH);
	if(GUI::Button("Save_LakeData"))
	{
		tinyxml2::XMLDocument doc;
		string strPath = "../Bin/Resources/Data/Map/Water/Element_Water_Info.xml";

		tinyxml2::XMLError loadResult = doc.LoadFile(strPath.c_str());

		tinyxml2::XMLElement* root = {};

		if (loadResult == tinyxml2::XML_SUCCESS)
		{
			root = doc.FirstChildElement("MapObjects_Water");
			Save_XML(doc, root);

			if (doc.SaveFile(strPath.c_str()) != tinyxml2::XML_SUCCESS) {
				MSG_BOX("Failed to Save File");
			}
		}
		else
			MSG_BOX("Failed to Save Data");
	}

	GUI::Text(CMyTools::ToString(m_ModelPrototypeTags[m_iLodIndex]).c_str());
	GUI::InputInt("Lod Level", (_int*)(&m_iLodIndex));

	GUI::InputFloat("N_V1", (&m_fUVValue1));
	GUI::InputFloat("N_V2", (&m_fUVValue2));
	GUI::InputFloat("N_V3", (&m_fUVValue3));

	GUI::InputFloat2("UV_Speed1", (_float*)(&m_vUVSpeed));
	GUI::InputFloat2("UV_Speed2", (_float*)(&m_vLargeUVSpeed));
	GUI::InputFloat2("UV_Speed3", (_float*)(&m_vSubUVSpeed3));

	GUI::InputFloat("m_fRefractionStrength", (&m_fRefractionStrength));
	GUI::InputFloat("m_fRefractionPow", (&m_fRefractionPow));

	m_iLodIndex = max(0, m_iLodIndex);
	m_iLodIndex = min(m_iMaxLodLevel, m_iLodIndex);

	GUI::ColorEdit4("Refaction Color", (_float*)&m_vRefractionColor);
	GUI::ColorEdit4("Surface Color", (_float*)&m_vSurfaceColor);
	GUI::InputFloat("m_fTimeSpeed", (_float*)&m_fTimeSpeed);

	GUI::Text("----- Transfrom ----");
	_float3 vMove = {};
	GUI::InputFloat("Right", &vMove.x, 1.f, 10.f);
	GUI::InputFloat("Up", &vMove.y, 1.f, 10.f);
	GUI::InputFloat("Look", &vMove.z, 1.f, 10.f);

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
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 10.f, 45.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 10.f, 45.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 10.f, 45.f);

	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	m_pTransformCom->Set_Scale(m_vScale);

	if (GUI::Button("Delete"))
		m_bDead = true;
	GUI::End();
	m_bSelected = true;
}

HRESULT CMapElement_Lake::Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement* object = doc.NewElement("Object");
	object->SetAttribute("Type", ENUM_CLASS(MAPOBJECT_TYPE::ELEMENT_LAKE));
	object->SetAttribute("Name", m_szSaveName);
	object->SetAttribute("Lod_Level", m_iLodIndex);
	root->InsertEndChild(object);

#pragma region PROTOTYPETAG
	for (_uint i = 0; i < m_iLodIndex + 1; ++i)
	{
		tinyxml2::XMLElement* prototype = doc.NewElement("PrototypeTag");
		prototype->SetText(CMyTools::ToString(m_ModelPrototypeTags[i]).c_str());
		object->InsertEndChild(prototype);
	}
	for (_uint i = 0; i < m_iLodIndex + 1; ++i)
	{
		tinyxml2::XMLElement* prototype = doc.NewElement("ShollowPrototypeTag");
		prototype->SetText(CMyTools::ToString(m_ShallowModelPrototypeTags[i]).c_str());
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
#pragma endregion

#pragma region SHADER_VALUE
	tinyxml2::XMLElement* TimeSpeed = doc.NewElement("TimeSpeed");
	TimeSpeed->SetAttribute("value", m_fTimeSpeed);
	object->InsertEndChild(TimeSpeed);

	tinyxml2::XMLElement* RefractionStrength = doc.NewElement("RefractionStrength");
	RefractionStrength->SetAttribute("value", m_fRefractionStrength);
	object->InsertEndChild(RefractionStrength);

	tinyxml2::XMLElement* RefractionPow = doc.NewElement("RefractionPow");
	RefractionPow->SetAttribute("value", m_fRefractionPow);
	object->InsertEndChild(RefractionPow);

	tinyxml2::XMLElement* UVValue1 = doc.NewElement("UVValue1");
	UVValue1->SetAttribute("value", m_fUVValue1);
	object->InsertEndChild(UVValue1);

	tinyxml2::XMLElement* UVValue2 = doc.NewElement("UVValue2");
	UVValue2->SetAttribute("value", m_fUVValue2);
	object->InsertEndChild(UVValue2);

	tinyxml2::XMLElement* UVValue3 = doc.NewElement("UVValue3");
	UVValue3->SetAttribute("value", m_fUVValue3);
	object->InsertEndChild(UVValue3);

	// float2 값들
	tinyxml2::XMLElement* UVSpeed = doc.NewElement("UVSpeed");
	UVSpeed->SetAttribute("x", m_vUVSpeed.x);
	UVSpeed->SetAttribute("y", m_vUVSpeed.y);
	object->InsertEndChild(UVSpeed);

	tinyxml2::XMLElement* LargeUVSpeed = doc.NewElement("LargeUVSpeed");
	LargeUVSpeed->SetAttribute("x", m_vLargeUVSpeed.x);
	LargeUVSpeed->SetAttribute("y", m_vLargeUVSpeed.y);
	object->InsertEndChild(LargeUVSpeed);

	tinyxml2::XMLElement* SubUVSpeed3 = doc.NewElement("SubUVSpeed3");
	SubUVSpeed3->SetAttribute("x", m_vSubUVSpeed3.x);
	SubUVSpeed3->SetAttribute("y", m_vSubUVSpeed3.y);
	object->InsertEndChild(SubUVSpeed3);

	// float4 값들
	tinyxml2::XMLElement* RefractionColor = doc.NewElement("RefractionColor");
	RefractionColor->SetAttribute("x", m_vRefractionColor.x);
	RefractionColor->SetAttribute("y", m_vRefractionColor.y);
	RefractionColor->SetAttribute("z", m_vRefractionColor.z);
	RefractionColor->SetAttribute("w", m_vRefractionColor.w);
	object->InsertEndChild(RefractionColor);

	tinyxml2::XMLElement* SurfaceColor = doc.NewElement("SurfaceColor");
	SurfaceColor->SetAttribute("x", m_vSurfaceColor.x);
	SurfaceColor->SetAttribute("y", m_vSurfaceColor.y);
	SurfaceColor->SetAttribute("z", m_vSurfaceColor.z);
	SurfaceColor->SetAttribute("w", m_vSurfaceColor.w);
	object->InsertEndChild(SurfaceColor);
	
#pragma endregion

	return S_OK;
}
