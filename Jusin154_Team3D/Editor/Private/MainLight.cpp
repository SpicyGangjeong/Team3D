#include "pch.h"
#include "MainLight.h"

#include "GameInstance.h"
#include "DebugCamera.h"

CMainLight::CMainLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMainLight::CMainLight(const CMainLight& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CMainLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainLight::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::LOOK, XMVectorSet(1.f, -1.f, 1.f, 0.f));

	return S_OK;
}

void CMainLight::Priority_Update(_float fTimeDelta)
{

}

void CMainLight::Update(_float fTimeDelta)
{

}

void CMainLight::Late_Update(_float fTimeDelta)
{
}

HRESULT CMainLight::Render()
{
	return S_OK;
}



HRESULT CMainLight::Ready_Components()
{
	__super::Ready_Components(nullptr);

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 0.f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 0.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.pDirection = m_pTransformCom->Get_StatePtr(STATE::LOOK);
	LightDesc.iLevel = NEXT_LEVEL;

	/* Com_Light*/
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMainLight::Bind_ShaderResources()
{
	return S_OK;
}



CMainLight* CMainLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainLight* pInstance = new CMainLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMainLight::Clone(void* pArg, CGameObject* pOwner)
{
	CMainLight* pInstance = new CMainLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMainLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMainLight::Free()
{
	__super::Free();

	Safe_Release(m_pLightCom);
}

void CMainLight::Describe_Entity()
{
}
