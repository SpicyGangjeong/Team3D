#include "pch.h"
#include "Ranrok_PropHit.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_PropHit::CRanrok_PropHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_PropHit::CRanrok_PropHit(const CRanrok_PropHit& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_PropHit::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokPropHit")))
		return E_FAIL;

	return S_OK;

}

HRESULT CRanrok_PropHit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"RanrokPropHit";



	m_fDuration = 5.f;
	

	return S_OK;
}

void CRanrok_PropHit::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_PropHit::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);




	
}

void CRanrok_PropHit::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_PropHit::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_float4 vHitPos = *static_cast<_float4*>(pArg);

	 /* 초기 객체 비지블*/

	CEffectParts* Hit_PT = Get_PartObject<CEffectParts>("Hit_PT");
	CEffectParts* pPulse_Purple = Get_PartObject<CEffectParts>("Pulse_Purple");


	Hit_PT->Set_Visible(true);
	pPulse_Purple->Set_Visible(true);


	m_pGameInstance->Sound_Play(SOUND::SD_KIND::RANROK_28, SD_CHANNEL_GROUP::EFFECT, false, 0.6f);
	 /* 초기 객체 위치 초기화*/

	_vector vHittingPos = XMLoadFloat4(&vHitPos);
	_vector vOwnerPos = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::POSITION);

	Hit_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHittingPos);
	pPulse_Purple->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);

	return S_OK;
}

HRESULT CRanrok_PropHit::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_PropHit::Ready_Child()
{
	return S_OK;
}

CRanrok_PropHit* CRanrok_PropHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_PropHit* pInstance = new CRanrok_PropHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_PropHit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_PropHit::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_PropHit* pInstance = new CRanrok_PropHit(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_PropHit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_PropHit::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

void CRanrok_PropHit::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);


}
#ifdef _DEBUG
void CRanrok_PropHit::Describe_Entity()
{

}
#endif

HRESULT CRanrok_PropHit::Bind_ShaderResources()
{
	return S_OK;
}


