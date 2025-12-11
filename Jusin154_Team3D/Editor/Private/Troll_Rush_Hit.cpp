#include "pch.h"
#include "Troll_Rush_Hit.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Wand.h"
#include "Player.h"

CTroll_Rush_Hit::CTroll_Rush_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTroll_Rush_Hit::CTroll_Rush_Hit(const CTroll_Rush_Hit& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CTroll_Rush_Hit::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CTroll_Rush_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/TrollRush")))
		return E_FAIL;


	m_wstrEffectName = L"Troll_Nomal_Smoke";


	m_fDuration = 3.f;

	return S_OK;
}

void CTroll_Rush_Hit::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CTroll_Rush_Hit::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



}

void CTroll_Rush_Hit::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CTroll_Rush_Hit::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	_float4 vPos = *static_cast<_float4*>(pArg); // 위치 넘기기

	CEditEffect* pSmoke = Get_PartObject<CEditEffect>("HitSmoke");

	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&vPos));

	pSmoke->Set_Visible(true);

	return S_OK;
}

HRESULT CTroll_Rush_Hit::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTroll_Rush_Hit::Ready_Child()
{
	return S_OK;
}

CTroll_Rush_Hit* CTroll_Rush_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Rush_Hit* pInstance = new CTroll_Rush_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Rush_Hit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CTroll_Rush_Hit::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Rush_Hit* pInstance = new CTroll_Rush_Hit(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Rush_Hit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Rush_Hit::OnCollision(CGameObject* pOther, void* pDesc)
{
	_int iIndex = CollisionCheck();

	if (iIndex < 0)
		return;

	if (m_isCollisionEnter == true)
		return;

	m_isCollisionEnter = true;

	_vector vPos = XMVectorSet(m_Hitbuffer.touches[iIndex].position.x, m_Hitbuffer.touches[iIndex].position.y, m_Hitbuffer.touches[iIndex].position.z, 1.f);


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}


}

void CTroll_Rush_Hit::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CTroll_Rush_Hit::Describe_Entity()
{

}
#endif

HRESULT CTroll_Rush_Hit::Bind_ShaderResources()
{
	return S_OK;
}


