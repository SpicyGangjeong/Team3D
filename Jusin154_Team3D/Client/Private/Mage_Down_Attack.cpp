#include "pch.h"
#include "Mage_Down_Attack.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CMage_Down_Attack::CMage_Down_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CMage_Down_Attack::CMage_Down_Attack(const CMage_Down_Attack& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CMage_Down_Attack::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CMage_Down_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/GoblinMage/DownAttack")))
		return E_FAIL;


	m_wstrEffectName = L"DownAttack";


	m_fDuration = 2.F;

	return S_OK;
}

void CMage_Down_Attack::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CMage_Down_Attack::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

}

void CMage_Down_Attack::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CMage_Down_Attack::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_vector vPos = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::POSITION);

	vPos += m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK) * 3.f;
	vPos += XMVectorSet(0.f, 0.2f, 0.f, 0.f);

	CEffectParts* pBottomPT0 = Get_PartObject<CEffectParts>("BottomPT0");
	CEffectParts* pBottomPT1 = Get_PartObject<CEffectParts>("BottomPT1"); 
	CEffectParts* pDown_R = Get_PartObject<CEffectParts>("Down_R");
	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("Smoke");
	CEffectParts* pRound_PT = Get_PartObject<CEffectParts>("Round_PT");

	pBottomPT0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pBottomPT1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pDown_R->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pRound_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

	pBottomPT0->Set_Visible(true);
	pBottomPT1->Set_Visible(true);
	pDown_R->Set_Visible(true);
	pSmoke->Set_Visible(true);
	pRound_PT->Set_Visible(true);


	CPlayer* pPlayer = static_cast<CPlayer*>(m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first);


	if (pPlayer == nullptr)
		return E_FAIL;

	_float fDistance = XMVectorGetX(XMVector4Length(pPlayer->Get_WorldPostion() - Get_WorldPostion()));

	_float fShakeValue = clamp(30.f / fDistance, 1.f, 5.f);

	pPlayer->Start_CameraShake(0.6f, fShakeValue);

	return S_OK;
}

HRESULT CMage_Down_Attack::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMage_Down_Attack::Ready_Child()
{
	return S_OK;
}

CMage_Down_Attack* CMage_Down_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMage_Down_Attack* pInstance = new CMage_Down_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMage_Down_Attack");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CMage_Down_Attack::Clone(void* pArg, CGameObject* pOwner)
{
	CMage_Down_Attack* pInstance = new CMage_Down_Attack(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMage_Down_Attack");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMage_Down_Attack::OnCollision(CGameObject* pOther, void* pDesc)
{

}

void CMage_Down_Attack::Free()
{
	__super::Free();
	SAFE_RELEASE(m_pInfoInstance);
}
#ifdef _DEBUG
void CMage_Down_Attack::Describe_Entity()
{

}
#endif

HRESULT CMage_Down_Attack::Bind_ShaderResources()
{
	return S_OK;
}


