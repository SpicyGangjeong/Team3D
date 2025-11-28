#include "pch.h"
#include "Bombard.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Dummy_PhysXEffectHitBox.h"


CBombard::CBombard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CBombard::CBombard(const CBombard& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CBombard::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CBombard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Bombard")))
		return E_FAIL;


	m_wstrEffectName = L"Bombard";

	m_pLight_Projectile = Get_PartObject<CEditEffect>("Bombard_Projectile");
	SAFE_ADDREF(m_pLight_Projectile);

	m_fDuration = 2.5f;

	return S_OK;
}

void CBombard::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CBombard::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	_vector vOwnerLook =  m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	m_pLight_Projectile->Get_Component<CTransform>()->Translation(vOwnerLook * 2.f);
}

void CBombard::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CBombard::Pre_Setting(CGameObject* pObject)
{
	if (pObject == nullptr)
		return E_FAIL;

	m_pOwner = pObject;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	Reset_EffectParts();

	m_fAccTime = 0.f;
	__super::m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;



	CPartObject* pShootPt = Get_PartObject<CEditEffect>("Bombard_Shoot_Pt");

	CPartObject* pCircle0 = Get_PartObject<CEditEffect>("Bombard_Circle0");

	pShootPt->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pLight_Projectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	pCircle0->Set_Visible(true);
	pShootPt->Set_Visible(true);

	m_pLight_Projectile->Set_Visible(true);


	m_bVisible = true;


	return S_OK;
}

HRESULT CBombard::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBombard::Ready_Child()
{
	CDummy_PhysXEffectHitBox::PHYSXDUMMY_DESC Desc{};

	m_pTransformCom->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	XMStoreFloat3(&Desc.vPos, m_pOwner->Get_WorldPostion() + XMVectorSet(0.f, 0.f, 1.f, 0.f));


	_vector vOwnerLook = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	_float3 vDir = {};

	XMStoreFloat3(&Desc.vPos, m_pOwner->Get_WorldPostion() + XMVectorSet(0.f, 0.f, 1.f, 0.f));

	XMStoreFloat3(&vDir, vOwnerLook * 2.f);

	Desc.vRotRPY = { 0.f, 0.f, 0.f };
	Desc.iSubKind = 70;
	Desc.vDeltaPos = vDir;
	Desc.vLifeTime = { 0.f, 2.f };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXEffectHitBox>(g_iStaticLevel, CURRENT_LEVEL, LAYER_HITBOX, &Desc, this , &m_pPhysHitBox))) {
		assert(false);
		return E_FAIL;
	}

	SAFE_ADDREF(m_pPhysHitBox);
	return S_OK;
}

CBombard* CBombard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBombard* pInstance = new CBombard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBombard");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CBombard::Clone(void* pArg, CGameObject* pOwner)
{
	CBombard* pInstance = new CBombard(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBombard");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBombard::OnCollision(CGameObject* pOther , void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO*	CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, CollisionDesc->vWorldPos);
	}


	m_pLight_Projectile->Set_Visible(false);
    Get_PartObject<CEditEffect>("Bombard_Shoot_Pt")->Set_Visible(false);

	_vector vOwnerLook = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	Get_PartObject<CEditEffect>("Bombard_Smoke")->Get_Component<CTransform>()->Translation(vOwnerLook);

	Get_PartObject<CEditEffect>("Bombard_Circle0")->Set_Visible(false);


	m_pPhysHitBox->Set_Dead();
	SAFE_RELEASE(m_pPhysHitBox);
}

void CBombard::Free()
{
	__super::Free();

	if (m_pPhysHitBox != nullptr)
		SAFE_RELEASE(m_pPhysHitBox);

	SAFE_RELEASE(m_pLight_Projectile);

}

void CBombard::Describe_Entity()
{

}

HRESULT CBombard::Bind_ShaderResources()
{
	return S_OK;
}


