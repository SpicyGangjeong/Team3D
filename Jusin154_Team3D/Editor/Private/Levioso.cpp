#include "pch.h"
#include "Levioso.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Player.h"
#include "TrailObject.h"
#include "Dummy_PhysXEffectHitBox.h"


CLevioso::CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CLevioso::CLevioso(const CLevioso& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CLevioso::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CLevioso::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Levioso")))
		return E_FAIL;

	m_pLeviosoPJ_0 = Get_PartObject<CEditEffect>("Levioso_PJ0");
	m_pLeviosoPJ_1 = Get_PartObject<CEditEffect>("Levioso_PJ1");
	m_pLeviosoTrail = Get_PartObject<CTrailObject>("Levioso_Trail");
	m_pTrail_PT_0 = Get_PartObject<CEditEffect>("Trail_PT0");

	SAFE_ADDREF(m_pLeviosoPJ_0);
	SAFE_ADDREF(m_pLeviosoPJ_1);
	SAFE_ADDREF(m_pLeviosoTrail);
	SAFE_ADDREF(m_pTrail_PT_0);

	m_wstrEffectName = L"Levioso";

	m_fTurnSpeed = 10.f;
	m_fRange = 1.f;

	m_fDuration = 1.5f;

	return S_OK;
}

void CLevioso::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (nullptr != m_pPhysHitBox) {
		m_pPhysHitBox->Get_Component<CTransform>()->RewindMomentum();
	}
}

void CLevioso::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);
	
	

	m_pLeviosoPJ_0->Get_Component<CTransform>()->Translation(m_vOwnerLook * 0.25f);
	m_pLeviosoPJ_1->Get_Component<CTransform>()->Translation(m_vOwnerLook * 0.25f);


	m_pTrail_PT_0->Get_Component<CTransform>()->Translation(m_vOwnerLook * 0.25f);



	if (nullptr != m_pPhysHitBox) {
		m_pPhysHitBox->Get_Component<CTransform>()->AccumulateMomentum(m_vOwnerLook * 0.25f);
	}


	_matrix WorldMat = m_pTrail_PT_0->Get_Component<CTransform>()->Get_XMWorldMatrix();

	m_pLeviosoTrail->Trail_Update(WorldMat, fTimeDelta);

}

void CLevioso::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return;

	_matrix WandWorld = pPlayer->Get_WandPos();

	//m_pLeviosoTrail->Get_Component<CTrail>()->Fixed_Trail(WandWorld);



	__super::Late_Update(fTimeDelta);


}

HRESULT CLevioso::Pre_Setting(CGameObject* pObject)
{
	if (pObject == nullptr)
		return E_FAIL;

	m_pOwner = pObject;


	Reset_EffectParts();

	__super::m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;

	m_fAccRotateTime = 0.f;



	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return E_FAIL;

	_vector WandPos = pPlayer->Get_WandPos().r[3];

	m_pLeviosoPJ_0->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());
	m_pLeviosoPJ_1->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());
	m_pTrail_PT_0->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());
	
	m_pLeviosoPJ_0->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);
	m_pLeviosoPJ_1->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);
	m_pTrail_PT_0->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);


	m_pLeviosoPJ_0 ->Set_Visible(true);
	m_pLeviosoPJ_1->Set_Visible(true);
	m_pTrail_PT_0->Set_Visible(true);

	m_pLeviosoTrail->Set_Visible(true);
	m_pLeviosoTrail->Get_Component<CTrail>()->Reset_Trail();

	m_vOwnerLook = XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));

	m_bVisible = true;

	return S_OK;
}

HRESULT CLevioso::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevioso::Ready_Child()
{
	CDummy_PhysXEffectHitBox::PHYSXDUMMY_DESC Desc{};

	m_pTransformCom->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	XMStoreFloat3(&Desc.vPos, m_pOwner->Get_WorldPostion() + XMVectorSet(0.f, 0.f, 1.f, 0.f));


	Desc.vRotRPY = { 0.f, 0.f, 0.f };
	Desc.iSubKind = 70;
	Desc.vDeltaPos = _float3(0.f, 0.f, 0.f);
	Desc.vLifeTime = { 0.f, 1.5f };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXEffectHitBox>(g_iStaticLevel, CURRENT_LEVEL, LAYER_HITBOX, &Desc, this, &m_pPhysHitBox))) {
		assert(false);
		return E_FAIL;
	}

	SAFE_ADDREF(m_pPhysHitBox);

	return S_OK;
}

CLevioso* CLevioso::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevioso* pInstance = new CLevioso(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLevioso");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CLevioso::Clone(void* pArg, CGameObject* pOwner)
{
	CLevioso* pInstance = new CLevioso(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLevioso");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLevioso::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);



	m_pLeviosoPJ_0->Set_Visible(false);
	m_pLeviosoPJ_1->Set_Visible(false);
	m_pTrail_PT_0->Set_Visible(false);
	m_pLeviosoTrail->Set_Visible(false);

	m_pPhysHitBox->Set_Dead();
	SAFE_RELEASE(m_pPhysHitBox);

}

void CLevioso::Free()
{
	__super::Free();

	if(m_pPhysHitBox != nullptr)
		SAFE_RELEASE(m_pPhysHitBox);

	SAFE_RELEASE(m_pLeviosoPJ_0);
	SAFE_RELEASE(m_pLeviosoPJ_1);
	SAFE_RELEASE(m_pLeviosoTrail);
	SAFE_RELEASE(m_pTrail_PT_0);
}
#ifdef _DEBUG

void CLevioso::Describe_Entity()
{
	GUI::Begin("LEVIOSO");

	GUI::DragFloat("TURN SPEED", &m_fTurnSpeed);
	GUI::DragFloat("RANGE", &m_fRange);
	GUI::End();

}

#endif // _DEBUG

HRESULT CLevioso::Bind_ShaderResources()
{
	return S_OK;
}


