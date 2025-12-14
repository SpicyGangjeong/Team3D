#include "pch.h"
#include "Goblin_Teleport.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"

CGoblin_Teleport::CGoblin_Teleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CGoblin_Teleport::CGoblin_Teleport(const CGoblin_Teleport& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CGoblin_Teleport::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/GoblinTeleport")))
		return E_FAIL;

	return S_OK;

}

HRESULT CGoblin_Teleport::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"GoblinTeleport";


	m_fDuration = 2.f;

	return S_OK;
}

void CGoblin_Teleport::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CGoblin_Teleport::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



}

void CGoblin_Teleport::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CGoblin_Teleport::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("Smoke");

	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_Component<CCharacter_Controller>()->Get_Position());

	pSmoke->Set_Visible(true);

	return S_OK;
}

HRESULT CGoblin_Teleport::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGoblin_Teleport::Ready_Child()
{
	return S_OK;
}

CGoblin_Teleport* CGoblin_Teleport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Teleport* pInstance = new CGoblin_Teleport(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Teleport");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CGoblin_Teleport::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Teleport* pInstance = new CGoblin_Teleport(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Teleport");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGoblin_Teleport::OnCollision(CGameObject* pOther, void* pDesc)
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

void CGoblin_Teleport::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CGoblin_Teleport::Describe_Entity()
{

}
#endif

HRESULT CGoblin_Teleport::Bind_ShaderResources()
{
	return S_OK;
}


