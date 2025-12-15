#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "VIBuffer_Terrain.h"
#include "AlphaMap.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	m_fUsingSurfaceParams = 15.f / 27.f;
	m_vRotation = _float3{ 0.f, 0.f, 0.f };

	TERRAIN_DESC* pDesc = static_cast<TERRAIN_DESC*>(pArg);

	m_isEdit = pDesc->isEdit;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));

	if (FAILED(Ready_AlphaMap(pDesc)))
		return E_FAIL;

	//m_pVIBufferCom->Set_CullingRadius(0.04f);

	if(!m_isEdit)
	{
		m_pAlphaMap->Load_ToFile(pDesc->strAlphaMapTag.c_str());

		if (FAILED(m_pVIBufferCom->Load_HeightMap(pDesc->strHeightMapTag.c_str())))
			return E_FAIL;
	}
	// Psx 적용할지 안할지
	m_bUsePsx = true;

#ifdef 진우
	//if(!m_bUsePsx)
	//{
	//	if (FAILED(m_pVIBufferCom->Load_HeightMap("Hogsmeade_HeightMap.bin")))
	//}
	//else
	{
		CRigidBody_Static::RIGIDBODY_STATIC_DESC Desc{};
		Desc.pMeshName = TEXT("Hogsmeade_HeightMap");
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::TERRAIN);
		/* Com_RigidBody */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogsmeade"),
			reinterpret_cast<CComponent**>(&m_pRigidBody), &Desc))) {
			return E_FAIL;
		}
	}
#endif

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
}

void CTerrain::Update(_float fTimeDelta)
{
	if (m_isEdit)
		Describe_Entity();

	//m_pVIBufferCom->Culling(m_pTransformCom->Get_XMWorldMatrix());
	
	if(m_pGameInstance->Key_Down(DIK_LCONTROL) && m_pGameInstance->Key_Down(DIK_W))
	{
		m_bWasWireFrame = !m_bWasWireFrame;
	}
}

void CTerrain::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//}
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	
	if(m_bWasWireFrame)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::TERRAIN)))) {
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::ENV_TERRAIN_ANISO)))) {
			return E_FAIL;
		}
	}

	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::Ready_Components(void* pArg)
{
	__super::Ready_Components(nullptr);

	TERRAIN_DESC* pDesc = static_cast<TERRAIN_DESC*>(pArg);

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->strBufferTag,
		reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NORTEX,
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

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseMultiplier", &m_vDRN.x, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSurfaceMultiplier", &m_vDRN.y, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalMultiplier", &m_vDRN.z, sizeof(_float)))) {
		return E_FAIL;
	}
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

	if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTextures", 0, m_pDiffuseTextureCom->Get_Size()))){
		return E_FAIL;
	}
	if (FAILED(m_pNormalTextureCom->Bind_ShaderResources(m_pShaderCom, "g_NormalTextures", 0, m_pNormalTextureCom->Get_Size()))){
		return E_FAIL;
	}
	if (FAILED(m_pMROTextureCom->Bind_ShaderResources(m_pShaderCom, "g_SurfaceParamsTextures", 0, m_pMROTextureCom->Get_Size()))){
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_MaskTexture", m_pAlphaMap->Get_SRV()))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources_Edit()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseMultiplier", &m_vDRN.x, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSurfaceMultiplier", &m_vDRN.y, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalMultiplier", &m_vDRN.z, sizeof(_float)))) {
		return E_FAIL;
	}
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

HRESULT CTerrain::Ready_AlphaMap(TERRAIN_DESC* pDesc)
{
	m_pAlphaMap = CAlphaMap::Create(m_pDevice, m_pContext, pDesc->iAlphaSizeX, pDesc->iAlphaSizeY);

	if(nullptr == m_pAlphaMap)
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg, CGameObject* pOwner)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pAlphaMap);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pDiffuseTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pMROTextureCom);
}

void CTerrain::Describe_Entity()
{
	GUI::Begin("Terrain");

#pragma region ALPHA_MAP
	GUI::InputInt("Color", &m_iColorIndex);
	m_iColorIndex = max(0, min(4, m_iColorIndex));

	GUI::InputInt("Rnage", (_int*)(&m_iMaskRange));
	GUI::DragFloat("Value", &m_fMaskValue, 0.01f, -1.0f, 1.f);
	if (GUI::Button("Save", ImVec2(100.f, 30.f)))
	{
		m_pAlphaMap->Save_ToFile("Hogwart_AlphaMap.bin");
	}
	if (GUI::Button("Load", ImVec2(100.f, 30.f)))
	{
		m_pAlphaMap->Load_ToFile("Hogwart_AlphaMap.bin");
	}


	//GUI::DragFloat("Up", &m_fUpValue, 0.01f, -1.0f, 1.f);

	if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		if (m_pGameInstance->isPicking(&m_vPickingPosition))
		{
			if (m_pGameInstance->Key_Pressing(DIK_PERIOD))
				m_pVIBufferCom->FitY(m_pTransformCom->Get_XMWorldMatrix(), m_fUpValue);
			if (m_pGameInstance->Key_Pressing(DIK_COMMA))
				m_pVIBufferCom->FitY(m_pTransformCom->Get_XMWorldMatrix(), m_fUpValue * -1.f);


			XMStoreFloat3(&m_vPickingPosition, XMVector3TransformCoord(XMLoadFloat3(&m_vPickingPosition), m_pTransformCom->Get_WorldMatrixInv()));

			m_pAlphaMap->Update(m_vPickingPosition, m_iColorIndex, m_fMaskValue, m_iMaskRange);

		}
	}
#pragma endregion

#pragma region HEIGHT MAP
	HRESULT hr = {};
	if(GUI::Button("Save HeightMap"))
		hr = m_pVIBufferCom->Save_HeightMap("Hogwart_Height.bin");
	if(GUI::Button("Load HeightMap"))
		hr = m_pVIBufferCom->Load_HeightMap("Hogwart_Height.bin");
#pragma endregion

	_float4 vPos = {};
	_float3 vRotation = {};
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
	GUI::DragFloat3("Pos", (_float*)(&vPos));
	GUI::DragFloat3("Rota", (_float*)(&m_vRotation));

	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vPos));

	//GUI::DragFloat("Culling Radius", &m_fCullingRadius, 0.01f, 0.01f, 2.f);

	GUI::End();
}
