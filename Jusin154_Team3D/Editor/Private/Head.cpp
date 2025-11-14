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

	return S_OK;
}

void CHead::Priority_Update(_float fTimeDelta)
{

}

void CHead::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CHead::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CHead::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CHead::Ready_Components()
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

CHead* CHead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHead* pInstance = new CHead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHead");
		SAFE_RELEASE(pInstance);
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
		SAFE_RELEASE(pInstance);
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
