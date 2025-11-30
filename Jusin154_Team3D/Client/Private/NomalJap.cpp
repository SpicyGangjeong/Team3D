#include "pch.h"
#include "NomalJap.h"

#include "Unit.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "EffectParts.h"
#include "PhysXEffectHitBox.h"
#include "TrailObject.h"


CNomalJap::CNomalJap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CNomalJap::CNomalJap(const CNomalJap& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
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


	m_wstrEffectName = L"Nomal_Jap";

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Jap"))){
		return E_FAIL;
	}


	m_pProjectile_Side = Get_PartObject<CEffectParts>("JapProjSide");
	m_pProjectile = Get_PartObject<CEffectParts>("JapProjCircle");

	SAFE_ADDREF(m_pProjectile);
	SAFE_ADDREF(m_pProjectile_Side);

	m_fDuration = 5.f;

	return S_OK;
}

void CNomalJap::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (nullptr != m_pPhysHitBox) {
		m_pPhysHitBox->Get_Component<CTransform>()->RewindMomentum();
	}

}

void CNomalJap::Update(_float fTimeDelta)
{

	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	_vector vCameraLook = XMLoadFloat3(&m_vCameraLook);

	m_pProjectile_Side->Get_Component<CTransform>()->Translation(vCameraLook * 1.2f);
	m_pProjectile->Get_Component<CTransform>()->Translation(vCameraLook * 1.2f);



	if (nullptr != m_pPhysHitBox) {
		m_pPhysHitBox->Get_Component<CTransform>()->AccumulateMomentum(vCameraLook * 1.2f);
	}

	if (m_fAccTime > XM_2PI){
		return;
	}

	_vector vRotateUp = XMLoadFloat3(&m_vRotateUp);
	m_fAccTime += fTimeDelta * 15.f; // ? 15배? 매직 넘버 뭐임 

	m_pProjectile_Side->Get_Component<CTransform>()->Translation(vRotateUp * 0.6f * sinf(m_fAccTime));
	m_pProjectile->Get_Component<CTransform>()->Translation(vRotateUp * 0.6f * sinf(m_fAccTime));

	if (nullptr != m_pPhysHitBox) {
		m_pPhysHitBox->Get_Component<CTransform>()->AccumulateMomentum(vRotateUp * 0.6f * sinf(m_fAccTime));
	}

}

void CNomalJap::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	Get_PartObject<CTrailObject>()->Trail_Update(m_pProjectile->Get_Component<CTransform>()->Get_XMWorldMatrix(), fTimeDelta);

	__super::Late_Update(fTimeDelta);
}

HRESULT CNomalJap::Pre_Setting(CGameObject* pObject)
{
	if (pObject == nullptr)
		return E_FAIL;

	/* 부모 할당 */
	m_pOwner = pObject;

	/* 피직스 생성*/
	if (FAILED(Ready_Child())){
		return E_FAIL;
	}

	/* 초기 셋팅 초기화 */
	Reset_EffectParts();
	m_fAccTime = 0.f;
	__super::m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;

	/* 초기 객체 위치 초기화 */
	m_pProjectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pProjectile_Side->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	/* 초기 객체 비지블 */
	m_pProjectile->Set_Visible(true);
	m_pProjectile_Side->Set_Visible(true);

	/*트레일 초기화 */
	Get_PartObject<CTrailObject>()->Set_Visible(true);

	Get_PartObject<CTrailObject>()->Get_Component<CTrail>()->Reset_Trail();

	//나아가는 벡터와 한점을 가져와 수직인 평면상에 하나의 점으로  DIR 을 만듬
	pair<_float3, _float3> pairCameraLook = m_pInfoInstance->Get_CameraCoordinateSystem();
	_vector vCameraLook = XMVector3Normalize(XMLoadFloat3(&pairCameraLook.first));
	XMStoreFloat3(&m_vCameraLook, vCameraLook);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vQuaternion = XMQuaternionRotationAxis(vCameraLook, m_pGameInstance->Random_Float(0.f, XM_PIDIV2));
	
	{ /* 대상 위치 지정 */
		m_pTargetUnit = m_pInfoInstance->Get_LockOnUnit();
		if (nullptr != m_pTargetUnit) {
			XMStoreFloat4(&m_vTargetPos, m_pTargetUnit->Get_WorldPostion()); // 타겟이 있다면 타겟 위치로 지정
		}
		else {
			// 타겟이 없다면 현재위치 -> 카메라 룩벡터 * 4초간 예상 이동거리 를 대상으로 지정
			XMStoreFloat4(&m_vTargetPos, m_pOwner->Get_WorldPostion() + vCameraLook * m_pTransformCom->Get_Speed() * 4.f);
		}
	}

	XMStoreFloat3(&m_vRotateUp, XMVector3Normalize(XMVector3Rotate(vUp, vQuaternion)));

	m_bVisible = true;

	return S_OK;
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
	CPhysXEffectHitBox::PHYSXDUMMY_DESC Desc{};

	m_pTransformCom->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	XMStoreFloat3(&Desc.vPos, m_pOwner->Get_WorldPostion() + XMVectorSet(0.f, 0.f, 1.f, 0.f));

	Desc.vRotRPY = { 0.f, 0.f, 0.f };
	Desc.iSubKind = ENUM_CLASS(PXOBJECT::SKILL_NORMALJAP);
	Desc.vDeltaPos = _float3(0.f, 0.f, 0.f);
	Desc.vLifeTime = { 0.f, 1.f };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPhysXEffectHitBox>(g_iStaticLevel, CURRENT_LEVEL, LAYER_HITBOX, &Desc, this, &m_pPhysHitBox))) {
		assert(false);
		return E_FAIL;
	}

	SAFE_ADDREF(m_pPhysHitBox);
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

void CNomalJap::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	for (auto& pPair : m_PartObjects)
	{

		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, CollisionDesc->vWorldPos);
	}

	Get_PartObject<CEffectParts>("JapPT0")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());

	m_pTargetUnit = nullptr;
	m_pProjectile_Side->Set_Visible(false);
	m_pProjectile->Set_Visible(false);

	Get_PartObject<CTrailObject>()->Set_Visible(false);
	Get_PartObject<CTrailObject>()->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	if (nullptr != m_pPhysHitBox) {
		m_pPhysHitBox->Set_Dead();
		SAFE_RELEASE(m_pPhysHitBox);
	}
}

void CNomalJap::Free()
{
	__super::Free();

	if (m_pPhysHitBox != nullptr)
		SAFE_RELEASE(m_pPhysHitBox);

	Safe_Release(m_pProjectile);
	Safe_Release(m_pProjectile_Side);

}
#ifdef _DEBUG

void CNomalJap::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CNomalJap::Bind_ShaderResources()
{
	return S_OK;
}


