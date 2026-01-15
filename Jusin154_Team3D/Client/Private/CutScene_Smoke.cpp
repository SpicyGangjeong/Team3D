#include "pch.h"
#include "CutScene_Smoke.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"

CCutScene_Smoke::CCutScene_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CCutScene_Smoke::CCutScene_Smoke(const CCutScene_Smoke& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CCutScene_Smoke::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/CutSceneSmoke")))
		return E_FAIL;

	return S_OK;

}

HRESULT CCutScene_Smoke::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"CutSceneSmoke";

	m_fDuration = 999.f;
	return S_OK;
}

void CCutScene_Smoke::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CCutScene_Smoke::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CCutScene_Smoke::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CCutScene_Smoke::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;

	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("Smoke");

	_float4* vPos = static_cast<_float4*>(pArg);
	_vector vPosVector = XMLoadFloat4(vPos);

	pSmoke->Set_Visible(true);


	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);

	return S_OK;
}

HRESULT CCutScene_Smoke::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCutScene_Smoke::Ready_Child()
{
	return S_OK;
}

CCutScene_Smoke* CCutScene_Smoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutScene_Smoke* pInstance = new CCutScene_Smoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCutScene_Smoke");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CCutScene_Smoke::Clone(void* pArg, CGameObject* pOwner)
{
	CCutScene_Smoke* pInstance = new CCutScene_Smoke(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCutScene_Smoke");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCutScene_Smoke::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CCutScene_Smoke::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CCutScene_Smoke::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CCutScene_Smoke::Bind_ShaderResources()
{
	return S_OK;
}


