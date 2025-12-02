#include "pch.h"
#include "DecendoSide.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "Wand.h"
#include "TrailObject.h"


CDecendoSide::CDecendoSide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CDecendoSide::CDecendoSide(const CDecendoSide& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CDecendoSide::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CDecendoSide::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/DecendoSide")))
		return E_FAIL;

	m_wstrEffectName = L"DecendoSide";

	m_pWandLight = Get_PartObject<CEffectParts>("Wand_Light_R");
	m_pWandTrail = Get_PartObject<CTrailObject>();

	SAFE_ADDREF(m_pWandLight);
	SAFE_ADDREF(m_pWandTrail);

	m_fDuration = 1.f;

	return S_OK;
}

void CDecendoSide::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CDecendoSide::Update(_float fTimeDelta)
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

}

void CDecendoSide::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CDecendoSide::Pre_Setting(CGameObject* pObject)
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

	m_pWandLight->Set_Visible(true);

	/*트레일 초기화 */
	m_pWandTrail->Set_Visible(true);
	m_pWandTrail->Get_Component<CTrail>()->Reset_Trail();

	/*_vector pPos = pPlayer->Get_WandPos().r[3];*/

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());


	m_bVisible = true;

	return S_OK;
}

HRESULT CDecendoSide::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDecendoSide::Ready_Child()
{
	return S_OK;
}

CDecendoSide* CDecendoSide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecendoSide* pInstance = new CDecendoSide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDecendoSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CDecendoSide::Clone(void* pArg, CGameObject* pOwner)
{
	CDecendoSide* pInstance = new CDecendoSide(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDecendoSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDecendoSide::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CDecendoSide::Free()
{
	__super::Free();


	Safe_Release(m_pWandLight);
	Safe_Release(m_pWandTrail);
}
#ifdef _DEBUG

void CDecendoSide::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CDecendoSide::Bind_ShaderResources()
{
	return S_OK;
}


