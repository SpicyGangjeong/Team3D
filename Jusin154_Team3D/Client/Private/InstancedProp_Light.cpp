#include "pch.h"
#include "InstancedProp_Light.h"
#include "VIBuffer_Model_Instance.h"

CInstancedProp_Light::CInstancedProp_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInstancedProp_Light::CInstancedProp_Light(const CInstancedProp_Light& rhs)
	:CGameObject(rhs)
{
}

void CInstancedProp_Light::Priority_Update(_float fTimeDelta)
{

}

void CInstancedProp_Light::Update(_float fTimeDelta)
{
}

void CInstancedProp_Light::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

	if (m_bLightOn)
		m_pGameInstance->Add_RenderGroup(RENDER::BLOOM, this);
}

HRESULT CInstancedProp_Light::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	for (_uint i = 0; i < m_iNumMesh; i++)
	{
		if (m_iGlassMeshIndex == i)
		{
			if (FAILED(m_pVIBufferInstanceCom->Bind_Matrial(m_pShaderCom, m_iGlassMeshIndex))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pMaskTextureCom->Get_SRV(0)))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_SRV("g_GlassTexture", m_pGlassTextureCom->Get_SRV(0)))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlassRatio", &m_fGlassRatio, sizeof(_float)))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_WORLDMODLE_INSTANCE::LIGHT)))) {
				return E_FAIL;
			}

		}
		else
		{
			if (FAILED(m_pVIBufferInstanceCom->Bind_Matrial(m_pShaderCom, i))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_WORLDMODLE_INSTANCE::DEFAULT)))) {
				return E_FAIL;
			}
		}

		m_pVIBufferInstanceCom->Render(i);
	}

	return S_OK;
}

HRESULT CInstancedProp_Light::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomStrength", &m_fBloomStrength, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_SRV("g_EmissiveTexture", m_pEmissiveTextureCom->Get_SRV(0))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_WORLDMODLE_INSTANCE::BLOOM)))) {
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferInstanceCom->Render(m_iGlassMeshIndex))) {
		return E_FAIL;
	}

	return S_OK;
}

void CInstancedProp_Light::Toggle_Light()
{
	m_bLightOn = !m_bLightOn;
}

HRESULT CInstancedProp_Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInstancedProp_Light::Initialize(void* pArg)
{
	INSTANCE_PROP_LIGHT_DESC* pDesc = static_cast<INSTANCE_PROP_LIGHT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bEnableRigidbody = pDesc->bEnableRigidbody;
	m_iGlassMeshIndex = pDesc->iGlassMeshIndex;
	m_bLightOn = false;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_iNumMesh = m_pVIBufferInstanceCom->Get_NumMesh();

	if(m_bEnableRigidbody)
	{
		if (FAILED(ReadyForPhysX()))
			return E_FAIL;
	}

	if (FAILED(Load_InstancedProp(pDesc->strInstanceDataPath.c_str())))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstancedProp_Light::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		return E_FAIL;

	INSTANCE_PROP_LIGHT_DESC* pDesc = static_cast<INSTANCE_PROP_LIGHT_DESC*>(pArg);

	/* Com_VIBuffer_Instance_Model */
	if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, pDesc->strPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pVIBufferInstanceCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_INSTANCE_PROP_MODEL,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Mask"),
		reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Base"),
		reinterpret_cast<CComponent**>(&m_pGlassTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Emissive"),
		reinterpret_cast<CComponent**>(&m_pEmissiveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstancedProp_Light::Bind_ShaderResources()
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

HRESULT CInstancedProp_Light::ReadyForPhysX()
{
	if (FAILED(m_pVIBufferInstanceCom->Ready_PhysXMeshes(NEXT_LEVEL))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInstancedProp_Light::Load_InstancedProp(const _char* pFilePath)
{
	ifstream FileIn(pFilePath, ios::binary);

	if (!FileIn.is_open())
	{
		MSG_BOX("Failed to Load InstancedProp File");
		return E_FAIL;
	}

	vector<_float4x4> WorldMatrices = {};

#pragma region FOR RIGIDBODY
	CRigidBody_Static::RIGIDBODY_STATIC_DESC Desc{};

	Desc.iSubKind = ENUM_CLASS(PXOBJECT::TERRAIN);

	vector<_wstring> RigidBodyTags;


	for (_uint i = 0; i < m_iNumMesh; ++i)
	{
		RigidBodyTags.push_back(CMyTools::ToWstring(m_pVIBufferInstanceCom->Get_MeshName(i)) + to_wstring(i));
	}
#pragma endregion

	FileIn.read(reinterpret_cast<char*>(&m_iNumInstacne), sizeof(_uint));
	WorldMatrices.reserve(m_iNumInstacne);

	_float4x4 WorldMatrix = {};

	for (_uint i = 0; i < m_iNumInstacne; ++i)
	{
		FileIn.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));

		WorldMatrices.push_back(WorldMatrix);

		if (m_bEnableRigidbody)
		{
			for (_uint i = 0; i < m_iNumMesh; ++i)
			{
				CRigidBody_Static* pRigidBody = { nullptr };
				CRigidBody_Static::RIGIDBODY_STATIC_DESC Desc = {};
				Desc.pMeshName = RigidBodyTags[i].c_str();
				Desc.pWorldMatrix = &WorldMatrix;
				if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, RigidBodyTags[i], (CComponent**)&pRigidBody, &Desc))) {
					return E_FAIL;
				}

				m_RigidBody.push_back(pRigidBody);
			}

		}
	}
	FileIn.close();

	if (FAILED(m_pVIBufferInstanceCom->Load_WorldData(WorldMatrices)))
	{
		MSG_BOX("Failed to Load World Matrices to VIBuffer Instance");
		return E_FAIL;
	}

	return S_OK;
}

CInstancedProp_Light* CInstancedProp_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstancedProp_Light* pInstance = new CInstancedProp_Light(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInstancedProp_Light");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CInstancedProp_Light::Clone(void* pArg, CGameObject* pOwner)
{
	CInstancedProp_Light* pInstance = new CInstancedProp_Light(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInstancedProp_Light");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CInstancedProp_Light::Free()
{
	__super::Free();

	for (auto& pRigidBody : m_RigidBody)
	{
		SAFE_RELEASE(pRigidBody);
	}

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferInstanceCom);
	SAFE_RELEASE(m_pGlassTextureCom);
	SAFE_RELEASE(m_pMaskTextureCom);
	SAFE_RELEASE(m_pEmissiveTextureCom);
}

#ifdef _DEBUG
void CInstancedProp_Light::Describe_Entity()
{
}
#endif // _DEBUG



