#include "pch.h"
#include "Reparo.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "Wand.h"
#include "TrailObject.h"


CReparo::CReparo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }

{
}

CReparo::CReparo(const CReparo& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CReparo::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Reparo")))
		return E_FAIL;

	return S_OK;

}

HRESULT CReparo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"Reparo";

	m_pWandLight = Get_PartObject<CEffectParts>("Wand_Light");
	m_pWandTrail = Get_PartObject<CTrailObject>();
	m_pWandParticle = Get_PartObject<CEffectParts>("Reparo_Wand_PT");

	SAFE_ADDREF(m_pWandLight);
	SAFE_ADDREF(m_pWandTrail);
	SAFE_ADDREF(m_pWandParticle);

	m_fDuration = 10.f;



	m_Events.emplace(6.f, [&]() {
		m_isParticleEnd = true;
		m_pWandTrail->SetDissolve(true);
		m_pWandParticle->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f , -999.f , 0.f , 1.f));
	});

	return S_OK;
}

void CReparo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CReparo::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return;

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());


	_matrix TrailMat = m_isTrailEnd ? XMLoadFloat4x4(&m_TrailStopMat) : pWand->Get_WorldMatrix();
	m_pWandTrail->Oneside_Rope_Trail_Update(TrailMat, fTimeDelta);

	if (m_isParticleEnd == false)
	{
		m_pWandParticle->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	}

}

void CReparo::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CReparo::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;

	m_isTrailEnd = false;
	m_isParticleEnd = false;
	m_pWandTrail->SetDissolve(false);

	m_pWandLight->Set_Visible(true);
	m_pWandParticle->Set_Visible(true);

	/*트레일 초기화 */
	m_pWandTrail->Set_Visible(true);

	m_pWandTrail->Get_Component<CTrail>()->Reset_Trail();

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	m_pWandParticle->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	return S_OK;
}

HRESULT CReparo::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CReparo::Ready_Child()
{
	return S_OK;
}

CReparo* CReparo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReparo* pInstance = new CReparo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CReparo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CReparo::Clone(void* pArg, CGameObject* pOwner)
{
	CReparo* pInstance = new CReparo(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CReparo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CReparo::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CReparo::Free()
{
	__super::Free();


	Safe_Release(m_pWandLight);
	Safe_Release(m_pWandTrail);
	Safe_Release(m_pWandParticle);
}
#ifdef _DEBUG

void CReparo::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CReparo::Bind_ShaderResources()
{
	return S_OK;
}


