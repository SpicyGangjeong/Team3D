#include "pch.h"
#include "LightningSide.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "Wand.h"
#include "TrailObject.h"


CLightningSide::CLightningSide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }

{
}

CLightningSide::CLightningSide(const CLightningSide& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CLightningSide::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/LightningSide")))
		return E_FAIL;

	return S_OK;

}

HRESULT CLightningSide::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"LightningSide";

	m_pWandLight = Get_PartObject<CEffectParts>("Wand_Light_B");
	m_pWandTrail = Get_PartObject<CTrailObject>();

	SAFE_ADDREF(m_pWandLight);
	SAFE_ADDREF(m_pWandTrail);

	m_fDuration = 3.f;

	m_Events.emplace(0.5f, [&]() {
		CWand* pWand = static_cast<CWand*>(m_pOwner);

		if (pWand == nullptr)
			return;

		m_isTrailEnd = true;

		XMStoreFloat4x4(&m_TrailStopMat, pWand->Get_WorldMatrix());
		});

	return S_OK;
}

void CLightningSide::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CLightningSide::Update(_float fTimeDelta)
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
	m_pWandTrail->Trail_Update(TrailMat, fTimeDelta);

}

void CLightningSide::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CLightningSide::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;

	m_isTrailEnd = false;

	CEffectParts* pWand_Lightning = Get_PartObject<CEffectParts>("Wand_Lightning");

	m_pWandLight->Set_Visible(true);
	pWand_Lightning->Set_Visible(true);

	/*트레일 초기화 */
	m_pWandTrail->Set_Visible(true);
	m_pWandTrail->Get_Component<CTrail>()->Reset_Trail();

	/*_vector pPos = pPlayer->Get_WandPos().r[3];*/

	m_pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pWand_Lightning->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	return S_OK;
}

HRESULT CLightningSide::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLightningSide::Ready_Child()
{
	return S_OK;
}

CLightningSide* CLightningSide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightningSide* pInstance = new CLightningSide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLightningSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CLightningSide::Clone(void* pArg, CGameObject* pOwner)
{
	CLightningSide* pInstance = new CLightningSide(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLightningSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLightningSide::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CLightningSide::Free()
{
	__super::Free();


	Safe_Release(m_pWandLight);
	Safe_Release(m_pWandTrail);
}
#ifdef _DEBUG

void CLightningSide::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CLightningSide::Bind_ShaderResources()
{
	return S_OK;
}


