#include "pch.h"
#include "NomalJap.h"

#include "Unit.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "EffectParts.h"
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

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::JAP);


	m_wstrEffectName = L"Nomal_Jap";

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Jap"))) {
		return E_FAIL;
	}


	m_pProjectile_Side = Get_PartObject<CEffectParts>("JapProjSide");
	m_pProjectile = Get_PartObject<CEffectParts>("JapProjCircle");

	SAFE_ADDREF(m_pProjectile);
	SAFE_ADDREF(m_pProjectile_Side);

	m_fDuration = 1.7f;
	assert(m_fDuration > 0.f);
	return S_OK;
}

void CNomalJap::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


	XMStoreFloat4(&m_vStartPos, m_pProjectile->Get_WorldPostion());

}

void CNomalJap::Update(_float fTimeDelta)
{

	if (m_bVisible == false){
		return;
	}

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	_vector vDirection = XMLoadFloat3(&m_vCameraLook);


	// 선속도
	m_pProjectile_Side->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);
	m_pProjectile->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);

	//Up벡터 방향으로 파이만큼 이동

	if (m_fAccRotateTime > XM_2PI)
		return;

	m_fAccRotateTime += fTimeDelta * m_fAngularSpeed;

	m_pProjectile_Side->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateUp) * sinf(m_fAccRotateTime) * m_fRotateAmount);
	m_pProjectile->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateUp) * sinf(m_fAccRotateTime) * m_fRotateAmount);

	// 내 라이트 방향으로 좌우무빙 하도록 해보자 


	if (m_fAccZigZagTime > XM_2PI)
		m_fAccZigZagTime = 0.f;

	m_fAccZigZagTime += fTimeDelta * m_fZigZagSpeed;

	m_pProjectile_Side->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateRight) * sinf(m_fAccZigZagTime) * m_fZigZagAmount);
	m_pProjectile->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateRight) * sinf(m_fAccZigZagTime) * m_fZigZagAmount);

	XMStoreFloat4(&m_vEndPos, m_pProjectile->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);

	if (true == m_pGameInstance->SphereCast(0.25, XMLoadFloat4(&m_vStartPos), XMVector3Normalize(vDir), XMVectorGetX(XMVector3Length(vDir))
		, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eDYNAMIC | PSX::PxQueryFlag::eSTATIC, m_Hitbuffer))
	{
		OnCollision(this);
	}
}


void CNomalJap::Late_Update(_float fTimeDelta)
{
	if (false == m_bVisible) {
		return;
	}
	if (false == m_bHit) {
		SweepTarget();
	}
	Get_PartObject<CTrailObject>()->Trail_Update(m_pProjectile->Get_Component<CTransform>()->Get_XMWorldMatrix(), fTimeDelta);

	__super::Late_Update(fTimeDelta);
}

void CNomalJap::SweepTarget()
{
	_vector vStartPos = m_pProjectile->Get_WorldPostion();
	_vector vEndPos = XMLoadFloat4(&m_vTargetPos);
	_vector vDir = { vEndPos - vStartPos };

	_float fDistance = XMVectorGetX(XMVector3Length(vEndPos - vStartPos));

	PSX::PxSweepBuffer pxBuffer = {};

	_bool bHit = m_pGameInstance->SphereCast(0.2f, vStartPos, vDir, fDistance, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eDYNAMIC, pxBuffer);

	if (bHit) {
		const PSX::PxSweepHit& hit = pxBuffer.block;
		PSX::PxRigidActor* pActor = hit.actor;
		PSX::PxShape* pShape = hit.shape;

		if (nullptr != pActor && nullptr != pActor->userData)
		{
			PhsXUserData* pUserData = static_cast<PhsXUserData*>(pActor->userData);

			switch (pUserData->eKind)
			{
			case PHYSX_KIND::CCTActor:
			{
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::MONSTER:
				case PXOBJECT::GOBLIN_WARRIOR:
				{
					pUserData->pOwner->OnCollision(this);
					m_bHit = true;
				}
				break;
				case PXOBJECT::TROLL:
				{
					pUserData->pOwner->OnCollision(this);
					m_bHit = true;
				}
				break;
				}
			}
			}
		}
	}
}

HRESULT CNomalJap::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	m_fAccMoveTime = 0.f;
	m_fAccRotateTime = 0.f;

	/* 초기 위치 저장  */
	_vector vStartPos = m_pOwner->Get_WorldPostion();
	XMStoreFloat4(&m_vStartPos, vStartPos);

	/* 초기 객체 위치 초기화 */
	m_pProjectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	m_pProjectile_Side->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);

	/* 초기 객체 비지블 */
	m_pProjectile->Set_Visible(true);
	m_pProjectile_Side->Set_Visible(true);

	/*트레일 초기화 */
	Get_PartObject<CTrailObject>()->Set_Visible(true);
	Get_PartObject<CTrailObject>()->Get_Component<CTrail>()->Reset_Trail();

	_uint iIndex = *static_cast<_uint*>(pArg);


	m_fLinearSpeed = m_vJapData[iIndex].x;
	m_fAngle = XMConvertToRadians(m_vJapData[iIndex].y);
	m_fZigZagSpeed = m_vJapData[iIndex].z;
	m_fZigZagAngle = XMConvertToRadians(m_vJapData[iIndex].w);


	_vector vDirection = m_pOwner->Get_Owner()->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	XMStoreFloat3(&m_vCameraLook, vDirection);


	{ /* 대상 위치 지정 */
		m_bHit = false;
		CUnit* pTargetUnit = m_pInfoInstance->Get_LockOnUnit();
		if (nullptr != pTargetUnit) {
			XMStoreFloat4(&m_vTargetPos, pTargetUnit->Get_LockOnPos());

			if (XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos))) >= 35.f) { //거리가 일정 이상이라면 그냥 정면으로 발사
				XMStoreFloat4(&m_vTargetPos, vStartPos + vDirection * m_fLinearSpeed * 0.5f);
			}
			XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));
		}
		else {
			// 타겟이 없다면 현재위치 -> 카메라 룩벡터 * duration간 예상 이동거리 를 대상으로 지정
			XMStoreFloat4(&m_vTargetPos, vStartPos + vDirection * m_fLinearSpeed * 0.5f);
		}
	}


	/* 대상 거리 계산 */

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);


	_vector vQuaternion = XMQuaternionRotationAxis(XMLoadFloat3(&m_vCameraLook), m_fAngle);

	XMStoreFloat3(&m_vRotateUp, XMVector3Normalize(XMVector3Rotate(vUp, vQuaternion)));

	vQuaternion = XMQuaternionRotationAxis(XMLoadFloat3(&m_vRotateUp), m_fAngle);

	XMStoreFloat3(&m_vRotateRight, XMVector3Normalize(XMVector3Rotate(vRight, vQuaternion)));


	/* 내속도를 알고 잇음*/

	/* 거리 / 속도 = 시간 */

	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));

	m_fTimeRate = fDistance / m_fLinearSpeed;

	//내가 적에게까지 갈때까지 걸리는 시간을 암

	//그러므로 내가 0 ~ 파이 까지 러프하는 과정이 그 시간동안 이루어져야 함

	//그 속도가 파이 / 시간 이므로 이걸 프레임마다 누적함 

	m_fAngularSpeed = XM_2PI / m_fTimeRate; // 프레임마다 누적할 시간 

	m_fAccZigZagTime = 0.f;

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
	int iIndex = CollisionCheck();

	if (iIndex < 0){
		return;
	}

	if (m_isCollisionEnter == true){
		return;
	}

	m_isCollisionEnter = true;

	_vector vPos = XMVectorSet(m_Hitbuffer.touches[iIndex].position.x, m_Hitbuffer.touches[iIndex].position.y, m_Hitbuffer.touches[iIndex].position.z, 1.f);


	for (auto& pPair : m_PartObjects)
	{

		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}

	Get_PartObject<CEffectParts>("JapPT0")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());

	m_pProjectile_Side->Set_Visible(false);
	m_pProjectile->Set_Visible(false);
	Get_PartObject<CEffectParts>("Circle_Particle_Red")->Set_Visible(false);

	Get_PartObject<CTrailObject>()->Set_Visible(false);
	Get_PartObject<CTrailObject>()->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

}

void CNomalJap::Free()
{
	__super::Free();

	Safe_Release(m_pProjectile);
	Safe_Release(m_pProjectile_Side);

}
#ifdef _DEBUG

void CNomalJap::Describe_Entity()
{
	GUI::Begin("Jap");

	GUI::InputFloat("Speed", &m_fLinearSpeed);

	if (GUI::InputFloat("Angle", &m_fAngle))
		m_fAngle = XMConvertToRadians(m_fAngle);


	GUI::InputFloat("ZigTime", &m_fAccZigZagTime);

	GUI::InputFloat("ZigSpeed", &m_fZigZagSpeed);

	GUI::InputFloat("ZigAmount", &m_fZigZagAmount);

	if (GUI::InputFloat("ZigZagAngle", &m_fZigZagAngle))
		m_fZigZagAngle = XMConvertToRadians(m_fZigZagAngle);


	GUI::End();
}

#endif // _DEBUG

HRESULT CNomalJap::Bind_ShaderResources()
{
	return S_OK;
}


