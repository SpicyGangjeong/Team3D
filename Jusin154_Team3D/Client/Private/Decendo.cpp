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
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Decendo")))
		return E_FAIL;

	return S_OK;

}

HRESULT CDecendo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Create_Effect()))
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

	Get_PartObject<CTrailObject>()->Oneside_Rope_Trail_Update(m_pProjectile->Get_Component<CTransform>()->Get_XMWorldMatrix(), fTimeDelta);

	XMStoreFloat4(&m_vEndPos, m_pProjectile->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);

	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);
		ON_COLLISION_INFO CollisionInfo = SweepTarget(vStartPos, vEndPos, 0.002f);

		OnCollision(this, &CollisionInfo);
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
	CPartObject* pDecendo_Wand_Line = Get_PartObject<CEffectParts>("Decendo_Wand_Line");

	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pDecendo_Wand_Line->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	pCircle0->Set_Visible(true);
	pWandLight->Set_Visible(true);
	pDecendo_Wand_Line->Set_Visible(true);

	/* 초기 위치 저장  */
	_vector vStartPos = m_pOwner->Get_WorldPostion();
	XMStoreFloat4(&m_vStartPos, vStartPos);

	/* 초기 객체 위치 초기화 */

	m_pProjectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	m_pProjectile_Blur->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);

	/* 초기 객체 비지블 */
	m_pProjectile->Set_Visible(true);
	m_pProjectile_Blur->Set_Visible(true);
	//m_pProjectile_Fire->Set_Visible(true);

	/*트레일 초기화 */
	Get_PartObject<CTrailObject>()->Set_Visible(true);
	Get_PartObject<CTrailObject>()->Get_Component<CTrail>()->Reset_Trail();


	_vector vDirection = m_pOwner->Get_Owner()->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	XMStoreFloat3(&m_vCameraLook, vDirection);

	{ /* 대상 위치 지정 */
		m_pInfoInstance->Get_LockOnInfo(m_Info);

		if (nullptr != m_Info.pUnit || nullptr != m_Info.pEffect) {

			XMStoreFloat4(&m_vTargetPos, Get_LockOnPos(m_Info));

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


	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vHitPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	CCharacter_Controller* pCCT = CollisionDesc.pObject->Get_Component<CCharacter_Controller>();
	_vector vFootPos = {};
	_vector vHeadPos = {};
	_vector vPos = {};

	if (pCCT != nullptr)
	{
		vFootPos = pCCT->Get_FootPosition();
		vPos = pCCT->Get_Position();
		vHeadPos = pCCT->Get_HeadPosition();


		CPartObject* pDecendo_Rock_PT = Get_PartObject<CEffectParts>("Decendo_Rock_PT");
		CPartObject* pDecendo_Red_Blur = Get_PartObject<CEffectParts>("Decendo_Red_Blur");
		CPartObject* pDecendo_Down = Get_PartObject<CEffectParts>("Decendo_Down");
		CPartObject* pDecendo_Ring = Get_PartObject<CEffectParts>("Decendo_Ring");
		CPartObject* pDecendo_Smoke = Get_PartObject<CEffectParts>("Decendo_Smoke");

		pDecendo_Rock_PT->Set_Visible(true);
		pDecendo_Rock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);

		pDecendo_Red_Blur->Set_Visible(true);
		pDecendo_Red_Blur->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

		pDecendo_Down->Set_Visible(true);
		pDecendo_Down->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHeadPos);

		pDecendo_Ring->Set_Visible(true);
		pDecendo_Ring->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);

		pDecendo_Smoke->Set_Visible(true);
		pDecendo_Smoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);


	}


	m_pProjectile_Blur->Set_Visible(false);
	m_pProjectile->Set_Visible(false);

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


