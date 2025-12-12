#include "pch.h"
#include "Troll_Rush_Hit.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CTroll_Rush_Hit::CTroll_Rush_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTroll_Rush_Hit::CTroll_Rush_Hit(const CTroll_Rush_Hit& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
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


	m_fDuration = 3.5f;

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


	_vector vPos =  m_pOwner->Get_Component<CCharacter_Controller>()->Get_Position();

	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("HitSmoke");
	CEffectParts* pRock_PT_35 = Get_PartObject<CEffectParts>("Rock_PT_35");

	_vector vLook = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	vPos += vLook * 2.f;

	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pRock_PT_35->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

	pSmoke->Set_Visible(true);
	pRock_PT_35->Set_Visible(true);


	CPlayer* pPlayer = static_cast<CPlayer*>(m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first);


	if (pPlayer == nullptr)
		return E_FAIL;

	_float fDistance = XMVectorGetX(XMVector4Length(pPlayer->Get_WorldPostion() - Get_WorldPostion()));

	_float fShakeValue = clamp(40.f / fDistance, 3.f, 9.f);

	pPlayer->Start_CameraShake(0.8f, fShakeValue);

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

	SAFE_RELEASE(m_pInfoInstance);
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


