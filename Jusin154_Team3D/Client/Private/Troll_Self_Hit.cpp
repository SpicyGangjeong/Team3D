#include "pch.h"
#include "Troll_Self_Hit.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CTroll_Self_Hit::CTroll_Self_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTroll_Self_Hit::CTroll_Self_Hit(const CTroll_Self_Hit& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CTroll_Self_Hit::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/TrollSelfHit")))
		return E_FAIL;

	return S_OK;

}

HRESULT CTroll_Self_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"TrollSelfHit";


	m_fDuration = 3.5f;

	return S_OK;
}

void CTroll_Self_Hit::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CTroll_Self_Hit::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



}

void CTroll_Self_Hit::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CTroll_Self_Hit::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	_vector vPos =  m_pOwner->Get_Component<CCharacter_Controller>()->Get_HeadPosition();

	CEffectParts* pHit0 = Get_PartObject<CEffectParts>("Hit0");
	CEffectParts* pHit1 = Get_PartObject<CEffectParts>("Hit1");


	pHit0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pHit1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

	pHit0->Set_Visible(true);
	pHit1->Set_Visible(true);


	return S_OK;
}

HRESULT CTroll_Self_Hit::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTroll_Self_Hit::Ready_Child()
{
	return S_OK;
}

CTroll_Self_Hit* CTroll_Self_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Self_Hit* pInstance = new CTroll_Self_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Self_Hit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CTroll_Self_Hit::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Self_Hit* pInstance = new CTroll_Self_Hit(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Self_Hit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Self_Hit::OnCollision(CGameObject* pOther, void* pDesc)
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

void CTroll_Self_Hit::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}
#ifdef _DEBUG
void CTroll_Self_Hit::Describe_Entity()
{

}
#endif

HRESULT CTroll_Self_Hit::Bind_ShaderResources()
{
	return S_OK;
}


