#include "pch.h"
#include "Dummy_Goblin.h"

#include "GameInstance.h"
#include "DebugCamera.h"

CDummy_Goblin::CDummy_Goblin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject( pDevice, pContext )
{
}

CDummy_Goblin::CDummy_Goblin(const CDummy_Goblin& Prototype)
	: CGameObject( Prototype )
{
}

HRESULT CDummy_Goblin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_Goblin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CDummy_Goblin::Priority_Update(_float fTimeDelta)
{

}

void CDummy_Goblin::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CDummy_Goblin::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CDummy_Goblin::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//m_pShaderCom->Bind_SRV("g_NormalTexture", nullptr);
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

HRESULT CDummy_Goblin::Ready_Components()
{
	__super::Ready_Components(nullptr);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_GoblinBody_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pModelCom->Change_AnimationIndex(0, true, 0.4f, true);

	return S_OK;
}

HRESULT CDummy_Goblin::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))){
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))){
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))){
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

CDummy_Goblin* CDummy_Goblin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_Goblin* pInstance = new CDummy_Goblin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_Goblin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_Goblin::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_Goblin* pInstance = new CDummy_Goblin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_Goblin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_Goblin::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

void CDummy_Goblin::Describe_Entity()
{
}
