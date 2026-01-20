#include "pch.h"
#include "Land.h"

#include "GameInstance.h"
#include "AlphaMap.h"

CLand::CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLand::CLand(const CLand& rhs)
	: CGameObject(rhs), m_fUsingSurfaceParams{rhs.m_fUsingSurfaceParams}
{
}

void CLand::Priority_Update(_float fTimeDelta)
{
}

void CLand::Update(_float fTimeDelta)
{
	if (m_bEdit)
		Describe_Entity();
}

void CLand::Late_Update(_float fTimeDelta)
{
	if(m_pGameInstance->IsIn_WorldFrustum(m_pTransformCom->Get_State(STATE::POSITION), 400.f))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	else
	{
		_uint i = 0;
	}
}

HRESULT CLand::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
	/*	if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(17))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLand::Initialize_Prototype()
{
	m_fUsingSurfaceParams = MRO_PARAMETER;

	return S_OK;
}

HRESULT CLand::Initialize(void* pArg)
{
	m_vScale = _float3(1.f, 1.f, 1.f);
	m_vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_AlphaMap()))
		return E_FAIL;

	LAND_DESC* pLand_Desc = static_cast<LAND_DESC*>(pArg);

	if(pLand_Desc->isLoadAlphaMap)
		m_pAlphaMap->Load_ToFile(pLand_Desc->strAlphaMapTag.c_str());

	return S_OK;
}

HRESULT CLand::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	LAND_DESC* pDesc = static_cast<LAND_DESC*>(pArg);

	m_bEdit = pDesc->bEdit;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	memcpy(&m_vScale, &pDesc->vScale, sizeof(_float3));

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->strModelComTag,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Terrain_Diffuse"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom), nullptr))) {
		return E_FAIL;
	}
	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Terrain_Normal"), reinterpret_cast<CComponent**>(&m_pNormalTextureCom), nullptr))) {
		return E_FAIL;
	}
	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Terrain_MRO"), reinterpret_cast<CComponent**>(&m_pMROTextureCom), nullptr))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLand::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTextures", 0, m_pDiffuseTextureCom->Get_Size()))) {
		return E_FAIL;
	}
	if (FAILED(m_pNormalTextureCom->Bind_ShaderResources(m_pShaderCom, "g_NormalTextures", 0, m_pNormalTextureCom->Get_Size()))) {
		return E_FAIL;
	}
	if (FAILED(m_pMROTextureCom->Bind_ShaderResources(m_pShaderCom, "g_SurfaceParamsTextures", 0, m_pMROTextureCom->Get_Size()))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_MaskTexture", m_pAlphaMap->Get_SRV()))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLand::Ready_AlphaMap()
{
	m_pAlphaMap = CAlphaMap::Create(m_pDevice, m_pContext, 1024, 1024);

	if (nullptr == m_pAlphaMap)
		return E_FAIL;

	return S_OK;
}

CLand* CLand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLand* pInstance = new CLand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLand");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLand::Clone(void* pArg, CGameObject* pOwner)
{
	CLand* pInstance = new CLand(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLand");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CLand::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

	SAFE_RELEASE(m_pDiffuseTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pMROTextureCom);
	SAFE_RELEASE(m_pAlphaMap);
}

void CLand::Describe_Entity()
{
	GUI::Begin("Land Debug");

	GUI::InputFloat2("PickingUV", (_float*)(&m_vPickingUV));

	GUI::Text("----- AlphaMap ----");
	GUI::InputInt("Color", &m_iColorIndex);
	m_iColorIndex = max(0, min(4, m_iColorIndex));
	GUI::InputInt("Rnage", (_int*)(&m_iMaskRange));
	GUI::DragFloat("Value", &m_fMaskValue, 0.01f, -1.0f, 1.f);

	if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_M))
	{
		_float3 m_vPickingPosition = {};
		if (m_pGameInstance->isPicking(&m_vPickingPosition))
		{
			if (m_pModelCom->PickingMesh(&m_vPickingPosition, &m_vPickingUV, 0, m_pTransformCom->Get_XMWorldMatrix()))
			{
				m_pAlphaMap->Update_Land(m_vPickingUV, m_iColorIndex, m_fMaskValue, m_iMaskRange);
			}
			//XMStoreFloat3(&m_vPickingPosition, XMVector3TransformCoord(XMLoadFloat3(&m_vPickingPosition), m_pTransformCom->Get_WorldMatrixInv()));
		}
	}
	GUI::Text("----- Position ----");
	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(STATE::POSITION));
	GUI::InputFloat("X##LandPos", &m_vPosition.x, 0.1f, 10.f);
	GUI::InputFloat("Y##LandPos", &m_vPosition.y, 0.1f, 10.f);
	GUI::InputFloat("Z##LandPos", &m_vPosition.z, 0.1f, 10.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

	if (GUI::Button("Save", ImVec2(100.f, 30.f)))
	{
		m_pAlphaMap->Save_ToFile("Land_TU_BB_AlphaMap.bin");
	}
	if (GUI::Button("Load", ImVec2(100.f, 30.f)))
	{
		m_pAlphaMap->Load_ToFile("Land_TU_BB_AlphaMap.bin");
	}	
	if (GUI::Button("SaveDDS", ImVec2(100.f, 30.f)))
	{
		m_pAlphaMap->Save_DDS("Land_TU_BB_AlphaMap.dds");
	}
	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	m_pTransformCom->Set_Scale(m_vScale);

	GUI::End();
}
