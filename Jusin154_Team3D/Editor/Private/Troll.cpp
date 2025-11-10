#include "pch.h"
#include "Troll.h"

#include "GameInstance.h"
#include "DebugCamera.h"

CTroll::CTroll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CTroll::CTroll(const CTroll& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CTroll::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTroll::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pModelCom->Set_AnimationIndex(0);

	return S_OK;
}

void CTroll::Priority_Update(_float fTimeDelta)
{

}

void CTroll::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CTroll::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CTroll::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CTroll::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Troll_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTroll::Bind_ShaderResources()
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

CTroll* CTroll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll* pInstance = new CTroll(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTroll::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll* pInstance = new CTroll(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTroll::Free()
{
	__super::Free();
}

void CTroll::Describe_Entity()
{
}
