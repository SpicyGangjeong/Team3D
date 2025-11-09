#include "pch.h"
#include "TombProtector.h"

#include "GameInstance.h"
#include "DebugCamera.h"

CTombProtector::CTombProtector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CTombProtector::CTombProtector(const CTombProtector& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CTombProtector::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTombProtector::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	m_pModelCom->Set_AnimationIndex(0);

	return S_OK;
}

void CTombProtector::Priority_Update(_float fTimeDelta)
{

}

void CTombProtector::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CTombProtector::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CTombProtector::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CTombProtector::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_TombProtector_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTombProtector::Bind_ShaderResources()
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
	return S_OK;
}

CTombProtector* CTombProtector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTombProtector* pInstance = new CTombProtector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTombProtector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTombProtector::Clone(void* pArg, CGameObject* pOwner)
{
	CTombProtector* pInstance = new CTombProtector(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTombProtector");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTombProtector::Free()
{
	__super::Free();
}

void CTombProtector::Describe_Entity()
{
}
