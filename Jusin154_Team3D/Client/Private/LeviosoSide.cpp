#include "pch.h"
#include "LeviosoSide.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "Wand.h"
#include "TrailObject.h"


CLeviosoSide::CLeviosoSide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CLeviosoSide::CLeviosoSide(const CLeviosoSide& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CLeviosoSide::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CLeviosoSide::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/LeviosoSide")))
		return E_FAIL;

	m_wstrEffectName = L"LeviosoSide";


	m_pWandLight = Get_PartObject<CEffectParts>("Wand_Light_P");
	m_pWandTrail = Get_PartObject<CTrailObject>();

	SAFE_ADDREF(m_pWandLight);
	SAFE_ADDREF(m_pWandTrail);

	m_fDuration = 1.f;

	return S_OK;
}

void CLeviosoSide::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CLeviosoSide::Update(_float fTimeDelta)
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

void CLeviosoSide::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CLeviosoSide::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;


	m_pWandLight->Set_Visible(true);

	/*트레일 초기화 */
	m_pWandTrail->Set_Visible(true);
	m_pWandTrail->Get_Component<CTrail>()->Reset_Trail();

	/*_vector pPos = pPlayer->Get_WandPos().r[3];*/

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	return S_OK;
}

HRESULT CLeviosoSide::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLeviosoSide::Ready_Child()
{
	return S_OK;
}

CLeviosoSide* CLeviosoSide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLeviosoSide* pInstance = new CLeviosoSide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNomalJapSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CLeviosoSide::Clone(void* pArg, CGameObject* pOwner)
{
	CLeviosoSide* pInstance = new CLeviosoSide(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNomalJapSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLeviosoSide::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CLeviosoSide::Free()
{
	__super::Free();


	Safe_Release(m_pWandLight);
	Safe_Release(m_pWandTrail);
}
#ifdef _DEBUG

void CLeviosoSide::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CLeviosoSide::Bind_ShaderResources()
{
	return S_OK;
}


