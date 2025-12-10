#include "pch.h"
#include "Goblin_Attack.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"

CGoblin_Attack::CGoblin_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CGoblin_Attack::CGoblin_Attack(const CGoblin_Attack& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CGoblin_Attack::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CGoblin_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/GoblinAttack")))
		return E_FAIL;


	m_wstrEffectName = L"TrollSwing";


	m_fDuration = 2.f;

	return S_OK;
}

void CGoblin_Attack::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CGoblin_Attack::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



}

void CGoblin_Attack::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CGoblin_Attack::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	m_pPT1 = Get_PartObject<CEffectParts>("Goblin_Particle");
	m_pPT2 = Get_PartObject<CEffectParts>("Goblin_Particle2");


	m_pPT1->Set_Visible(true);
	m_pPT2->Set_Visible(true);

	return S_OK;
}

HRESULT CGoblin_Attack::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGoblin_Attack::Ready_Child()
{
	return S_OK;
}

CGoblin_Attack* CGoblin_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Attack* pInstance = new CGoblin_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Attack");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CGoblin_Attack::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Attack* pInstance = new CGoblin_Attack(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Attack");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGoblin_Attack::OnCollision(CGameObject* pOther, void* pDesc)
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

void CGoblin_Attack::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CGoblin_Attack::Describe_Entity()
{

}
#endif

HRESULT CGoblin_Attack::Bind_ShaderResources()
{
	return S_OK;
}


