#include "pch.h"
#include "Decendo.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "TrailObject.h"

#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"


CDecendo::CDecendo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }

{
}

CDecendo::CDecendo(const CDecendo& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
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

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::DESCENDO);

	m_wstrEffectName = L"Decendo";

	m_pProjectile_Blur = Get_PartObject<CEffectParts>("Decendo_Proj_Blur");
	m_pProjectile = Get_PartObject<CEffectParts>("Decendo_Proj");

	SAFE_ADDREF(m_pProjectile_Blur);
	SAFE_ADDREF(m_pProjectile);

	m_fDuration = 3.5f;

	return S_OK;
}

void CDecendo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pProjectile->Get_WorldPostion());
}

void CDecendo::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	_vector vDirection = XMLoadFloat3(&m_vCameraLook);

	// 선속도
	m_pProjectile_Blur->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);
	m_pProjectile->Get_Component<CTransform>()->Translation(vDirection * m_fLinearSpeed * fTimeDelta);

	//Up벡터 방향으로 파이만큼 이동

	if (m_fRotateAccTime > XM_2PI)
		return;

	m_fRotateAccTime += fTimeDelta * m_fAngularSpeed;

	m_pProjectile_Blur->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateUp) * sinf(m_fRotateAccTime) * 0.1f);
	m_pProjectile->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vRotateUp) * sinf(m_fRotateAccTime) * 0.1f);


}

void CDecendo::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	Get_PartObject<CTrailObject>()->Trail_Update(m_pProjectile->Get_Component<CTransform>()->Get_XMWorldMatrix(), fTimeDelta);

	XMStoreFloat4(&m_vEndPos, m_pProjectile->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);

	if (true == m_pGameInstance->SphereCast(0.125, XMLoadFloat4(&m_vStartPos), XMVector3Normalize(vDir), XMVectorGetX(XMVector3Length(vDir))
		, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eDYNAMIC | PSX::PxQueryFlag::eSTATIC, m_Hitbuffer))
	{
		OnCollision();
	}

	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);
		SweepTarget(vStartPos, vEndPos, 0.2f);
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CDecendo::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;

	CPartObject* pCircle0 = Get_PartObject<CEffectParts>();
	CPartObject* pWandLight = Get_PartObject<CEffectParts>("Decendo_Wand_Light");

	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	pCircle0->Set_Visible(true);
	pWandLight->Set_Visible(true);

	/* 초기 위치 저장  */
	_vector vStartPos = m_pOwner->Get_WorldPostion();
	XMStoreFloat4(&m_vStartPos, vStartPos);

	/* 초기 객체 위치 초기화 */

	m_pProjectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	m_pProjectile_Blur->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);

	/* 초기 객체 비지블 */
	m_pProjectile->Set_Visible(true);
	m_pProjectile_Blur->Set_Visible(true);

	/*트레일 초기화 */
	Get_PartObject<CTrailObject>()->Set_Visible(true);
	Get_PartObject<CTrailObject>()->Get_Component<CTrail>()->Reset_Trail();


	_vector vDirection = m_pOwner->Get_Owner()->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	XMStoreFloat3(&m_vCameraLook, vDirection);

	{ /* 대상 위치 지정 */

		m_pInfoInstance->Get_LockOnInfo(m_Info);
		if (nullptr != m_Info.pUnit) {

			XMStoreFloat4(&m_vTargetPos, m_Info.pUnit->Get_LockOnPos());

			XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));
		}
		else {
			// 타겟이 없다면 현재위치 -> 카메라 룩벡터 * duration간 예상 이동거리 를 대상으로 지정
			XMStoreFloat4(&m_vTargetPos, vStartPos + vDirection * m_fLinearSpeed * 0.5f);
		}
	}


	/* 대상 거리 계산 */

	m_fRotateAccTime = 0.f;

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);


	_vector vQuaternion = XMQuaternionRotationAxis(XMLoadFloat3(&m_vCameraLook), XMConvertToRadians(m_pGameInstance->Random_Float(0, 360.f)));

	XMStoreFloat3(&m_vRotateUp, XMVector3Normalize(XMVector3Rotate(vUp, vQuaternion)));


	/* 내속도를 알고 잇음*/

	/* 거리 / 속도 = 시간 */

	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));

	m_fTimeRate = fDistance / m_fLinearSpeed;

	//내가 적에게까지 갈때까지 걸리는 시간을 암

	//그러므로 내가 0 ~ 파이 까지 러프하는 과정이 그 시간동안 이루어져야 함

	//그 속도가 파이 / 시간 이므로 이걸 프레임마다 누적함 

	m_fAngularSpeed = XM_2PI / m_fTimeRate; // 프레임마다 누적할 시간 

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

void CDecendo::OnCollision(CGameObject* pOther, void* pDesc)
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



	m_pProjectile_Blur->Set_Visible(false);
	m_pProjectile->Set_Visible(false);


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return;

	CPartObject* pCircle0 = Get_PartObject<CEffectParts>();
	CPartObject* pWandLight = Get_PartObject<CEffectParts>("Decendo_Wand_Light");


	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	pCircle0->Set_Visible(false);
	pWandLight->Set_Visible(false);

	_vector vPlayerPos = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::POSITION);



	Get_PartObject<CEffectParts>("Decendo_Down")->Get_Component<CTransform>()->LookAt(vPlayerPos);
	Get_PartObject<CEffectParts>("Decendo_Smoke")->Get_Component<CTransform>()->LookAt(vPlayerPos);



	Get_PartObject<CTrailObject>()->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	Get_PartObject<CTrailObject>()->Set_Visible(false);

}

void CDecendo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pProjectile_Blur);
	SAFE_RELEASE(m_pProjectile);

}
#ifdef _DEBUG
void CDecendo::Describe_Entity()
{
	GUI::Begin("DECENDO");

	GUI::InputFloat("Speed", &m_fLinearSpeed);

	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));

	GUI::InputFloat("DISTANCE", &fDistance);
	GUI::InputFloat("AngularSpeed", &m_fAngularSpeed);
	GUI::InputFloat("TimeRate", &m_fTimeRate);


	GUI::InputFloat3("TargetPos", (_float*)&m_vTargetPos);

	GUI::End();

}
#endif
HRESULT CDecendo::Bind_ShaderResources()
{
	return S_OK;
}


