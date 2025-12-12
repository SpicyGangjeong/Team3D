#include "pch.h"
#include "WandEnd.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"

CWandEnd::CWandEnd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CWandEnd::CWandEnd(const CWandEnd& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CWandEnd::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/WandEnd")))
		return E_FAIL;

	return S_OK;

}

HRESULT CWandEnd::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"Lumos";

	m_pWandPt = Get_PartObject<CEffectParts>("WandParticle_R");

	SAFE_ADDREF(m_pWandPt);

	m_fDuration = 2.f;

	m_Events.emplace(0.4f, [&]() {
		m_isParticleEnd = true;

		m_pWandPt->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));
		});

	return S_OK;
}

void CWandEnd::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CWandEnd::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	if (m_isParticleEnd == true)
		return;

	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return;

	m_pWandPt->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

}

void CWandEnd::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CWandEnd::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;

	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;

	m_pWandPt->Set_Visible(true);
	m_pWandPt->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	m_isParticleEnd = false;

	return S_OK;
}

HRESULT CWandEnd::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWandEnd::Ready_Child()
{
	return S_OK;
}

CWandEnd* CWandEnd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWandEnd* pInstance = new CWandEnd(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWandEnd");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CWandEnd::Clone(void* pArg, CGameObject* pOwner)
{
	CWandEnd* pInstance = new CWandEnd(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWandEnd");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CWandEnd::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CWandEnd::Free()
{
	__super::Free();

	Safe_Release(m_pWandPt);

}
#ifdef _DEBUG

void CWandEnd::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CWandEnd::Bind_ShaderResources()
{
	return S_OK;
}


