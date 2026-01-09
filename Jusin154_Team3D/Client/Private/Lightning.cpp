#include "pch.h"
#include "Lightning.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CLightning::CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CLightning::CLightning(const CLightning& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CLightning::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Lightning")))
		return E_FAIL;

	return S_OK;

}

HRESULT CLightning::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
	
	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC);

	m_wstrEffectName = L"Lightning";

	m_Events.emplace(0.5f, [&]() {

		_float fShakeValue = clamp(15.f / m_fDistance, 2.f, 5.f);

		static_cast<CPlayer*>(m_pOwner->Get_Owner())->Start_CameraShake(0.5f, fShakeValue);

		});


	m_fDuration = 5.5f;

	return S_OK;
}

void CLightning::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


}

void CLightning::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

}

void CLightning::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);

	if (false == m_bHit) {
		_vector vTop = XMLoadFloat4(&m_vTargetPos) + XMVectorSet(0.f, 5.f, 0.f, 0.f);

		_vector vBottom = XMLoadFloat4(&m_vTargetPos) - XMVectorSet(0.f, 5.f, 0.f, 0.f);

		ON_COLLISION_INFO CollisionInfo = SweepTarget(vTop, vBottom, 0.02f, true);

		OnCollision(this, &CollisionInfo);
	}
}

HRESULT CLightning::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;


	CPartObject* pBottomLightning = Get_PartObject<CEffectParts>("BottomLightning");
	CPartObject* pFlare = Get_PartObject<CEffectParts>("Flare");
	CPartObject* pHitPT0 = Get_PartObject<CEffectParts>("HitPT0");
	CPartObject* pLightning_Tornado_PT = Get_PartObject<CEffectParts>("Lightning_Tornado_PT");
	CPartObject* pLightning0 = Get_PartObject<CEffectParts>("Lightning0");
	CPartObject* pStart_Line = Get_PartObject<CEffectParts>("Start_Line");
	CPartObject* pLightning_Smoke = Get_PartObject<CEffectParts>("Lightning_Smoke");
	CPartObject* pLightning_Rock_PT = Get_PartObject<CEffectParts>("Lightning_Rock_PT");

	_vector vDirection = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	XMStoreFloat3(&m_vCameraLook, vDirection);

	_vector vStartPos = pWand->Get_Owner()->Get_Component<CCharacter_Controller>()->Get_FootPosition();
	XMStoreFloat4(&m_vStartPos, vStartPos);

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

	pBottomLightning->Set_Visible(true);
	pFlare->Set_Visible(true);
	pHitPT0->Set_Visible(true);
	pLightning_Tornado_PT->Set_Visible(true);
	pLightning0->Set_Visible(true);
	pStart_Line->Set_Visible(true);
	pLightning_Smoke->Set_Visible(true);
	//pLightning_Rock_PT->Set_Visible(true);

	pBottomLightning->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pFlare->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pHitPT0->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pLightning_Tornado_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pLightning0->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pStart_Line->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	pLightning_Smoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));
	//pLightning_Rock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&m_vTargetPos));

	m_fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));



	return S_OK;
}

HRESULT CLightning::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLightning::Ready_Child()
{
	return S_OK;
}

CLightning* CLightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightning* pInstance = new CLightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLightning");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CLightning::Clone(void* pArg, CGameObject* pOwner)
{
	CLightning* pInstance = new CLightning(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLightning");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLightning::OnCollision(CGameObject* pOther, void* pDesc)
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

	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return;


}

void CLightning::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CLightning::Describe_Entity()
{

}
#endif

HRESULT CLightning::Bind_ShaderResources()
{
	return S_OK;
}


