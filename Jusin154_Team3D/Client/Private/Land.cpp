#include "pch.h"
#include "Land.h"

#include "GameInstance.h"
#include "InstancedProp.h"

CLand::CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLand::CLand(const CLand& rhs)
	: CGameObject(rhs)
	, m_fUsingSurfaceParams(rhs.m_fUsingSurfaceParams)
{
}

void CLand::Priority_Update(_float fTimeDelta)
{
	for (auto& pInstanceProp : m_InstanceProps)
		pInstanceProp->Priority_Update(fTimeDelta);
}

void CLand::Update(_float fTimeDelta)
{
	for (auto& pInstanceProp : m_InstanceProps)
		pInstanceProp->Update(fTimeDelta);
}

void CLand::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_fRaduis))
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		for (auto& pInstanceProp : m_InstanceProps)
			pInstanceProp->Late_Update(fTimeDelta);
	}
}

HRESULT CLand::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::LAND)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Render(0))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLand::Render_Shadow(SHADOW eType)
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::SHADOW)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Render(0))) {
		return E_FAIL;
	}

	return S_OK;
}

void CLand::Add_InstanceProp(CInstancedProp* pIntanceProp)
{
	m_InstanceProps.push_back(pIntanceProp);
}

HRESULT CLand::Initialize_Prototype()
{
	m_fUsingSurfaceParams = MRO_PARAMETER;

	return S_OK;
}

HRESULT CLand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_fRaduis = 350.f;

	LAND_DESC* pDesc = static_cast<LAND_DESC*>(pArg);

	string RigidBodyTag = CMyTools::ToString(pDesc->strModelComTag) + "_RigidBody";

	ReadyForPhysX(RigidBodyTag.c_str());
	ConvertToPhysX(RigidBodyTag.c_str());

	return S_OK;
}

HRESULT CLand::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	LAND_DESC* pDesc = static_cast<LAND_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);

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

	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, pDesc->strAlphaMapTag, reinterpret_cast<CComponent**>(&m_pMaskTextureCom), nullptr))) {
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

	if (FAILED(m_pShaderCom->Bind_SRV("g_MaskTexture", m_pMaskTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CLand::ReadyForPhysX(const _char* pName)
{
	_uint iLevel = NEXT_LEVEL;

	if (FAILED(m_pModelCom->Ready_PhysXMeshes(XMMatrixIdentity(), iLevel, pName))) {
		assert(false);
	}
}

void CLand::ConvertToPhysX(const _char* pName)
{
	_wstring wstrName = CMyTools::ToWstring(pName);

	CRigidBody_Static::RIGIDBODY_STATIC_DESC Desc = {};
	Desc.iSubKind = ENUM_CLASS(PXOBJECT::TERRAIN);
	Desc.pMeshName = wstrName.c_str();
	Desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, wstrName, (CComponent**)&m_RigidBody, &Desc))) {
		assert(false);
	}
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

	SAFE_RELEASE(m_pDiffuseTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pMROTextureCom);
	SAFE_RELEASE(m_pMaskTextureCom);

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_RigidBody);
	SAFE_RELEASE(m_pModelCom);

	for (auto& pInstanceProp : m_InstanceProps)
		SAFE_RELEASE(pInstanceProp);
}
#ifdef _DEBUG
void CLand::Describe_Entity()
{

}
#endif // _DEBUG
