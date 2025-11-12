#include "pch.h"
#include "Body.h"

#include "GameInstance.h"
#include "RootModelPart.h"

CModelParts::CModelParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CModelParts::CModelParts(const CModelParts& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CModelParts::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModelParts::Initialize(void* pArg)
{
	PARTS_OBJECT_DESC* pDesc = static_cast<PARTS_OBJECT_DESC*>(pArg);

	m_pParentTransformCom = pDesc->pParentTransform;
	if (pDesc->pModelPrototypeTag)
	{
		m_strModelPrototypeTag = pDesc->pModelPrototypeTag;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CModelParts::Priority_Update(_float fTimeDelta)
{
}

void CModelParts::Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
}

void CModelParts::Late_Update(_float fTimeDelta)
{
}

HRESULT CModelParts::Render()
{
	if (!m_pModelCom)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

void CModelParts::Change_Model(const _wchar* Prototype)
{
	if (m_pModelCom)
	{
		Remove_Component<CModel>();
		SAFE_RELEASE(m_pModelCom);
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, Prototype,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return;
}

HRESULT CModelParts::Ready_Components()
{
	__super::Ready_Components(nullptr);

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelParts::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix))) {
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

void CModelParts::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

void CModelParts::Describe_Entity()
{
}
