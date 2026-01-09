#include "pch.h"
#include "Avadakedavra.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CAvadakedavra::CAvadakedavra(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CAvadakedavra::CAvadakedavra(const CAvadakedavra& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CAvadakedavra::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Avadakedavra")))
		return E_FAIL;

	return S_OK;

}

HRESULT CAvadakedavra::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
	
	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::AVADAKEDAVRA);

	m_wstrEffectName = L"Avadakedavra";


	m_fDuration = 4.5f;

	return S_OK;
}

void CAvadakedavra::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CAvadakedavra::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

}

void CAvadakedavra::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);

	if (m_Info.pUnit) {
		XMStoreFloat4(&m_vEndPos, m_Info.pUnit->Get_LockOnPos());
	}

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);


	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);
		ON_COLLISION_INFO CollisionInfo = SweepTarget(vStartPos, vEndPos, 0.02f);

		OnCollision(this, &CollisionInfo);
	}
}

HRESULT CAvadakedavra::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return E_FAIL;


	_vector vDirection = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);

	/*완드 포지션*/

	_vector vWandPos = pWand->Get_WorldPostion();

	XMStoreFloat3(&m_vCameraLook, vDirection);

	//_vector vStartPos = vWandPos;
	XMStoreFloat4(&m_vStartPos, vWandPos);

	{ /* 대상 위치 지정 */

		m_pInfoInstance->Get_LockOnInfo(m_Info);

		if (nullptr != m_Info.pUnit) {

			XMStoreFloat4(&m_vTargetPos, Get_UnitPos(m_Info));

			XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));
		}
		else {
			// 타겟이 없다면 현재위치 -> 카메라 룩벡터 * duration간 예상 이동거리 를 대상으로 지정
			XMStoreFloat4(&m_vTargetPos, vWandPos + vDirection * m_fLinearSpeed * 2.f);
		}
	}


	CEffectParts* pCircle_0 = Get_PartObject<CEffectParts>("Circle_0");
	CEffectParts* pCircle_Blur = Get_PartObject<CEffectParts>("Circle_Blur");
	CEffectParts* pCircle_1 = Get_PartObject<CEffectParts>("Circle_1");
	CEffectParts* pCircle_Blur_Big = Get_PartObject<CEffectParts>("Circle_Blur_Big");

	CEffectParts* pHit_Light = Get_PartObject<CEffectParts>("Hit_Light");
	CEffectParts* pHit_PT_G = Get_PartObject<CEffectParts>("Hit_PT_G");
	CEffectParts* pHit_PT_G_Small = Get_PartObject<CEffectParts>("Hit_PT_G_Small");
	CEffectParts* pRay_G = Get_PartObject<CEffectParts>("Ray_G");
	CEffectParts* pSmoke_Black = Get_PartObject<CEffectParts>("Smoke_Black");

	CEffectParts* pWand_Light = Get_PartObject<CEffectParts>("Wand_Light");

	CEffectParts* pWand_Lightning_G = Get_PartObject<CEffectParts>("Wand_Lightning_G");

	pCircle_0->Get_Component<CTransform>()->Set_State(STATE::POSITION , XMLoadFloat4(&m_vTargetPos));
	pCircle_Blur->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pHit_Light->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pHit_PT_G->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pSmoke_Black->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));

	pCircle_1->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pCircle_Blur_Big->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pHit_PT_G_Small->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos) - 1.f * XMLoadFloat3(&m_vCameraLook));

	pWand_Light->Get_Component<CTransform>()->Set_State(STATE::POSITION, vWandPos);
	pWand_Lightning_G->Get_Component<CTransform>()->Set_State(STATE::POSITION, vWandPos);

	pCircle_0->Set_Visible(true);
	pCircle_Blur->Set_Visible(true);
	pHit_Light->Set_Visible(true);
	pHit_PT_G->Set_Visible(true);
	pSmoke_Black->Set_Visible(true);
	pRay_G->Set_Visible(true);
	pHit_PT_G_Small->Set_Visible(true);
	pWand_Lightning_G->Set_Visible(true);

	pCircle_1->Set_Visible(true);
	pCircle_Blur_Big->Set_Visible(true);

	pWand_Light->Set_Visible(true);

	/* RAY 월드 구성하기  */

	_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vTargetPos) - vWandPos)) * 0.35f;

	CTransform* pRay_Transform = pRay_G->Get_Component<CTransform>();

	_vector vRight = XMVector3Normalize(XMLoadFloat3(&m_vCameraLook)) * fLength * -1.f;
	_vector vUp = XMVector3Normalize( XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f) , vRight));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight , vUp));
	_vector vPos = vWandPos;

	_matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };

	pRay_Transform->Set_WorldMatrix(PJ_WorldMat);

	static_cast<CPlayer*>(m_pOwner)->Start_CameraShake(0.5f, 1.2f);

	return S_OK;
}

HRESULT CAvadakedavra::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAvadakedavra::Ready_Child()
{
	return S_OK;
}

CAvadakedavra* CAvadakedavra::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAvadakedavra* pInstance = new CAvadakedavra(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAvadakedavra");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CAvadakedavra::Clone(void* pArg, CGameObject* pOwner)
{
	CAvadakedavra* pInstance = new CAvadakedavra(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAvadakedavra");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CAvadakedavra::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);
}

void CAvadakedavra::Free()
{
	__super::Free();

	//SAFE_RELEASE(m_pProjectile0);
	//SAFE_RELEASE(m_pProjectile1);
	//SAFE_RELEASE(m_pStupefy_PJ_PT);
}

#ifdef _DEBUG
void CAvadakedavra::Describe_Entity()
{

}
#endif

HRESULT CAvadakedavra::Bind_ShaderResources()
{
	return S_OK;
}


