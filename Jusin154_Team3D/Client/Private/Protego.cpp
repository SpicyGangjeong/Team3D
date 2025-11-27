#include "pch.h"
#include "Protego.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "PhysXEffectHitBox.h"


CProtego::CProtego(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CProtego::CProtego(const CProtego& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CProtego::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CProtego::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Protego")))
		return E_FAIL;

	m_pSphere = Get_PartObject<CEffectParts>("ProtegoSphere");
	m_pBottom = Get_PartObject<CEffectParts>("ProtegoBottom");
	m_pCircle = Get_PartObject<CEffectParts>("ProtegoCircle");

	SAFE_ADDREF(m_pSphere);
	SAFE_ADDREF(m_pBottom);
	SAFE_ADDREF(m_pCircle);

	m_wstrEffectName = L"Protego";


	m_fDuration = 2.5f;

	return S_OK;
}

void CProtego::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CProtego::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pSphere->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pBottom->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pCircle-> Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);
}

void CProtego::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CProtego::Pre_Setting(CGameObject* pObject)
{
	if (pObject == nullptr)
		return E_FAIL;

	m_pOwner = pObject;

	Reset_EffectParts();

	m_fAccTime = 0.f;
	__super::m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;

	m_pSphere->Set_Visible(true);
	m_pCircle->Set_Visible(true);
	m_pBottom->Set_Visible(true);

	m_bVisible = true;

	return S_OK;
}

HRESULT CProtego::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CProtego::Ready_Child()
{
	return S_OK;
}

CProtego* CProtego::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProtego* pInstance = new CProtego(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProtego");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CProtego::Clone(void* pArg, CGameObject* pOwner)
{
	CProtego* pInstance = new CProtego(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProtego");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CProtego::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CProtego::Free()
{
	__super::Free();

	//if(m_pPhysHitBox != nullptr)
	//	if (m_pPhysHitBox->Get_Depth() == false)
	//		SAFE_RELEASE(m_pPhysHitBox);

	SAFE_RELEASE(m_pSphere);
	SAFE_RELEASE(m_pBottom);
	SAFE_RELEASE(m_pCircle);

}
#ifdef _DEBUG

void CProtego::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CProtego::Bind_ShaderResources()
{
	return S_OK;
}


