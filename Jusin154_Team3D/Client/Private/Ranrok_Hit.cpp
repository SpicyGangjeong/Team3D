#include "pch.h"
#include "Ranrok_Hit.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_Hit::CRanrok_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Hit::CRanrok_Hit(const CRanrok_Hit& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Hit::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokHit")))
		return E_FAIL;

	return S_OK;

}

HRESULT CRanrok_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"RanrokHit";



	m_fDuration = 5.f;
	

	return S_OK;
}

void CRanrok_Hit::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_Hit::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);




	
}

void CRanrok_Hit::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_Hit::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_float4 vHitPos = *static_cast<_float4*>(pArg);

	 /* 초기 객체 비지블*/

	CEffectParts* pHit_PT_R = Get_PartObject<CEffectParts>("Hit_PT_R");
	CEffectParts* pRock_PT = Get_PartObject<CEffectParts>("Rock_PT");
	CEffectParts* pSplatter_Hit = Get_PartObject<CEffectParts>("Splatter_Hit");


	pSplatter_Hit->Set_Visible(true);
	pHit_PT_R->Set_Visible(true);
	pRock_PT->Set_Visible(true);


	 /* 초기 객체 위치 초기화*/

	_vector vHittingPos = XMLoadFloat4(&vHitPos);

	pSplatter_Hit->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHittingPos);
	pHit_PT_R->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHittingPos);
	pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHittingPos);

	return S_OK;
}

HRESULT CRanrok_Hit::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_Hit::Ready_Child()
{
	return S_OK;
}

CRanrok_Hit* CRanrok_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Hit* pInstance = new CRanrok_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Hit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Hit::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Hit* pInstance = new CRanrok_Hit(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Hit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Hit::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

void CRanrok_Hit::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);


}
#ifdef _DEBUG
void CRanrok_Hit::Describe_Entity()
{

}
#endif

HRESULT CRanrok_Hit::Bind_ShaderResources()
{
	return S_OK;
}


