#include "pch.h"
#include "Hair.h"

#include "GameInstance.h"
#include "RootModelPart.h"

CHair::CHair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelParts(pDevice, pContext)
{
}

CHair::CHair(const CHair& Prototype)
	: CModelParts(Prototype)
{
}

HRESULT CHair::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHair::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CHair::Priority_Update(_float fTimeDelta)
{

}

void CHair::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CHair::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CHair::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CHair::Ready_Components()
{
	if (m_strModelPrototypeTag != L"")
	{
		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
			reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

	}

	return S_OK;
}

CHair* CHair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHair* pInstance = new CHair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHair::Clone(void* pArg, CGameObject* pOwner)
{
	CHair* pInstance = new CHair(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHair::Free()
{
	__super::Free();

}

void CHair::Describe_Entity()
{
}
