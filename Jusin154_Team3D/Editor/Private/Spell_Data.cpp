#include "pch.h"
#include "Spell_Data.h"
#include "GameObject.h"

CSpell_Data::CSpell_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CSpell_Data::CSpell_Data(const CSpell_Data& rhs)
	:CGameObject(rhs)
{
}

HRESULT CSpell_Data::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Data::Initialize(void* pArg)
{
	return S_OK;
}

void CSpell_Data::Priority_Update(_float fTimeDelta)
{
}

void CSpell_Data::Update(_float fTimeDelta)
{
}

void CSpell_Data::Late_Update(_float fTimeDelta)
{
}

HRESULT CSpell_Data::Render()
{
	return S_OK;
}

_vector CSpell_Data::Get_WorldPostion()
{
	return _vector();
}

HRESULT CSpell_Data::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CSpell_Data::Ready_Components(void* pArg)
{
	return S_OK;
}

CSpell_Data* CSpell_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Data* pInstance = new CSpell_Data(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Data::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Data* pInstance = new CSpell_Data(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Data::Free()
{
	__super::Free();

}

void CSpell_Data::Describe_Entity()
{
}
