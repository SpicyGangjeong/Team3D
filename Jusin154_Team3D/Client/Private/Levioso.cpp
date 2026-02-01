#include "pch.h"
#include "Levioso.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "TrailObject.h"
#include "Wand.h"
#include "InfoInstance.h"


CLevioso::CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CLevioso::CLevioso(const CLevioso& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CLevioso::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Levioso")))
		return E_FAIL;

	return S_OK;

}

HRESULT CLevioso::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::LEVIOSO);

	m_pLeviosoPJ = Get_PartObject<CEffectParts>("Levioso_PJ");

	SAFE_ADDREF(m_pLeviosoPJ);

	m_wstrEffectName = L"Levioso";


	m_fDuration = 6.f;



	return S_OK;
}

void CLevioso::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pLeviosoPJ->Get_WorldPostion());

}

void CLevioso::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	m_pLeviosoPJ->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vCameraLook) * m_fLinearSpeed);

}

void CLevioso::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	XMStoreFloat4(&m_vEndPos, m_pLeviosoPJ->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);

	if (false == m_bHit) {

		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);

		if (false == m_bHit) {
			_vector vStartPos = XMLoadFloat4(&m_vStartPos);
			_vector vEndPos = XMLoadFloat4(&m_vEndPos);
			if (false == XMVector3NearEqual(vEndPos, XMVectorZero(), XMVectorReplicate(FLT_EPSILON5)))
			{
				ON_COLLISION_INFO CollisionInfo = SweepTarget(vStartPos, vEndPos, 0.02f);
				OnCollision(this, &CollisionInfo);
			}
		}
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CLevioso::Pre_Setting(CGameObject* pObject, void* pArg)
{

	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return E_FAIL;

	m_pGameInstance->Sound_Play(SOUND::SD_KIND::SP_LEVIOSA_27, SD_CHANNEL_GROUP::EFFECT, false, 0.6f);

	_vector WandPos = pPlayer->Get_WandPos().r[3];

	CEffectParts* pWandSmoke = Get_PartObject<CEffectParts>("Wand_Smoke");
	CEffectParts* pWandLight = Get_PartObject<CEffectParts>("Wand_Light_P");
	CEffectParts* pSpread_Circle = Get_PartObject<CEffectParts>("Spread_Circle");


	m_pLeviosoPJ->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);

	pWandSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, pPlayer->Get_PartObject<CWand>()->Get_WorldPostion());
	pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pPlayer->Get_PartObject<CWand>()->Get_WorldPostion());
	pSpread_Circle->Get_Component<CTransform>()->Set_State(STATE::POSITION, pPlayer->Get_PartObject<CWand>()->Get_WorldPostion());

	m_pLeviosoPJ->Set_Visible(true);
	pWandSmoke->Set_Visible(true);
	pWandLight->Set_Visible(true);
	pSpread_Circle->Set_Visible(true);

	XMStoreFloat3(&m_vCameraLook , XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK)));

	_vector vDirection = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	XMStoreFloat3(&m_vCameraLook, vDirection);

	_vector vStartPos = WandPos;
	XMStoreFloat4(&m_vStartPos, vStartPos);

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
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	m_fAccTime = 0.f;
	m_fDuration = 3.5f; //적중하면 지속시간 3초

	_vector vHitPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	CCharacter_Controller* pHitCCT = CollisionDesc.pObject->Get_Component<CCharacter_Controller>();

	if (pHitCCT != nullptr)
	{
		m_pGameInstance->Sound_Play(SOUND::SD_KIND::SP_LEVIOSA_26, SD_CHANNEL_GROUP::EFFECT, false, 0.6f);

		CEffectParts* pLevioso_Spline = Get_PartObject<CEffectParts>("Levioso_Spline");
		CEffectParts* pLevioso_Hit0 = Get_PartObject<CEffectParts>("Levioso_Hit0");

		pLevioso_Hit0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);
		pLevioso_Spline->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);

		pLevioso_Hit0->Set_Visible(true);
		pLevioso_Spline->Set_Visible(true);

		/* 바람과 파티클은 풋포지션 */


		_vector vFootPos = pHitCCT->Get_FootPosition();

		CEffectParts* pLevioso_Tornado = Get_PartObject<CEffectParts>("Levioso_Tornado");
		CEffectParts* pLevioso_Rotate0 = Get_PartObject<CEffectParts>("Levioso_Rotate0");
		CEffectParts* pLevioso_Rotate1 = Get_PartObject<CEffectParts>("Levioso_Rotate1");
		CEffectParts* pLevioso_Bottom_PT = Get_PartObject<CEffectParts>("Levioso_Bottom_PT");

		pLevioso_Tornado->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pLevioso_Rotate0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pLevioso_Rotate1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pLevioso_Bottom_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);


		_float3 vScale = _float3(1.f, 1.f, 1.f);
		pLevioso_Tornado->Get_Component<CTransform>()->Set_Scale(vScale);
		pLevioso_Rotate0->Get_Component<CTransform>()->Set_Scale(vScale);
		pLevioso_Rotate1->Get_Component<CTransform>()->Set_Scale(vScale);
		pLevioso_Bottom_PT->Get_Component<CTransform>()->Set_Scale(vScale);

		switch (static_cast<PXOBJECT>(CollisionDesc.eCollisionType))
		{
		case PXOBJECT::GOBLIN_MAGICIAN:
		case PXOBJECT::GOBLIN_ASSASSIN:
		case PXOBJECT::GOBLIN_WARRIOR:
		{
			_float3 vScale = _float3(0.7f, 0.7f, 0.7f);
			pLevioso_Tornado->Get_Component<CTransform>()->Set_Scale(vScale);
			pLevioso_Rotate0->Get_Component<CTransform>()->Set_Scale(vScale);
			pLevioso_Rotate1->Get_Component<CTransform>()->Set_Scale(vScale);
			pLevioso_Bottom_PT->Get_Component<CTransform>()->Set_Scale(vScale);
			break;
		}

		default:
			break;
		}

		pLevioso_Tornado->Set_Visible(true);
		pLevioso_Rotate0->Set_Visible(true);
		pLevioso_Rotate1->Set_Visible(true);
		pLevioso_Bottom_PT->Set_Visible(true);
	}

	m_pLeviosoPJ->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -9999.f, 0.f, 1.f));

}

void CLevioso::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLeviosoPJ);
}
#ifdef _DEBUG

void CLevioso::Describe_Entity()
{
}

#endif // _DEBUG

HRESULT CLevioso::Bind_ShaderResources()
{
	return S_OK;
}


