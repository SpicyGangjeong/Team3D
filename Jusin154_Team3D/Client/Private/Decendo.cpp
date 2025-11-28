#include "pch.h"
#include "Decendo.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "PhysXEffectHitBox.h"
#include "TrailObject.h"


CDecendo::CDecendo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CDecendo::CDecendo(const CDecendo& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CDecendo::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CDecendo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Decendo")))
		return E_FAIL;


	m_wstrEffectName = L"Decendo";

	m_pProjectile_Blur = Get_PartObject<CEffectParts>("Decendo_Proj_Blur");
	m_pProjectile = Get_PartObject<CEffectParts>("Decendo_Proj");

	SAFE_ADDREF(m_pProjectile_Blur);
	SAFE_ADDREF(m_pProjectile);

	m_fDuration = 2.5f;

	return S_OK;
}

void CDecendo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CDecendo::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	m_pProjectile_Blur->Get_Component<CTransform>()->Translation(m_vOwnerLook * 0.5f);
	m_pProjectile->Get_Component<CTransform>()->Translation(m_vOwnerLook * 0.5f);



	if (m_fAccTime > XM_2PI)
		return;

	m_fAccTime += fTimeDelta * 7.5f;

	m_pProjectile_Blur->Get_Component<CTransform>()->Translation(m_vRotateUp * 0.2f * sinf(m_fAccTime));
	m_pProjectile->Get_Component<CTransform>()->Translation(m_vRotateUp * 0.2f * sinf(m_fAccTime));
}

void CDecendo::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	Get_PartObject<CTrailObject>()->Trail_Update(m_pProjectile->Get_Component<CTransform>()->Get_XMWorldMatrix(), fTimeDelta);

	__super::Late_Update(fTimeDelta);
}

HRESULT CDecendo::Pre_Setting(CGameObject* pObject)
{
	if (pObject == nullptr)
		return E_FAIL;

	/* 부모 할당 */
	m_pOwner = pObject;

	/* 피직스 생성*/
	if (FAILED(Ready_Child()))
		return E_FAIL;

	/* 초기 셋팅 초기화 */
	Reset_EffectParts();
	m_fAccTime = 0.f;
	__super::m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;


	CPartObject* pCircle0 = m_PartObjects.begin()->second;

	/* 초기 객체 위치 초기화 */
	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pProjectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pProjectile_Blur->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	/* 초기 객체 비지블 */
	pCircle0->Set_Visible(true);
	m_pProjectile->Set_Visible(true);
	m_pProjectile_Blur->Set_Visible(true);
	
	/*트레일 초기화 */
	Get_PartObject<CTrailObject>()->Set_Visible(true);
	Get_PartObject<CTrailObject>()->Get_Component<CTrail>()->Reset_Trail();


	//나아가는 벡터와 한점을 가져와 수직인 평면상에 하나의 점으로  DIR 을 만듬
	m_vOwnerLook = XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);


	_vector vQuaternion = XMQuaternionRotationAxis(m_vOwnerLook, m_pGameInstance->Random_Float(0.f, XM_PIDIV2));

	m_vRotateUp = XMVector3Rotate(vUp, vQuaternion);

	m_vRotateUp = XMVector3Normalize(m_vRotateUp);

	m_bVisible = true;

	return S_OK;
}

HRESULT CDecendo::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDecendo::Ready_Child()
{
	CPhysXEffectHitBox::PHYSXDUMMY_DESC Desc{};

	m_pTransformCom->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	XMStoreFloat3(&Desc.vPos, m_pOwner->Get_WorldPostion() + XMVectorSet(0.f, 0.f, 1.f, 0.f));

	_vector vOwnerLook = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	_float3 vDir = {};

	XMStoreFloat3(&Desc.vPos, m_pOwner->Get_WorldPostion() + XMVectorSet(0.f, 0.f, 1.f, 0.f));

	XMStoreFloat3(&vDir , vOwnerLook * 0.5f);

	Desc.vRotRPY = { 0.f, 0.f, 0.f };
	Desc.iSubKind = 70;
	Desc.vDeltaPos = vDir ;
	Desc.vLifeTime = { 0.f, 1.f };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPhysXEffectHitBox>(g_iStaticLevel, CURRENT_LEVEL, LAYER_HITBOX, &Desc, this , &m_pPhysHitBox))) {
		assert(false);
		return E_FAIL;
	}

	return S_OK;
}

CDecendo* CDecendo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecendo* pInstance = new CDecendo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDecendo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CDecendo::Clone(void* pArg, CGameObject* pOwner)
{
	CDecendo* pInstance = new CDecendo(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDecendo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDecendo::OnCollision(CGameObject* pOther , void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO*	CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, CollisionDesc->vWorldPos);
	}



	m_pProjectile_Blur->Set_Visible(false);
	m_pProjectile->Set_Visible(false);



	_vector vPlayerPos = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::POSITION);

	Get_PartObject<CEffectParts>("Decendo_Down")->Get_Component<CTransform>()->LookAt(vPlayerPos);
	Get_PartObject<CEffectParts>("Decendo_Smoke")->Get_Component<CTransform>()->LookAt(vPlayerPos);

	m_PartObjects.begin()->second->Set_Visible(false);

	Get_PartObject<CTrailObject>()->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	Get_PartObject<CTrailObject>()->Set_Visible(false);

	m_pPhysHitBox->Set_Dead();
}

void CDecendo::Free()
{
	__super::Free();

	//if(m_pPhysHitBox != nullptr)
	//	if (m_pPhysHitBox->Get_Depth() == false)
	//		SAFE_RELEASE(m_pPhysHitBox);

	SAFE_RELEASE(m_pProjectile_Blur);
	SAFE_RELEASE(m_pProjectile);

}
#ifdef _DEBUG

void CDecendo::Describe_Entity()
{
	GUI::Begin("VALUE");

	_float4 vValue = {};

	if (GUI::InputFloat("TurnValue", (_float*)&m_fTurnValue))
	{
	}

	GUI::End();
}
#endif // _DEBUG


HRESULT CDecendo::Bind_ShaderResources()
{
	return S_OK;
}


