#include "pch.h"
#include "NomalJapSide.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "Wand.h"
#include "TrailObject.h"


CNomalJapSide::CNomalJapSide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CNomalJapSide::CNomalJapSide(const CNomalJapSide& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CNomalJapSide::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/JapsideEffect")))
		return E_FAIL;

	return S_OK;
}

HRESULT CNomalJapSide::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"JapSideEffect";


	m_pWandParticle = Get_PartObject<CEffectParts>("WandParticle1");
	m_pWandLight = Get_PartObject<CEffectParts>("Wand_Red_Long");
	m_pWandLight_R = Get_PartObject<CEffectParts>("Wand_Light_R");
	m_pWandTrail = Get_PartObject<CTrailObject>();

	SAFE_ADDREF(m_pWandLight);
	SAFE_ADDREF(m_pWandTrail);
	SAFE_ADDREF(m_pWandParticle);
	SAFE_ADDREF(m_pWandLight_R);

	m_Events.emplace(0.6f, [&]() {

		// 파티클 제어
		m_isParticleEnd = true;
		m_pWandParticle->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));


		// 트레일 제어
		CWand* pWand = static_cast<CWand*>(m_pOwner);

		if (pWand == nullptr)
			return;

		m_isTrailEnd = true;

		XMStoreFloat4x4(&m_TrailStopMat, pWand->Get_WorldMatrix());
		m_pWandLight_R->Set_Visible(false);

		});

	m_fDuration = 2.f;

	return S_OK;
}

void CNomalJapSide::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CNomalJapSide::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return;

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	m_pWandLight_R->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	// 파티클생성이 막히면 실행되지 않음
	if (m_isParticleEnd == false)
		m_pWandParticle->Get_Component<CTransform>()->Set_WorldMatrix(pWand->Get_WorldMatrix());

	// 트레일이 종료되면 위치를 고정함
	_matrix TrailMat = m_isTrailEnd ? XMLoadFloat4x4(&m_TrailStopMat) : pWand->Get_WorldMatrix();
	m_pWandTrail->Trail_Update(TrailMat, fTimeDelta);

}

void CNomalJapSide::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CNomalJapSide::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;

	m_pWandParticle->Set_Visible(true);
	m_isParticleEnd = false; // 파티클이 일정시간이 되면 나오지 않게 하려고

	m_pWandLight->Set_Visible(true);
	m_pWandLight_R->Set_Visible(true);

	/*트레일 초기화 */
	m_pWandTrail->Set_Visible(true);
	m_isTrailEnd = false;
	m_pWandTrail->Get_Component<CTrail>()->Reset_Trail();

	/*_vector pPos = pPlayer->Get_WandPos().r[3];*/

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	m_pWandLight_R->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	return S_OK;
}

HRESULT CNomalJapSide::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNomalJapSide::Ready_Child()
{
	return S_OK;
}

CNomalJapSide* CNomalJapSide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNomalJapSide* pInstance = new CNomalJapSide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNomalJapSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CNomalJapSide::Clone(void* pArg, CGameObject* pOwner)
{
	CNomalJapSide* pInstance = new CNomalJapSide(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNomalJapSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CNomalJapSide::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CNomalJapSide::Free()
{
	__super::Free();


	Safe_Release(m_pWandLight);
	Safe_Release(m_pWandTrail);
	Safe_Release(m_pWandParticle);
	Safe_Release(m_pWandLight_R);
	
}
#ifdef _DEBUG

void CNomalJapSide::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CNomalJapSide::Bind_ShaderResources()
{
	return S_OK;
}


