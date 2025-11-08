#include "pch.h"
#include "Dummy_Goblin_Head.h"

#include "GameInstance.h"
#include "DebugCamera.h"

CDummy_Goblin_Head::CDummy_Goblin_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy_Goblin_Head::CDummy_Goblin_Head(const CDummy_Goblin_Head& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CDummy_Goblin_Head::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_Goblin_Head::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CDummy_Goblin_Head::Priority_Update(_float fTimeDelta)
{

}

void CDummy_Goblin_Head::Update(_float fTimeDelta)
{
	m_pOwner->Get_Component<CModel>()->Play_Animation(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CDummy_Goblin_Head::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CDummy_Goblin_Head::Render()
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

HRESULT CDummy_Goblin_Head::Ready_Components()
{
	__super::Ready_Components(nullptr);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_GoblinHead_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pModelCom->Change_AnimationIndex(0, true, 0.4f, true);

	return S_OK;
}

HRESULT CDummy_Goblin_Head::Bind_ShaderResources()
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
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bRimLight", &m_bRimLight, sizeof(_bool)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimStrength", &m_fRimLightStrength, sizeof(_float)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimLightPower, sizeof(_float)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimLightColor, sizeof(_float3)))) {
	//	return E_FAIL;
	//}
	return S_OK;
}

CDummy_Goblin_Head* CDummy_Goblin_Head::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_Goblin_Head* pInstance = new CDummy_Goblin_Head(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_Goblin_Head");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_Goblin_Head::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_Goblin_Head* pInstance = new CDummy_Goblin_Head(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_Goblin_Head");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_Goblin_Head::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

void CDummy_Goblin_Head::Describe_Entity()
{
}
