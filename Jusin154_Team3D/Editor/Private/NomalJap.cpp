#include "pch.h"
#include "NomalJap.h"

#include "GameInstance.h"
#include "EditEffect.h"


CNomalJap::CNomalJap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CNomalJap::CNomalJap(const CNomalJap& rhs)
	: CEffect_Container(rhs)
{

}

HRESULT CNomalJap::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CNomalJap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;


	m_wstrEffectName = L"Nomal_Jap";


	return S_OK;
}

void CNomalJap::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CNomalJap::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CNomalJap::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CNomalJap::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNomalJap::Ready_Child()
{

	//CPartObject::PARTOBJECT_DESC PartsDesc{};

	//PartsDesc.pParentTransform = m_pTransformCom;

	//if (FAILED(Add_PartObject<CEditEffect>("EffectObject" + to_string(m_iNumPart++), ENUM_CLASS(LEVEL::EFFECT), &m_pEditEffect, &PartsDesc)))
	//	return E_FAIL;

	return S_OK;
}

CNomalJap* CNomalJap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNomalJap* pInstance = new CNomalJap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNomalJap");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CNomalJap::Free()
{
	__super::Free();

}

CGameObject* CNomalJap::Clone(void* pArg, CGameObject* pOwner)
{
	CNomalJap* pInstance = new CNomalJap(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNomalJap");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CNomalJap::Describe_Entity()
{

}

HRESULT CNomalJap::Bind_ShaderResources()
{
	return S_OK;
}
