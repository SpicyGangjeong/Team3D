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

	if (FAILED(__super::Initialize(pArg))){
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))){
		return E_FAIL;
	}

	m_fUsingSurfaceParams = SRO_PARAMETER;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));

	{
		CRigidBody_Static::RIGIDBODY_STATIC_DESC Desc{};
		Desc.pMeshName = pDesc->strRigidBody_MeshName.c_str();
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::TERRAIN);
		Desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		/* Com_RigidBody */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->strRigidBody_ComponentTag,
			reinterpret_cast<CComponent**>(&m_pRigidBody), &Desc))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
}

void CTerrain::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

}

void CTerrain::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//}
}

HRESULT CTerrain::Render()
{
	if (false == m_bVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseMultiplier", &m_vDRN.x, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSurfaceMultiplier", &m_vDRN.y, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalMultiplier", &m_vDRN.z, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::ENV_TERRAIN_ANISO)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::Render_Shadow(SHADOW eType)
{
	if (false == m_bVisible)
		return S_OK;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::SHADOW)))) {
		return E_FAIL;
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
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->strVIBufferTag,
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

	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
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
	GUI::Begin("Renderer", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
	size_t iIndex = (size_t)this;

	
	_string strTag = "Terrain_" + to_string((size_t)iIndex);
	if (GUI::CollapsingHeader(strTag.c_str()))
	{
		static _int iTuningValue = 16;
		GUI::DragInt("DSN1", (_int*)&iTuningValue, 1, 1024);
		GUI::DragInt("DSN2", (_int*)&iTuningValue, 1, 1024);
		GUI::DragInt("DSN3", (_int*)&iTuningValue, 1, 1024);
		m_vDRN.x = 1.f / (_float)iTuningValue;
		m_vDRN.y = 1.f / (_float)iTuningValue;
		m_vDRN.z = 1.f / (_float)iTuningValue;
		GUI::PopItemWidth();
		GUI::Checkbox("Visible", &m_bVisible);
	}
	GUI::End();
}
#endif // _DEBUG
