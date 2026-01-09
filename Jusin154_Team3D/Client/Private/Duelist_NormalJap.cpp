#include "pch.h"
#include "Duelist_NormalJap.h"

#include "Unit.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "EffectParts.h"
#include "TrailObject.h"
#include "Human_Duelist.h"
#include "Player.h"


CDuelist_NormalJap::CDuelist_NormalJap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CDuelist_NormalJap::CDuelist_NormalJap(const CDuelist_NormalJap& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{

}

HRESULT CDuelist_NormalJap::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/NPCJap"))) {
		return E_FAIL;
	}

	return S_OK;

}

HRESULT CDuelist_NormalJap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::DUELIST_JAP);


	m_wstrEffectName = L"NPCJap";



	m_pProjectile_Side = Get_PartObject<CEffectParts>("JapProjSide");
	m_pProjectile = Get_PartObject<CEffectParts>("JapProjCircle");
	m_pJapFire = Get_PartObject<CEffectParts>("JapFire");

	SAFE_ADDREF(m_pProjectile);
	SAFE_ADDREF(m_pProjectile_Side);
	SAFE_ADDREF(m_pJapFire);

	m_fDuration = 1.7f;
	assert(m_fDuration > 0.f);
	return S_OK;
}

void CDuelist_NormalJap::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


	XMStoreFloat4(&m_vStartPos, m_pProjectile->Get_WorldPostion());

}

void CDuelist_NormalJap::Update(_float fTimeDelta)
{

	if (m_bVisible == false) {
		return;
	}

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	_vector vDirection = XMLoadFloat3(&m_vCameraLook);


	// 선속도
	m_pProjectile_Side->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);
	m_pProjectile->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);
	m_pJapFire->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);

	//Up벡터 방향으로 파이만큼 이동

	if (m_fAccRotateTime > XM_2PI)
		return;

	m_fAccRotateTime += fTimeDelta * m_fAngularSpeed;

	m_pProjectile_Side->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateUp) * sinf(m_fAccRotateTime) * m_fRotateAmount);
	m_pProjectile->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateUp) * sinf(m_fAccRotateTime) * m_fRotateAmount);
	m_pJapFire->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateUp) * sinf(m_fAccRotateTime) * m_fRotateAmount);

	// 내 라이트 방향으로 좌우무빙 하도록 해보자 


	if (m_fAccZigZagTime > XM_2PI)
		m_fAccZigZagTime = 0.f;

	m_fAccZigZagTime += fTimeDelta * m_fZigZagSpeed;

	m_pProjectile_Side->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateRight) * sinf(m_fAccZigZagTime) * m_fZigZagAmount);
	m_pProjectile->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateRight) * sinf(m_fAccZigZagTime) * m_fZigZagAmount);
	m_pJapFire->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateRight) * sinf(m_fAccZigZagTime) * m_fZigZagAmount);

	XMStoreFloat4(&m_vEndPos, m_pProjectile->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);
}


void CDuelist_NormalJap::Late_Update(_float fTimeDelta)
{
	if (false == m_bVisible) {
		return;
	}
	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = m_pProjectile->Get_WorldPostion();
		ON_COLLISION_INFO CollisionInfo = MonsterSweepTarget(vStartPos, vEndPos, 0.02f);

		OnCollision(this, &CollisionInfo);
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CDuelist_NormalJap::Pre_Setting(CGameObject* pObject, void* pArg)
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
	m_pJapFire->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);

	/* 초기 객체 비지블 */
	m_pProjectile->Set_Visible(true);
	m_pProjectile_Side->Set_Visible(true);
	m_pJapFire->Set_Visible(true);

	_uint iIndex = *static_cast<_uint*>(pArg);


	m_fLinearSpeed = m_vJapData[iIndex].x;
	m_fAngle = XMConvertToRadians(m_vJapData[iIndex].y);
	m_fZigZagSpeed = m_vJapData[iIndex].z;
	m_fZigZagAngle = XMConvertToRadians(m_vJapData[iIndex].w);


	_vector vDirection = m_pOwner->Get_Owner()->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	XMStoreFloat3(&m_vCameraLook, vDirection);


	{ /* 대상 위치 지정 */
		m_bHit = false;
		CUnit* pPlayer = m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first;

		if (pPlayer == nullptr)
			return E_FAIL;

		_vector vPlayerPos = pPlayer->Get_Component<CCharacter_Controller>()->Get_Position();

		if (nullptr != pPlayer) {
			XMStoreFloat4(&m_vTargetPos, vPlayerPos);

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

HRESULT CDuelist_NormalJap::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDuelist_NormalJap::Ready_Child()
{

	return S_OK;
}

CDuelist_NormalJap* CDuelist_NormalJap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDuelist_NormalJap* pInstance = new CDuelist_NormalJap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDuelist_NormalJap");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CDuelist_NormalJap::Clone(void* pArg, CGameObject* pOwner)
{
	CDuelist_NormalJap* pInstance = new CDuelist_NormalJap(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDuelist_NormalJap");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDuelist_NormalJap::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}
	
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther->Get_Owner());
	if (pPlayer != nullptr)
	{
		pPlayer->Start_CameraShake(0.2f, 1.f);
	}

	if (m_bHitShield) {
		_vector vDir = m_pTransformCom->Get_State(STATE::LOOK);
		vDir = XMVector3Normalize(vDir);

		_float fRandAngle = XMConvertToRadians(m_pGameInstance->Real_Random_Float(-180.f, 0.f));

		_matrix rotX = XMMatrixRotationX(fRandAngle);
		_vector vRandDir = XMVector3TransformNormal(vDir, rotX);

		XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(vRandDir));
	}
	else {
		Get_PartObject<CEffectParts>("JapPT0")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());

		m_pProjectile_Side->Set_Visible(false);
		m_pProjectile->Set_Visible(false);

		m_pJapFire->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -9999.f, 0.f, 1.f));
	}

	m_bHitShield = false;
}

void CDuelist_NormalJap::Free()
{
	__super::Free();

	Safe_Release(m_pProjectile);
	Safe_Release(m_pProjectile_Side);
	SAFE_RELEASE(m_pJapFire);

}
#ifdef _DEBUG

void CDuelist_NormalJap::Describe_Entity()
{
	GUI::Begin("Jap", 0, IMGUI_GLOBAL_BEGIN_FLAG);

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

HRESULT CDuelist_NormalJap::Bind_ShaderResources()
{
	return S_OK;
}


