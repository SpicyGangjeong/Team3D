#include "pch.h"
#include "Troll_Shout.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Wand.h"
#include "Player.h"

CTroll_Shout::CTroll_Shout(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTroll_Shout::CTroll_Shout(const CTroll_Shout& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CTroll_Shout::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CTroll_Shout::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/TrollShout")))
		return E_FAIL;


	m_wstrEffectName = L"TrollShout";


	m_fDuration = 3.f;

	return S_OK;
}

void CTroll_Shout::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CTroll_Shout::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



}

void CTroll_Shout::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CTroll_Shout::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CCharacter_Controller* pCCT = m_pOwner->Get_Component<CCharacter_Controller>();

	_vector vPos = XMLoadFloat4(static_cast<_float4*>(pArg));
	vPos += m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK) * 2.f;

	_vector vFootPos = {};

	if (pCCT != nullptr)
	{
		vFootPos = pCCT->Get_FootPosition();

		CEditEffect* pShout_Blur = Get_PartObject<CEditEffect>("Shout_Blur");
		CEditEffect* pSmoke = Get_PartObject<CEditEffect>("Shout_Smoke");


		pShout_Blur->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
		pShout_Blur->Set_Visible(true);

		pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pSmoke->Set_Visible(true);
	}



	return S_OK;
}

HRESULT CTroll_Shout::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTroll_Shout::Ready_Child()
{
	return S_OK;
}

CTroll_Shout* CTroll_Shout::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Shout* pInstance = new CTroll_Shout(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Shout");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CTroll_Shout::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Shout* pInstance = new CTroll_Shout(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Shout");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Shout::OnCollision(CGameObject* pOther, void* pDesc)
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

void CTroll_Shout::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CTroll_Shout::Describe_Entity()
{

}
#endif

HRESULT CTroll_Shout::Bind_ShaderResources()
{
	return S_OK;
}


