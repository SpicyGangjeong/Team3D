#include "pch.h"
#include "NomalJap.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Dummy_PhysXEffectHitBox.h"


CNomalJap::CNomalJap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CNomalJap::CNomalJap(const CNomalJap& rhs)
	: CEffect_Container(rhs)
{

}

HRESULT CNomalJap::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CNomalJap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	Load_Package("../Bin/Resources/Data/Effect/Package/Jap");

	m_wstrEffectName = L"Nomal_Jap";

	m_PartObjects.begin()->second->Set_Visible(true);
	m_PartObjects.begin()->second->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	m_fDuration = 5.f;

	return S_OK;
}

void CNomalJap::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CNomalJap::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);
}

void CNomalJap::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CNomalJap::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNomalJap::Ready_Child()
{
	CDummy_PhysXEffectHitBox::PHYSXDUMMY_DESC Desc{};

	m_pTransformCom->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	XMStoreFloat3(&Desc.vPos, m_pOwner->Get_WorldPostion() + XMVectorSet(0.f, 0.f, 1.f, 0.f));

	Desc.vRotRPY = { 0.f, 0.f, 0.f };
	Desc.iSubKind = 70;
	Desc.vDeltaPos = { 0.f, 0.f, 1.f };
	Desc.vLifeTime = { 0.f, 2.f };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXEffectHitBox>(g_iStaticLevel, CURRENT_LEVEL, LAYER_HITBOX, &Desc, this , &m_pPhysHitBox))) {
		assert(false);
		return E_FAIL;
	}

	return S_OK;
}

CNomalJap* CNomalJap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNomalJap* pInstance = new CNomalJap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNomalJap");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CNomalJap::Clone(void* pArg, CGameObject* pOwner)
{
	CNomalJap* pInstance = new CNomalJap(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNomalJap");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CNomalJap::OnCollision(CGameObject* pOther , void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO*	CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, CollisionDesc->vWorldPos);
	}

	m_PartObjects.begin()->second->Set_Visible(false);

	m_pPhysHitBox->Set_Dead();
}

void CNomalJap::Free()
{
	__super::Free();

	//if(m_pPhysHitBox != nullptr)
	//	if (m_pPhysHitBox->Get_Depth() == false)
	//		SAFE_RELEASE(m_pPhysHitBox);

}

void CNomalJap::Describe_Entity()
{

}

HRESULT CNomalJap::Bind_ShaderResources()
{
	return S_OK;
}


