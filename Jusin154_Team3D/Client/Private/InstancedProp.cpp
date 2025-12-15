#include "pch.h"
#include "InstancedProp.h"
#include "VIBuffer_Model_Instance.h"

CInstancedProp::CInstancedProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInstancedProp::CInstancedProp(const CInstancedProp& rhs)
	:CGameObject(rhs)
{
}

void CInstancedProp::Priority_Update(_float fTimeDelta)
{
}

void CInstancedProp::Update(_float fTimeDelta)
{
}

void CInstancedProp::Late_Update(_float fTimeDelta)
{
	if (m_isShake) {
		m_pVIBufferInstanceCom->Shake(fTimeDelta);
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

}

HRESULT CInstancedProp::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMesh; i++)
	{
		if (FAILED(m_pVIBufferInstanceCom->Bind_Matrial(m_pShaderCom, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pVIBufferInstanceCom->Render(i);
	}

	return S_OK;
}

HRESULT CInstancedProp::Render_Shadow(SHADOW eType)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
		return E_FAIL;
	}
	for (_uint i = 0; i < m_iNumMesh; i++)
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pVIBufferInstanceCom->Render(i);
	}

	return S_OK;
}

HRESULT CInstancedProp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInstancedProp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	m_iNumMesh = m_pVIBufferInstanceCom->Get_NumMesh();

	return S_OK;
}

HRESULT CInstancedProp::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		return E_FAIL;

	INSTANCE_PROP_DESC* pDesc = static_cast<INSTANCE_PROP_DESC*>(pArg);

	/* Com_VIBuffer_Instance_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->strPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pVIBufferInstanceCom))))
		return E_FAIL;

	/* Laod Instance Data */
	if(FAILED(Load_InstancedProp(pDesc->strInstanceDataPath.c_str())))
		return E_FAIL;

	m_isShake = pDesc->isShake;
	if (m_isShake)
		m_pVIBufferInstanceCom->Set_Shake_Value(pDesc->vRadius, pDesc->vSpeed);

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_INSTANCE_PROP_MODEL,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstancedProp::Bind_ShaderResources()
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

HRESULT CInstancedProp::Load_InstancedProp(const _char* pFilePath)
{
	ifstream FileIn(pFilePath, ios::binary);

	if (!FileIn.is_open())
	{
		MSG_BOX("Failed to Load InstancedProp File");
		return E_FAIL;
	}

	vector<_float4x4> WorldMatrices = {};

	_uint iNumWorldMatrix = 0;

	FileIn.read(reinterpret_cast<char*>(&iNumWorldMatrix), sizeof(_uint));
	WorldMatrices.reserve(iNumWorldMatrix);

	_float4x4 WorldMatrix = {};

	for (_uint i = 0; i < iNumWorldMatrix; ++i)
	{
		FileIn.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));

		WorldMatrices.push_back(WorldMatrix);
	}
	FileIn.close();

	if (FAILED(m_pVIBufferInstanceCom->Load_WorldData(WorldMatrices)))
	{
		MSG_BOX("Failed to Load World Matrices to VIBuffer Instance");
		return E_FAIL;
	}

	return S_OK;
}

CInstancedProp* CInstancedProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstancedProp* pInstance = new CInstancedProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInstancedProp");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CInstancedProp::Clone(void* pArg, CGameObject* pOwner)
{
	CInstancedProp* pInstance = new CInstancedProp(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInstancedProp");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CInstancedProp::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferInstanceCom);
}

#ifdef _DEBUG
void CInstancedProp::Describe_Entity()
{
}
#endif // _DEBUG
