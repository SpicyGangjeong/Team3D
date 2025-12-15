#include "pch.h"
#include "Troll_Nomal_Smoke.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"

CTroll_Nomal_Smoke::CTroll_Nomal_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTroll_Nomal_Smoke::CTroll_Nomal_Smoke(const CTroll_Nomal_Smoke& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CTroll_Nomal_Smoke::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/TrollNomalSmoke")))
		return E_FAIL;

	return S_OK;

}

HRESULT CTroll_Nomal_Smoke::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"Troll_Nomal_Smoke";


	m_fDuration = 3.5f;

	return S_OK;
}

void CTroll_Nomal_Smoke::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CTroll_Nomal_Smoke::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



}

void CTroll_Nomal_Smoke::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CTroll_Nomal_Smoke::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	_float4 vPos = *static_cast<_float4*>(pArg); // 위치 넘기기

	CEffectParts* pRockPT = Get_PartObject<CEffectParts>("Rock_PT_15"); 
	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("NomalSmoke");


	pRockPT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&vPos) + XMVectorSet(0.f , 0.2f ,0.f, 0.f) );
	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&vPos) + XMVectorSet(0.f, 0.2f, 0.f, 0.f));

	pRockPT->Set_Visible(true);
	pSmoke->Set_Visible(true);

	return S_OK;
}

HRESULT CTroll_Nomal_Smoke::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTroll_Nomal_Smoke::Ready_Child()
{
	return S_OK;
}

CTroll_Nomal_Smoke* CTroll_Nomal_Smoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Nomal_Smoke* pInstance = new CTroll_Nomal_Smoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Nomal_Smoke");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CTroll_Nomal_Smoke::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Nomal_Smoke* pInstance = new CTroll_Nomal_Smoke(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Nomal_Smoke");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Nomal_Smoke::OnCollision(CGameObject* pOther, void* pDesc)
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

void CTroll_Nomal_Smoke::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CTroll_Nomal_Smoke::Describe_Entity()
{

}
#endif

HRESULT CTroll_Nomal_Smoke::Bind_ShaderResources()
{
	return S_OK;
}


