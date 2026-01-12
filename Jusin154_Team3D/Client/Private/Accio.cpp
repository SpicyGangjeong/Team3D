#include "pch.h"
#include "Accio.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "TrailObject.h"
#include "Wand.h"
#include "InfoInstance.h"


CAccio::CAccio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CAccio::CAccio(const CAccio& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CAccio::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Accio")))
		return E_FAIL;

	return S_OK;
}

HRESULT CAccio::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::ACCIO);


	m_wstrEffectName = L"Accio";


	m_pRope_Trail = Get_PartObject<CTrailObject>("Accio_Trail");
	m_pWandLight = Get_PartObject<CEffectParts>("Wand_Light");
	m_pRotate0 = Get_PartObject<CEffectParts>("Rotate0");
	m_pRotate1 = Get_PartObject<CEffectParts>("Rotate1");

	m_pAccio_PT = Get_PartObject<CEffectParts>("Accio_PT");
	m_pAccio_Distortion = Get_PartObject<CEffectParts>("Accio_Distortion");

	if (m_pRope_Trail == nullptr)
		return E_FAIL;

	SAFE_ADDREF(m_pRope_Trail);
	SAFE_ADDREF(m_pWandLight);
	SAFE_ADDREF(m_pRotate0);
	SAFE_ADDREF(m_pRotate1);
	SAFE_ADDREF(m_pAccio_PT);
	SAFE_ADDREF(m_pAccio_Distortion);

	XMStoreFloat4x4(&m_TrailWorld, XMMatrixIdentity());

	m_fDuration = 4.f;

	m_Events.emplace(2.f, [&]() {

		m_pRope_Trail->Get_Component<CTrail>()->Rope_Fix(true);

		if (m_isDissolve)
			m_pRope_Trail->SetDissolve(true);

		m_pRope_Trail->Get_TrailInfo()->fMass = 0.1f;

		});

	return S_OK;
}

void CAccio::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


	_matrix TrailPos = XMLoadFloat4x4(&m_TrailWorld);
	XMStoreFloat4(&m_vStartPos, TrailPos.r[3]);

}

void CAccio::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	/* 트레일 월드 이동*/
	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return;

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pPlayer->Get_WandPos().r[3]);

	_matrix TrailPos = XMLoadFloat4x4(&m_TrailWorld);

	TrailPos.r[3] += XMLoadFloat3(&m_vCameraLook) * m_fLinearSpeed;


	if (m_bHit == true && m_pEnemyCCT != nullptr)
	{
		/* 특정 대상과 충돌 했다면*/
		TrailPos.r[3] = m_pEnemyCCT->Get_Position();

		_vector vPos = m_pEnemyCCT->Get_FootPosition();

		m_pRotate0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
		m_pRotate1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
		m_pAccio_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pEnemyCCT->Get_Position());
		m_pAccio_Distortion->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}

	XMStoreFloat4x4(&m_TrailWorld, TrailPos);

	if (m_bHit == false)
	{
		m_fAccRotateTime += fTimeDelta * m_fTurnSpeed;
		_matrix rotMat = XMMatrixRotationAxis(XMLoadFloat3(&m_vCameraLook), m_fAccRotateTime);

		_float fRange = m_fRange - m_fAccRotateTime / m_fAttenuation;

		if (fRange > 0)
			TrailPos.r[3] += rotMat.r[1] * fRange;
	}

	if(m_isReverse == false)
		m_pRope_Trail->Rope_Trail_Update(TrailPos, pPlayer->Get_WandPos(), fTimeDelta);
	else
		m_pRope_Trail->Rope_Trail_Update(pPlayer->Get_WandPos(), TrailPos, fTimeDelta);


}

void CAccio::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	_matrix TrailPos = XMLoadFloat4x4(&m_TrailWorld);
	XMStoreFloat4(&m_vEndPos, TrailPos.r[3]);

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

HRESULT CAccio::Pre_Setting(CGameObject* pObject, void* pArg)
{

	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return E_FAIL;

	_vector WandPos = pPlayer->Get_WandPos().r[3];

	CEffectParts* pCircle = Get_PartObject<CEffectParts>("Accio_Circle");
	CEffectParts* pSpread_Circle = Get_PartObject<CEffectParts>("Spread_Circle");

	m_pRope_Trail->Set_Visible(true);
	pCircle->Set_Visible(true);
	
	m_pWandLight->Set_Visible(true);

	pSpread_Circle->Set_Visible(true);

	m_pRope_Trail->Get_Component<CTrail>()->Reset_Trail();

	pCircle->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);
	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);
	pSpread_Circle->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);

	_vector vDirection = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	XMStoreFloat3(&m_vCameraLook, vDirection);

	_vector vStartPos = WandPos;
	XMStoreFloat4(&m_vStartPos, vStartPos);

	_matrix WandWorld = XMMatrixIdentity();

	WandWorld.r[3] = pPlayer->Get_WandPos().r[3];

	XMStoreFloat4x4(&m_TrailWorld , WandWorld);

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

	m_fAccRotateTime = 0.f;

	m_pRope_Trail->Get_TrailInfo()->fMass = 0.05f;

	return S_OK;
}

HRESULT CAccio::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAccio::Ready_Child()
{

	return S_OK;
}

CAccio* CAccio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAccio* pInstance = new CAccio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAccio");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CAccio::Clone(void* pArg, CGameObject* pOwner)
{
	CAccio* pInstance = new CAccio(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAccio");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CAccio::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	dynamic_cast<CPlayer*>(m_pOwner)->Set_SpellHit(true);

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	m_pEnemyCCT = CollisionDesc.pObject->Get_Component<CCharacter_Controller>();


	CEffectParts* pHit = Get_PartObject<CEffectParts>("Accio_Hit");
	CEffectParts* pHit0 = Get_PartObject<CEffectParts>("Accio_Hit0");


	pHit->Set_Visible(true);
	pHit->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

	pHit0->Set_Visible(true);
	pHit0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

	m_pAccio_Distortion->Set_Visible(true);

	

	CEffectParts* pCircle = Get_PartObject<CEffectParts>("Accio_Circle");
	CEffectParts* pSpread_Circle = Get_PartObject<CEffectParts>("Spread_Circle");


	pCircle->Set_Visible(false);
	pSpread_Circle->Set_Visible(false);


	m_pRotate0->Set_Visible(true);
	m_pRotate1->Set_Visible(true);

	m_pAccio_PT->Set_Visible(true);
	m_pAccio_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	
	m_pRope_Trail->Get_Component<CTrail>()->Rope_Fix(true);

	if(m_isDissolve)
		m_pRope_Trail->SetDissolve(true);

	m_pRope_Trail->Get_TrailInfo()->fMass = 0.1f;

}

void CAccio::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRope_Trail);
	SAFE_RELEASE(m_pWandLight);
	SAFE_RELEASE(m_pRotate0);
	SAFE_RELEASE(m_pRotate1);
	SAFE_RELEASE(m_pAccio_PT);
	SAFE_RELEASE(m_pAccio_Distortion);
}
#ifdef _DEBUG

void CAccio::Describe_Entity()
{
	GUI::Begin("ACCIO");

	GUI::DragFloat("TurnSpeed", &m_fTurnSpeed);
	GUI::DragFloat("Speed", &m_fLinearSpeed);
	GUI::DragFloat("Range", &m_fRange);
	GUI::DragFloat("Attenuation", &m_fAttenuation);
	GUI::Checkbox("Dissolve", &m_isDissolve);
	GUI::Checkbox("Reverse", &m_isReverse);
	GUI::InputInt("LoopTime", &m_iLoopTime);
	
	m_pRope_Trail->Describe_Entity();
	m_pRope_Trail->Get_Component<CTrail>()->Describe_Entity();

	GUI::End();
}

#endif // _DEBUG

HRESULT CAccio::Bind_ShaderResources()
{
	return S_OK;
}


