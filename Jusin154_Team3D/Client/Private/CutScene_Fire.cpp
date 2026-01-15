#include "pch.h"
#include "CutScene_Fire.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"

CCutScene_Fire::CCutScene_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CCutScene_Fire::CCutScene_Fire(const CCutScene_Fire& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CCutScene_Fire::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/CutSceneFire")))
		return E_FAIL;

	return S_OK;

}

HRESULT CCutScene_Fire::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"CutSceneFire";


	m_fDuration = 999.f;
	return S_OK;
}

void CCutScene_Fire::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CCutScene_Fire::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CCutScene_Fire::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CCutScene_Fire::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;

	CEffectParts* pFire = Get_PartObject<CEffectParts>("Fire");

	_float4* vPos = static_cast<_float4*>(pArg);
	_vector vPosVector = XMLoadFloat4(vPos);
	pFire->Set_Visible(true);


	pFire->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);

	return S_OK;
}

HRESULT CCutScene_Fire::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCutScene_Fire::Ready_Child()
{
	return S_OK;
}

CCutScene_Fire* CCutScene_Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutScene_Fire* pInstance = new CCutScene_Fire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCutScene_Fire");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CCutScene_Fire::Clone(void* pArg, CGameObject* pOwner)
{
	CCutScene_Fire* pInstance = new CCutScene_Fire(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCutScene_Fire");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCutScene_Fire::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CCutScene_Fire::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CCutScene_Fire::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CCutScene_Fire::Bind_ShaderResources()
{
	return S_OK;
}


