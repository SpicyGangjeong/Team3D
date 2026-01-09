#include "pch.h"
#include "Stupefy.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CStupefy::CStupefy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CStupefy::CStupefy(const CStupefy& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CStupefy::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Stupefy")))
		return E_FAIL;

	return S_OK;

}

HRESULT CStupefy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
	
	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::STUPEFY);

	m_wstrEffectName = L"Stupefy";

	m_pProjectile0 = Get_PartObject<CEffectParts>("Stupefy_PJ0");
	SAFE_ADDREF(m_pProjectile0);

	m_pProjectile1 = Get_PartObject<CEffectParts>("Stupefy_PJ1");
	SAFE_ADDREF(m_pProjectile1);

	m_pStupefy_PJ_PT = Get_PartObject<CEffectParts>("Stupefy_PJ_PT");
	SAFE_ADDREF(m_pStupefy_PJ_PT);

	m_fDuration = 4.5f;

	return S_OK;
}

void CStupefy::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pStupefy_PJ_PT->Get_WorldPostion());
}

void CStupefy::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	m_pStupefy_PJ_PT->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vCameraLook) * m_fLinearSpeed);


}

void CStupefy::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);

	XMStoreFloat4(&m_vEndPos, m_pStupefy_PJ_PT->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);


	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);
		ON_COLLISION_INFO CollisionInfo = SweepTarget(vStartPos, vEndPos, 0.002f);

		OnCollision(this, &CollisionInfo);
	}

}

HRESULT CStupefy::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return E_FAIL;




	CEffectParts* pStupefy_PJ0 = Get_PartObject<CEffectParts>("Stupefy_PJ0");
	CEffectParts* pStupefy_PJ1 = Get_PartObject<CEffectParts>("Stupefy_PJ1");
	CEffectParts* pWand_Distortion = Get_PartObject<CEffectParts>("Wand_Distortion");

	CEffectParts* pStupefy_Light_Long = Get_PartObject<CEffectParts>("Stupefy_Light_Long");
	CEffectParts* pBodyFlare = Get_PartObject<CEffectParts>("BodyFlare");

	CEffectParts* pStupefy_Smoke = Get_PartObject<CEffectParts>("Stupefy_Smoke");

	pStupefy_PJ0->Set_Visible(true);
	pStupefy_PJ1->Set_Visible(true);
	pWand_Distortion->Set_Visible(true);
	pStupefy_Light_Long->Set_Visible(true);
	pBodyFlare->Set_Visible(true);
	pStupefy_Smoke->Set_Visible(true);
	m_pStupefy_PJ_PT->Set_Visible(true);

	_vector vDirection = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	/*완드 포지션*/

	_vector vWandPos = pWand->Get_WorldPostion();

	pStupefy_PJ0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vWandPos);
	pStupefy_PJ1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vWandPos);

	m_pStupefy_PJ_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vWandPos);

	pStupefy_Light_Long->Get_Component<CTransform>()->Set_State(STATE::POSITION, vWandPos);
	pBodyFlare->Get_Component<CTransform>()->Set_State(STATE::POSITION, vWandPos);
	//
	/*몸통 포지션*/

	_vector vPlayerPos = m_pOwner->Get_Component<CCharacter_Controller>()->Get_Position() + vDirection * -0.1f;

	pWand_Distortion->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPlayerPos);
	/*발 포지션*/

	_vector vPlayerFootPos = m_pOwner->Get_Component<CCharacter_Controller>()->Get_FootPosition();

	pStupefy_Smoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPlayerFootPos + XMVectorSet(0.f ,1.f ,0.f ,0.f));




	XMStoreFloat3(&m_vCameraLook, vDirection);

	_vector vStartPos = vWandPos;
	XMStoreFloat4(&m_vStartPos, vStartPos);

	{ /* 대상 위치 지정 */

		m_pInfoInstance->Get_LockOnInfo(m_Info);
		if (nullptr != m_Info.pUnit) {

			XMStoreFloat4(&m_vTargetPos,Get_UnitPos(m_Info));

			XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));
		}
		else {
			// 타겟이 없다면 현재위치 -> 카메라 룩벡터 * duration간 예상 이동거리 를 대상으로 지정
			XMStoreFloat4(&m_vTargetPos, vStartPos + vDirection * m_fLinearSpeed * 0.5f);
		}
	}


	/* PJ 월드 구성하기  */

	_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vTargetPos) - vPlayerPos));

	CTransform* pPJ0_Transform = pStupefy_PJ0->Get_Component<CTransform>();
	CTransform* pPJ1_Transform = pStupefy_PJ1->Get_Component<CTransform>();

	_vector vUp = XMVector3Normalize(vDirection) * fLength;
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f) , vUp));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vUp, vRight));
	_vector vPos = vPlayerPos;

	_matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };

	pPJ0_Transform->Set_WorldMatrix(PJ_WorldMat);
	pPJ1_Transform->Set_WorldMatrix(PJ_WorldMat);

	return S_OK;
}

HRESULT CStupefy::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStupefy::Ready_Child()
{
	return S_OK;
}

CStupefy* CStupefy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStupefy* pInstance = new CStupefy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStupefy");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CStupefy::Clone(void* pArg, CGameObject* pOwner)
{
	CStupefy* pInstance = new CStupefy(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStupefy");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CStupefy::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	CEffectParts* pStupefy_Hit_PT = Get_PartObject<CEffectParts>("Stupefy_Hit_PT");
	CEffectParts* pStupefy_Circle0 = Get_PartObject<CEffectParts>("Stupefy_Circle0");
	CEffectParts* pHitFlare = Get_PartObject<CEffectParts>("HitFlare");
	

	pStupefy_Hit_PT->Set_Visible(true);
	pStupefy_Hit_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pStupefy_Circle0->Set_Visible(true);
	pStupefy_Circle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pHitFlare->Set_Visible(true);
	pHitFlare->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);


	
	m_pProjectile0->Set_Visible(false);
	m_pProjectile1->Set_Visible(false);

	/*맞는순간 위로 올리기*/
	m_pStupefy_PJ_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, 500.f, 0.f, 1.f)); 


}

void CStupefy::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pProjectile0);
	SAFE_RELEASE(m_pProjectile1);
	SAFE_RELEASE(m_pStupefy_PJ_PT);
}

#ifdef _DEBUG
void CStupefy::Describe_Entity()
{

}
#endif

HRESULT CStupefy::Bind_ShaderResources()
{
	return S_OK;
}


