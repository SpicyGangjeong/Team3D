#include "pch.h"
#include "MapElement_Light.h"

#include "GameInstance.h"

#include "Terrain.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"

CMapElement_Light::CMapElement_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Light::CMapElement_Light(const CMapElement_Light& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Light::Initialize(void* pArg)
{
	MAPELEMENT_LIGHT_DESC* pDesc = static_cast<MAPELEMENT_LIGHT_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;
	m_fBloomStrength = pDesc->fBloomStregth;
	m_iGlassMeshIndex = pDesc->iGlassMeshIndex;

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

	return S_OK;
}

void CMapElement_Light::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif 
}

void CMapElement_Light::Update(_float fTimeDelta)
{
}

void CMapElement_Light::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_bSelected)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		m_pTransformCom->Set_Scale(m_vScale);
		m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	}
#endif 

	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius())) 
	{
		if (m_isLightOn)
		{
			_float fDistance = XMVectorGetX(XMVector3LengthSq(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - m_pTransformCom->Get_State(STATE::POSITION)));
			if (m_LightAdded_Distance > fDistance) // 카메라 범위 안
			{
				if(!m_isLightAdded)
				{
					m_isLightAdded = true;
					m_pGameInstance->Add_Light(CURRENT_LEVEL, m_pLightCom);
				}
			}
			else
			{
				if (m_isLightAdded)
				{
					m_isLightAdded = false;
					m_pGameInstance->Delete_Light(CURRENT_LEVEL, m_pLightCom);
				}
			}

			m_pGameInstance->Add_RenderGroup(RENDER::BLOOM, this);
		}

		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CMapElement_Light::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();
	
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (true == m_isLightOn && m_iGlassMeshIndex == i)
			continue;

		if (m_iGlassMeshIndex == i)
		{
			if (FAILED(m_pModelComs[0]->Bind_Material(m_iGlassMeshIndex, m_pShaderCom))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pGlassTextureCom->Get_SRV(0)))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_SRV("g_GlassTexture", m_pMaskTextureCom->Get_SRV(0)))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlassRatio", &m_fGlassRatio, sizeof(_float)))) {
				return E_FAIL;
			}
			
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::GLASS)))) {
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
				return E_FAIL;
			}
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMapElement_Light::Ready_Components(void* pArg)
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
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Emissive"),
		reinterpret_cast<CComponent**>(&m_pEmissiveTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Mask"),
		reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Base"),
		reinterpret_cast<CComponent**>(&m_pGlassTextureCom))))
		return E_FAIL;


	MAPELEMENT_LIGHT_DESC* pDesc = static_cast<MAPELEMENT_LIGHT_DESC*>(pArg);

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::POINT;

	memcpy(&LightDesc.vDiffuse, &pDesc->vDiffuse, sizeof(_float4));
	memcpy(&LightDesc.vAmbient, &pDesc->vAmbient, sizeof(_float4));
	memcpy(&LightDesc.vSpecular, &pDesc->vSpecular, sizeof(_float4));
	memcpy(&LightDesc.vPosOffset, &pDesc->vPosOffset, sizeof(_float4));

	LightDesc.fRange = pDesc->fRange;
	LightDesc.iLevel = NEXT_LEVEL;
	LightDesc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);
	
	/* Com_Light*/
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
	{
		return E_FAIL;
	}

	if(false == pDesc->isPow)
		m_pLightCom->Switch_LightAttenationMethod();

	return S_OK;
}

HRESULT CMapElement_Light::Bind_ShaderResources()
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

HRESULT CMapElement_Light::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomStrength", &m_fBloomStrength ,sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_SRV("g_EmissiveTexture", m_pEmissiveTextureCom->Get_SRV(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pGlassTextureCom->Get_SRV(0))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::BLOOM)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelComs[m_iLodIndex]->Render(m_iGlassMeshIndex))) {
		return E_FAIL;
	}

	return S_OK;
}

void CMapElement_Light::Toggle_Light()
{
	if (m_isLightOn)
		m_pGameInstance->Delete_Light(CURRENT_LEVEL, m_pLightCom);
	else
		m_pGameInstance->Add_Light(CURRENT_LEVEL, m_pLightCom);

	m_isLightAdded = m_isLightOn = !m_isLightOn;
}

CMapElement_Light* CMapElement_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Light* pInstance = new CMapElement_Light(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Light");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Light::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Light* pInstance = new CMapElement_Light(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Light");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Light::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLightCom);
	SAFE_RELEASE(m_pEmissiveTextureCom);
	SAFE_RELEASE(m_pMaskTextureCom);
	SAFE_RELEASE(m_pGlassTextureCom);

	SAFE_RELEASE(m_pShaderCom);
	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

void CMapElement_Light::Describe_Entity()
{
	m_pLightCom->Describe_Entity();

	if (m_bDead)
		return;
	if (nullptr == m_pGameInstance)
		return;
	GUI::Text("LIGHT ");
	GUI::InputFloat("Distacne", &m_LightAdded_Distance, 1.f, 10.f);
	if (GUI::Button("Light ON / OFF"))
		Toggle_Light();

	GUI::InputFloat("BloomStrength", &m_fBloomStrength, 0.1f);
	GUI::SliderFloat("GlassRatio", &m_fGlassRatio, 0.1f, 1.f);

	GUI::Text(CMyTools::ToString(m_ModelPrototypeTags[m_iLodIndex]).c_str());
	GUI::InputInt("Lod Level", (_int*)(&m_iLodIndex));
	m_iLodIndex = max(0, m_iLodIndex);
	m_iLodIndex = min(m_iMaxLodLevel, m_iLodIndex);
	GUI::Text("----- Transfrom ----");
	GUI::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		if(m_pGameInstance->isPicking(&m_vPosition))
		{ }
	}

	GUI::Text("----- Rotation ----");
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 10.f, 45.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 10.f, 45.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 10.f, 45.f);

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	if (GUI::Button("Delete"))
		m_bDead = true;



	m_bSelected = true;
}

HRESULT CMapElement_Light::Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement* object = doc.NewElement("Object");
	object->SetAttribute("Type", ENUM_CLASS(MAPOBJECT_TYPE::ELEMENT_LIGHT));
	object->SetAttribute("Lod_Level", m_iLodIndex);
	root->InsertEndChild(object);

#pragma region PROTOTYPETAG
	for (_uint i = 0; i < m_iLodIndex + 1; ++i)
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
#pragma endregion

#pragma region LIGHT_DESC
	const LIGHT_DESC* pLight = m_pLightCom->Get_LightDesc();

	/* Diffuse */
	tinyxml2::XMLElement* Diffuse = doc.NewElement("Diffuse");
	Diffuse->SetAttribute("x", pLight->vDiffuse.x);
	Diffuse->SetAttribute("y", pLight->vDiffuse.y);
	Diffuse->SetAttribute("z", pLight->vDiffuse.z);
	object->InsertEndChild(Diffuse);

	/* Ambient */
	tinyxml2::XMLElement* Ambient = doc.NewElement("Ambient");
	Ambient->SetAttribute("x", pLight->vAmbient.x);
	Ambient->SetAttribute("y", pLight->vAmbient.y);
	Ambient->SetAttribute("z", pLight->vAmbient.z);
	object->InsertEndChild(Ambient);

	/* Specular */
	tinyxml2::XMLElement* Specular = doc.NewElement("Specular");
	Specular->SetAttribute("x", pLight->vSpecular.x);
	Specular->SetAttribute("y", pLight->vSpecular.y);
	Specular->SetAttribute("z", pLight->vSpecular.z);
	object->InsertEndChild(Specular);
	
	/* PosOffset */
	tinyxml2::XMLElement* PosOffset = doc.NewElement("PosOffset");
	PosOffset->SetAttribute("x", pLight->vPosOffset.x);
	PosOffset->SetAttribute("y", pLight->vPosOffset.y);
	PosOffset->SetAttribute("z", pLight->vPosOffset.z);
	object->InsertEndChild(PosOffset);

	/* Info */
	tinyxml2::XMLElement* Info = doc.NewElement("Info");
	Info->SetAttribute("GlassIndex", m_iGlassMeshIndex);
	Info->SetAttribute("BloomStrength", m_fBloomStrength);
	Info->SetAttribute("Range", pLight->fRange);

	if(false == m_pLightCom->Is_PowerLightAtt())
		Info->SetAttribute("Pow", 0);
	else
		Info->SetAttribute("Pow", 1);

	object->InsertEndChild(Info);
#pragma endregion


	return S_OK;
}
