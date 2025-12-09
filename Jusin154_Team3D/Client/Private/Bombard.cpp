#include "pch.h"
#include "Bombard.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CBombard::CBombard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CBombard::CBombard(const CBombard& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CBombard::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CBombard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::BOMBARDA);


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Bombard")))
		return E_FAIL;


	m_wstrEffectName = L"Bombard";

	m_pLight_Projectile = Get_PartObject<CEffectParts>("Bombard_Projectile");
	SAFE_ADDREF(m_pLight_Projectile);

	m_fDuration = 2.5f;

	return S_OK;
}

void CBombard::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pLight_Projectile->Get_WorldPostion());
}

void CBombard::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	CTransform* pPJTransform = m_pLight_Projectile->Get_Component<CTransform>();

	pPJTransform->Translation( XMLoadFloat3(&m_vCameraLook) * m_fLinearSpeed);


}

void CBombard::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);

	XMStoreFloat4(&m_vEndPos, m_pLight_Projectile->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);

	if (true == m_pGameInstance->SphereCast(0.125, XMLoadFloat4(&m_vStartPos), XMVector3Normalize(vDir), XMVectorGetX(XMVector3Length(vDir))
		, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eDYNAMIC | PSX::PxQueryFlag::eSTATIC, m_Hitbuffer))
	{
		OnCollision(this);
	}

	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);
		SweepTarget(vStartPos, vEndPos, 0.125);
	}

}

HRESULT CBombard::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return E_FAIL;


	CPartObject* pShootPt = Get_PartObject<CEffectParts>("Bombard_Shoot_Pt");
	CPartObject* pCircle0 = Get_PartObject<CEffectParts>("Bombard_Circle0");


	pShootPt->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	m_pLight_Projectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	pCircle0->Set_Visible(true);
	pShootPt->Set_Visible(true);

	m_pLight_Projectile->Set_Visible(true);

	_vector vDirection = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	XMStoreFloat3(&m_vCameraLook, vDirection);

	_vector vStartPos = pWand->Get_WorldPostion();
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

HRESULT CBombard::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBombard::Ready_Child()
{
	return S_OK;
}

CBombard* CBombard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBombard* pInstance = new CBombard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBombard");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CBombard::Clone(void* pArg, CGameObject* pOwner)
{
	CBombard* pInstance = new CBombard(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBombard");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBombard::OnCollision(CGameObject* pOther, void* pDesc)
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

	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return;

	CPartObject* pShootPt = Get_PartObject<CEffectParts>("Bombard_Shoot_Pt");
	CPartObject* pCircle0 = Get_PartObject<CEffectParts>("Bombard_Circle0");

	pShootPt->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());


	m_pLight_Projectile->Set_Visible(false);

	//Get_PartObject<CEffectParts>("Bombard_PT_0")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());
	//Get_PartObject<CEffectParts>("Bombard_PT_1")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());
	//Get_PartObject<CEffectParts>("Bombard_PT_2")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());

	_vector vOwnerLook = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	Get_PartObject<CEffectParts>("Bombard_Smoke")->Get_Component<CTransform>()->Translation(vOwnerLook);

}

void CBombard::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLight_Projectile);

}
#ifdef _DEBUG
void CBombard::Describe_Entity()
{

}
#endif

HRESULT CBombard::Bind_ShaderResources()
{
	return S_OK;
}


