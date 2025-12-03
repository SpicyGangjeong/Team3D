#include "pch.h"
#include "NomalJap.h"

#include "Unit.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "EditEffect.h"
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

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Jap"))) {
		return E_FAIL;
	}


	m_pProjectile_Side = Get_PartObject<CEditEffect>("JapProjSide");
	m_pProjectile = Get_PartObject<CEditEffect>("JapProjCircle");

	SAFE_ADDREF(m_pProjectile);
	SAFE_ADDREF(m_pProjectile_Side);

	m_fDuration = 1.7f;
	assert(m_fDuration > 0.f);
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
	if (m_fAccTime > m_fDuration) {
		SAFE_RELEASE(m_pTargetUnit);
	}

	_vector vDirection = XMLoadFloat3(&m_vDirection);

	//_vector vStartPos = Get_WorldPostion();
	//_float3 vfStartPos = {};
	//XMStoreFloat3(&vfStartPos, Get_WorldPostion());

	// 선속도
	m_pProjectile_Side->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);
	m_pProjectile->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);

	//_vector vEndPos = Get_WorldPostion();

	//_float3 vfEndPos = {};
	//XMStoreFloat3(&vfEndPos, Get_WorldPostion());

	//_float3 vDir = {};
	//XMStoreFloat3(&vDir, vEndPos - vStartPos);

	//m_pGameInstance->SphereCast(0.125f, vfStartPos, vDir, XMVectorGetX(XMVector3Length(vEndPos - vStartPos)),  )


	if (false == m_bTrailPulseEnded && m_fAccTime > m_fDuration * 0.3f) {
		m_bTrailPulseEnded = true;

		if (nullptr != m_pTargetUnit && false == m_pTargetUnit->isDead()) {
			_vector vCurrentPos = m_pProjectile->Get_WorldPostion();
			_vector vTargetPos = m_pTargetUnit->Get_WorldPostion();
			XMStoreFloat4(&m_vTargetPos, vTargetPos);
			XMStoreFloat3(&m_vDirection, XMVector4Normalize(vTargetPos - vCurrentPos));

			_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vCurrentPos));
			m_fLinearSpeed = fDistance / (m_fDuration - m_fAccTime) * 10 / 3;
			SAFE_RELEASE(m_pTargetUnit);
		}

		return;
	}
	if (true == m_bTrailPulseEnded) {

		return;
	}
	_vector vRotateUp = XMLoadFloat3(&m_vRotateUp);
	m_fAccTime += fTimeDelta;

	// 각속도
	m_pProjectile_Side->Get_Component<CTransform>()->Translation(vRotateUp * 0.6f * sinf(m_fAngularSpeed * m_fAccTime));
	m_pProjectile->Get_Component<CTransform>()->Translation(vRotateUp * 0.6f * sinf(m_fAngularSpeed * m_fAccTime));
}


void CNomalJap::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false) {
		return;
	}

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
	if (FAILED(Ready_Child())) {
		return E_FAIL;
	}

	/* 초기 셋팅 초기화 */
	Reset_EditEffect();
	m_fAccTime = 0.f;
	__super::m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;
	m_bTrailPulseEnded = false;


	/* 초기 객체 위치 초기화 */
	_vector vStartPos = m_pOwner->Get_WorldPostion();
	XMStoreFloat4(&m_vStartPos, vStartPos);
	m_pProjectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	m_pProjectile_Side->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);

	/* 초기 객체 비지블 */
	m_pProjectile->Set_Visible(true);
	m_pProjectile_Side->Set_Visible(true);

	/*트레일 초기화 */
	Get_PartObject<CTrailObject>()->Set_Visible(true);
	Get_PartObject<CTrailObject>()->Get_Component<CTrail>()->Reset_Trail();

	//나아가는 벡터와 한점을 가져와 수직인 평면상에 하나의 점으로  DIR 을 만듬
	pair<_float3, _float3> pairCameraLook = m_pInfoInstance->Get_CameraCoordinateSystem();
	_vector vDirection = XMVector3Normalize(XMLoadFloat3(&pairCameraLook.first));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat3(&m_vDirection, vDirection);

	//Get_PartObject<CEditEffect>("Circle_Particle_Red")->Set_Visible(true);
	//Get_PartObject<CEditEffect>("Circle_Particle_Red")->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());

	_vector vQuaternion = XMQuaternionRotationAxis(vDirection, m_pGameInstance->Random_Float(0.f, XM_PIDIV2));

	{ /* 대상 위치 지정 */
		SAFE_RELEASE(m_pTargetUnit);
		m_pTargetUnit = m_pInfoInstance->Get_LockOnUnit();
		if (nullptr != m_pTargetUnit) {
			SAFE_ADDREF(m_pTargetUnit); // 타겟이 있다면 타겟 위치로 지정
			XMStoreFloat4(&m_vTargetPos, m_pTargetUnit->Get_WorldPostion());
		}
		else {
			// 타겟이 없다면 현재위치 -> 카메라 룩벡터 * duration간 예상 이동거리 를 대상으로 지정
			XMStoreFloat4(&m_vTargetPos, vStartPos + vDirection * m_pTransformCom->Get_Speed() * m_fDuration);
		}
	}
	{ /* 대상 거리 계산 */
		_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));
		m_fLinearSpeed = fDistance / m_fDuration;

		// 전체 duration 의 0.3 지점에서 2π 회전하도록 각속도 설정
		_float fTargetRatio = 0.3f;
		m_fAngularSpeed = m_pGameInstance->Random_Float(2.f, 2.8f) * XM_2PI / (m_fDuration * fTargetRatio);
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

	Get_PartObject<CEditEffect>("JapPT0")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());

	SAFE_RELEASE(m_pTargetUnit);
	m_pProjectile_Side->Set_Visible(false);
	m_pProjectile->Set_Visible(false);
	Get_PartObject<CEditEffect>("Circle_Particle_Red")->Set_Visible(false);

	Get_PartObject<CTrailObject>()->Set_Visible(false);
	Get_PartObject<CTrailObject>()->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

}

void CNomalJap::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTargetUnit);
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


