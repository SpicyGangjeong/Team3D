#include "pch.h"
#include "BroomBoost.h"

#include "GameInstance.h"
#include "EditEffect.h"


CBroomBoost::CBroomBoost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CBroomBoost::CBroomBoost(const CBroomBoost& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CBroomBoost::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CBroomBoost::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/BroomEffect")))
		return E_FAIL;

	m_wstrEffectName = L"BroomEffect";

	m_pBoostParticle_Y = Get_PartObject<CEditEffect>("BoostParticle_Yellow");
	m_pBoostParticle_R = Get_PartObject<CEditEffect>("BoostParticle_Red");

	SAFE_ADDREF(m_pBoostParticle_Y);
	SAFE_ADDREF(m_pBoostParticle_R);

	m_fDuration = 9999.f;

	return S_OK;
}

void CBroomBoost::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CBroomBoost::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;


	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	_matrix BroomTail_Mat = XMLoadFloat4x4(m_pBroom_TailMat);

	for (int i = 0; i < 3; ++i) {
		BroomTail_Mat.r[i] = XMVector3Normalize(BroomTail_Mat.r[i]);
	}

	_matrix WorldMat = m_pTransformCom->Get_XMWorldMatrix();/* * XMMatrixRotationAxis(XMVectorSet(0.f , 1.f, 0.f ,0.f), XMConvertToRadians(180.f))*/;


	m_pBoostParticle_R->Get_Component<CTransform>()->Set_WorldMatrix(BroomTail_Mat * WorldMat);
	m_pBoostParticle_Y->Get_Component<CTransform>()->Set_WorldMatrix(BroomTail_Mat * WorldMat);
}

void CBroomBoost::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CBroomBoost::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_vector vHitPos = XMLoadFloat4(static_cast<_float4*>(pArg));

	CEditEffect* pBooster_ScreenFX =  Get_PartObject<CEditEffect>("Booster_ScreenFX");

	m_pBroom_TailMat = m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr("lantern_01");

	_matrix BroomTail_Mat = XMLoadFloat4x4(m_pBroom_TailMat);

	for (int i = 0; i < 3; ++i) {
		BroomTail_Mat.r[i] = XMVector3Normalize(BroomTail_Mat.r[i]);
	}

	_matrix WorldMat = m_pTransformCom->Get_XMWorldMatrix();/* * XMMatrixRotationAxis(XMVectorSet(0.f , 1.f, 0.f ,0.f), XMConvertToRadians(180.f))*/;


	m_pBoostParticle_R->Get_Component<CTransform>()->Set_WorldMatrix(BroomTail_Mat * WorldMat);
	m_pBoostParticle_Y->Get_Component<CTransform>()->Set_WorldMatrix(BroomTail_Mat * WorldMat);

	pBooster_ScreenFX->Set_Visible(true);
	m_pBoostParticle_Y->Set_Visible(true);
	m_pBoostParticle_R->Set_Visible(true);

	pBooster_ScreenFX->Get_Component<CInstance_Model>()->Set_Loop(true);

	return S_OK;
}

HRESULT CBroomBoost::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroomBoost::Ready_Child()
{
	return S_OK;
}

CBroomBoost* CBroomBoost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroomBoost* pInstance = new CBroomBoost(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroomBoost");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CBroomBoost::Clone(void* pArg, CGameObject* pOwner)
{
	CBroomBoost* pInstance = new CBroomBoost(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroomBoost");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroomBoost::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CBroomBoost::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pBoostParticle_Y);
	SAFE_RELEASE(m_pBoostParticle_R);
}
#ifdef _DEBUG

void CBroomBoost::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CBroomBoost::Bind_ShaderResources()
{
	return S_OK;
}


