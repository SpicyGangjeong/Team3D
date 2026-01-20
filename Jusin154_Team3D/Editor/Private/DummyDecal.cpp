#include "pch.h"
#include "DummyDecal.h"

#include "GameInstance.h"


CDummyDecal::CDummyDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummyDecal::CDummyDecal(const CDummyDecal& rhs)
	: CGameObject(rhs)
	, m_fWinSizeX{rhs.m_fWinSizeX}
	, m_fWinSizeY{rhs.m_fWinSizeY}
{
}

HRESULT CDummyDecal::Initialize_Prototype()
{
	m_fWinSizeX = g_iWinSizeX;
	m_fWinSizeY = g_iWinSizeY;

	return S_OK;
}

HRESULT CDummyDecal::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	_float fMaxScale = 1.f;

	if (nullptr != pArg)
	{
		DUMMY_DECAL_DESC* pDesc = static_cast<DUMMY_DECAL_DESC*>(pArg);
		m_pTransformCom->Set_Scale(pDesc->vScale);
		m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));

		m_fUVTiling = pDesc->fUVTiling;
		m_vUVSpeed = pDesc->vUVSpeed;
		m_vMaskRed = pDesc->vMaskRed;
		m_vMaskGreen = pDesc->vMaskGreen;
		m_vMaskBlue = pDesc->vMaskBlue;

		_float fBoxRadius = 0.86f; // 1,1,1 기준
		fMaxScale = max(pDesc->vScale.x, max(pDesc->vScale.y, pDesc->vScale.z));
	}
	else
	{
		m_vMaskRed = _float4(1.f, 0.f, 0.f, 1.f);
		m_vMaskGreen = _float4(0.f, 0.f, 0.f, 0.f);
		m_vMaskBlue = _float4(1.f, 0.f, 0.f, 1.f);
		m_vUVSpeed = _float2(0.05f, 0.05f);
	}
	
	m_pTransformCom->Set_Radius(fMaxScale);

	return S_OK;
}

void CDummyDecal::Priority_Update(_float fTimeDelta)
{
	
}

void CDummyDecal::Update(_float fTimeDelta)
{
	m_fTimeAcc.x += fTimeDelta * m_vUVSpeed.x;
	m_fTimeAcc.y += fTimeDelta * m_vUVSpeed.y;

	//Describe_Entity();
}

void CDummyDecal::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::DECAL, this);
	}
}

HRESULT CDummyDecal::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	
	_uint iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pShaderCom->Bind_SRV("g_MaskingTexture", m_pMaskTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_SRV("g_NormalTexture", m_pNormalTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_SRV("g_SurfaceParamsTexture", m_pSurfaceTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_SRV("g_NoiseTexture", m_pFadeTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_SRV("g_DiffsueMaskTexture", m_pDiffuseMaskTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Begin(22))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	
	return S_OK;
}

HRESULT CDummyDecal::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Decal_MSK"),
		reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Decal_Noraml"),
		reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Base_MRO"),
		reinterpret_cast<CComponent**>(&m_pSurfaceTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Decal_Fade"),
		reinterpret_cast<CComponent**>(&m_pFadeTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Decal_DiffuseMask"),
		reinterpret_cast<CComponent**>(&m_pDiffuseMaskTextureCom))))
		return E_FAIL;


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_DecalBox"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyDecal::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	_float4x4 WorldInv = {};
	
	XMStoreFloat4x4(&WorldInv, m_pTransformCom->Get_WorldMatrixInv());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrixInv", &WorldInv))) {
		return E_FAIL;
	}

	// Base MRO
	// g_fUsingSurfaceParams 15.f / 27.f;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWinSizeX", &m_fWinSizeX, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWinSizeY", &m_fWinSizeY, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTime", &m_fTimeAcc, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVTiling", &m_fUVTiling, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskColorRed", &m_vMaskRed, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskColorGreen", &m_vMaskGreen, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskColorBlue", &m_vMaskBlue, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture"))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_NormalCopy"), m_pShaderCom, "g_NormalMapTexture"))) {
		return E_FAIL;
	}

	return S_OK;
}

CDummyDecal* CDummyDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummyDecal* pInstance = new CDummyDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummyDecal");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummyDecal::Clone(void* pArg, CGameObject* pOwner)
{
	CDummyDecal* pInstance = new CDummyDecal(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummyDecal");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummyDecal::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuseMaskTextureCom);
	SAFE_RELEASE(m_pFadeTextureCom);
	SAFE_RELEASE(m_pMaskTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pSurfaceTextureCom);
	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
}

void CDummyDecal::Describe_Entity()
{
	GUI::Begin("TEST DECAL");

	GUI::InputFloat("m_fUVTiling", (_float*)(&m_fUVTiling));
	GUI::InputFloat2("m_fSpeed", (_float*)(&m_vUVSpeed));

	GUI::ColorEdit4("Mask Red", (_float*)&m_vMaskRed);
	GUI::ColorEdit4("Mask Green", (_float*)&m_vMaskGreen);
	GUI::ColorEdit4("Mask Blue", (_float*)&m_vMaskBlue);

	m_pTransformCom->Describe_Entity();

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
		}
	}

	

	GUI::End();
}

HRESULT CDummyDecal::Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement* object = doc.NewElement("Object");
	object->SetAttribute("Type", 0);
	root->InsertEndChild(object);


#pragma region TRANSFORM
	_float3 vPosition = {};
	XMStoreFloat3(&vPosition, m_pTransformCom->Get_State(STATE::POSITION));

	_float3 vScale = m_pTransformCom->Get_Scale();

	_float3 vRotation = m_pTransformCom->Get_Rotation();


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

#pragma region DECAL

	/* Diffuse */
	tinyxml2::XMLElement* Shader_Value = doc.NewElement("Shader_Value");
	Shader_Value->SetAttribute("UVTiling", m_fUVTiling);
	Shader_Value->SetAttribute("UVSpeedX", m_vUVSpeed.x);
	Shader_Value->SetAttribute("UVSpeedY", m_vUVSpeed.y);
	object->InsertEndChild(Shader_Value);

	/* MaskRed */
	tinyxml2::XMLElement* MaskRed = doc.NewElement("MaskRed");
	MaskRed->SetAttribute("r", m_vMaskRed.x);
	MaskRed->SetAttribute("g", m_vMaskRed.y);
	MaskRed->SetAttribute("b", m_vMaskRed.z);
	MaskRed->SetAttribute("a", m_vMaskRed.w);
	object->InsertEndChild(MaskRed);

	/* MaskGreen */
	tinyxml2::XMLElement* MaskGreen = doc.NewElement("MaskGreen");
	MaskGreen->SetAttribute("r", m_vMaskGreen.x);
	MaskGreen->SetAttribute("g", m_vMaskGreen.y);
	MaskGreen->SetAttribute("b", m_vMaskGreen.z);
	MaskGreen->SetAttribute("a", m_vMaskGreen.w);
	object->InsertEndChild(MaskGreen);

	/* MaskBlue */
	tinyxml2::XMLElement* MaskBlue = doc.NewElement("MaskBlue");
	MaskBlue->SetAttribute("r", m_vMaskBlue.x);
	MaskBlue->SetAttribute("g", m_vMaskBlue.y);
	MaskBlue->SetAttribute("b", m_vMaskBlue.z);
	MaskBlue->SetAttribute("a", m_vMaskBlue.w);
	object->InsertEndChild(MaskBlue);

#pragma endregion

	return S_OK;
}
