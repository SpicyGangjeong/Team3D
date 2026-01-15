#include "pch.h"
#include "Troll_Throw.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"

CTroll_Throw::CTroll_Throw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTroll_Throw::CTroll_Throw(const CTroll_Throw& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CTroll_Throw::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/TrollThrow")))
		return E_FAIL;

	return S_OK;

}

HRESULT CTroll_Throw::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"TrollThrow";


	m_fDuration = 3.5f;

	return S_OK;
}

void CTroll_Throw::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CTroll_Throw::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



}

void CTroll_Throw::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CTroll_Throw::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	_float4 vPos = *static_cast<_float4*>(pArg); // 위치 넘기기

	CEffectParts* pRockPT = Get_PartObject<CEffectParts>("Rock_PT"); 
	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("NomalSmoke");


	pRockPT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&vPos) + XMVectorSet(0.f , 0.2f ,0.f, 0.f) );
	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&vPos) + XMVectorSet(0.f, 0.2f, 0.f, 0.f));

	pRockPT->Set_Visible(true);
	pSmoke->Set_Visible(true);

	return S_OK;
}

HRESULT CTroll_Throw::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTroll_Throw::Ready_Child()
{
	return S_OK;
}

CTroll_Throw* CTroll_Throw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Throw* pInstance = new CTroll_Throw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Throw");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CTroll_Throw::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Throw* pInstance = new CTroll_Throw(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Throw");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Throw::OnCollision(CGameObject* pOther, void* pDesc)
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

void CTroll_Throw::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CTroll_Throw::Describe_Entity()
{

}
#endif

HRESULT CTroll_Throw::Bind_ShaderResources()
{
	return S_OK;
}


