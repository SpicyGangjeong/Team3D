#include "pch.h"
#include "BombardSide.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "Wand.h"
#include "TrailObject.h"


CBombardSide::CBombardSide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CBombardSide::CBombardSide(const CBombardSide& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CBombardSide::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CBombardSide::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/BombardaSide")))
		return E_FAIL;

	m_wstrEffectName = L"BombardSide";


	m_pWandParticle = Get_PartObject<CEffectParts>("WandParticle_B_100");
	m_pWandLight = Get_PartObject<CEffectParts>("Wand_Light_B");
	m_pWandTrail = Get_PartObject<CTrailObject>();

	SAFE_ADDREF(m_pWandLight);
	SAFE_ADDREF(m_pWandTrail);
	SAFE_ADDREF(m_pWandParticle);

	m_fDuration = 1.f;

	m_Events.emplace(0.4f, [&](){
		m_pWandTrail->Set_Visible(false);
	});

	return S_OK;
}

void CBombardSide::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CBombardSide::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return;

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	m_pWandTrail->Get_Component<CTrail>()->Trail_Update(fTimeDelta, pWand->Get_WorldMatrix());
	m_pWandParticle->Get_Component<CTransform>()->Set_WorldMatrix(pWand->Get_WorldMatrix());

}

void CBombardSide::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CBombardSide::Pre_Setting(CGameObject* pObject)
{
	if (pObject == nullptr)
		return E_FAIL;

	m_pOwner = pObject;

	Reset_EffectParts();

	m_fAccTime = 0.f;
	__super::m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;

	m_pWandParticle->Set_Visible(true);

	m_pWandLight->Set_Visible(true);

	/*트레일 초기화 */
	m_pWandTrail->Set_Visible(true);
	m_pWandTrail->Get_Component<CTrail>()->Reset_Trail();

	/*_vector pPos = pPlayer->Get_WandPos().r[3];*/

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());


	m_bVisible = true;

	return S_OK;
}

HRESULT CBombardSide::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBombardSide::Ready_Child()
{
	return S_OK;
}

CBombardSide* CBombardSide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBombardSide* pInstance = new CBombardSide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBombardSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CBombardSide::Clone(void* pArg, CGameObject* pOwner)
{
	CBombardSide* pInstance = new CBombardSide(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBombardSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBombardSide::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CBombardSide::Free()
{
	__super::Free();


	Safe_Release(m_pWandLight);
	Safe_Release(m_pWandTrail);
	Safe_Release(m_pWandParticle);
}
#ifdef _DEBUG

void CBombardSide::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CBombardSide::Bind_ShaderResources()
{
	return S_OK;
}


