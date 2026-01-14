#include "pch.h"
#include "BroomRace_Bubble.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "Goblin_BattleAxe.h"
#include "TrailObject.h"

CBroomRace_Bubble::CBroomRace_Bubble(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CBroomRace_Bubble::CBroomRace_Bubble(const CBroomRace_Bubble& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CBroomRace_Bubble::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/BroomBubble")))
		return E_FAIL;

	return S_OK;

}

HRESULT CBroomRace_Bubble::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;



	m_fDuration = 99999.f;



	return S_OK;
}

void CBroomRace_Bubble::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CBroomRace_Bubble::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CBroomRace_Bubble::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);

	CEffectParts* pBubble = Get_PartObject<CEffectParts>("Bubble");

	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = pBubble->Get_WorldPostion();
		ON_COLLISION_INFO CollisionInfo = MonsterSweepTarget(vStartPos, vEndPos, 0.002f);

		OnCollision(this, &CollisionInfo);

	}
}

HRESULT CBroomRace_Bubble::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_float4* pPos = static_cast<_float4*>(pArg);

	m_vStartPos = *pPos;

	_vector vPosVector = XMLoadFloat4(pPos);

	CEffectParts* pBubble = Get_PartObject<CEffectParts>("Bubble");
	CEffectParts* pBubble_Distortion = Get_PartObject<CEffectParts>("Bubble_Distortion");
	CEffectParts* pHit_Bubble = Get_PartObject<CEffectParts>("Hit_Bubble");

	pBubble->Set_Visible(true);
	pBubble_Distortion->Set_Visible(true);

	pBubble->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);
	pBubble_Distortion->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);
	pHit_Bubble->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);

	
	return S_OK;
}

HRESULT CBroomRace_Bubble::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroomRace_Bubble::Ready_Child()
{
	return S_OK;
}

CBroomRace_Bubble* CBroomRace_Bubble::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroomRace_Bubble* pInstance = new CBroomRace_Bubble(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroomRace_Bubble");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CBroomRace_Bubble::Clone(void* pArg, CGameObject* pOwner)
{
	CBroomRace_Bubble* pInstance = new CBroomRace_Bubble(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroomRace_Bubble");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroomRace_Bubble::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;


	CEffectParts* pBubble = Get_PartObject<CEffectParts>("Bubble");
	CEffectParts* pBubble_Distortion = Get_PartObject<CEffectParts>("Bubble_Distortion");

	pBubble->Set_Visible(false);
	pBubble_Distortion->Set_Visible(false);

	m_fDuration = 3.f;
	m_fAccTime = 0.f;

	CEffectParts* pHit_Bubble = Get_PartObject<CEffectParts>("Hit_Bubble");

	pHit_Bubble->Set_Visible(true);

}

void CBroomRace_Bubble::Free()
{
	__super::Free();


}
#ifdef _DEBUG
void CBroomRace_Bubble::Describe_Entity()
{

}
#endif

HRESULT CBroomRace_Bubble::Bind_ShaderResources()
{
	return S_OK;
}


