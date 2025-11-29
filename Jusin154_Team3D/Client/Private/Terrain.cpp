#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "VIBuffer_Terrain.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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
	TERRAIN_DESC* pDesc = static_cast<TERRAIN_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_fUsingSurfaceParams = 15.f / 27.f;
	//m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-194, 18.5f, -153.f, 1.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));


	CRigidBody_Static::RIGIDBODY_STATIC_DESC Desc{};
	Desc.pMeshName = TEXT("Hogsmeade_HeightMap");
	Desc.iSubKind = 998;
	/* Com_RigidBody */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogsmeade"),
		reinterpret_cast<CComponent**>(&m_pRigidBody), &Desc))) {
		return E_FAIL;
	}

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
}

void CTerrain::Update(_float fTimeDelta)
{

#ifdef _DEBUG
	if (m_pGameInstance->Key_Down(DIK_LSHIFT) )
	{
		m_bWasWireFrame = !m_bWasWireFrame;
	}
#endif // _DEBUG
}

void CTerrain::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//}
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
#ifdef _DEBUG
	static _int iValue1 = 16;
	static _int iValue2 = 16;
	static _int iValue3 = 16;
	GUI::SliderInt("DSN1", (_int*)&iValue1, 1, 1024);
	GUI::SliderInt("DSN2", (_int*)&iValue2, 1, 1024);
	GUI::SliderInt("DSN3", (_int*)&iValue3, 1, 1024);
	m_vDRN.x = 1.f / (_float)iValue1;
	m_vDRN.y = 1.f / (_float)iValue2;
	m_vDRN.z = 1.f / (_float)iValue3;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseMultiplier", &m_vDRN.x, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSurfaceMultiplier", &m_vDRN.y, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalMultiplier", &m_vDRN.z, sizeof(_float)))) {
		return E_FAIL;
	}
	if (m_bWasWireFrame)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::ENV_TERRAIN_ANISO)))) {
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::DEFAULT)))) {
			return E_FAIL;
		}
	}
#else
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::ENV_TERRAIN_ANISO)))) {
		return E_FAIL;
	}

#endif // _DEBUG

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
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
		reinterpret_cast<CComponent**>(&m_pVIBufferCom)))){
		return E_FAIL;
	}


	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NORTEX,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, pDesc->strDiffuseTextureTag, reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom), nullptr))) {
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, pDesc->strNormalTextureTag, reinterpret_cast<CComponent**>(&m_pNormalTextureCom), nullptr))) {
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, pDesc->strMROTextureTag, reinterpret_cast<CComponent**>(&m_pMROTextureCom), nullptr))) {
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, pDesc->strAlphaMapTextureTag, reinterpret_cast<CComponent**>(&m_pAlphaMapTextureCom), nullptr))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_SRV("g_MaskTexture", m_pAlphaMapTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float)))){
		return E_FAIL;
	}

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

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pDiffuseTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pMROTextureCom);
	SAFE_RELEASE(m_pAlphaMapTextureCom);
}
#ifdef _DEBUG
void CTerrain::Describe_Entity()
{

}
#endif // _DEBUG
