#include "pch.h"
#include "Head.h"

#include "GameInstance.h"
#include "RootModelPart.h"

CHead::CHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelParts(pDevice, pContext)
{
}

CHead::CHead(const CHead& Prototype)
	: CModelParts(Prototype)
{
}

HRESULT CHead::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHead::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	dynamic_cast<CRootModelPart*>(m_pOwner)->Push_ModelParts(this);

	return S_OK;
}

void CHead::Priority_Update(_float fTimeDelta)
{

}

void CHead::Update(_float fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CHead::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CHead::Render()
{
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

HRESULT CHead::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CHead* CHead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHead* pInstance = new CHead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHead::Clone(void* pArg, CGameObject* pOwner)
{
	CHead* pInstance = new CHead(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHead::Free()
{
	__super::Free();

}

void CHead::Describe_Entity()
{
}
