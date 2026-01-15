#include "pch.h"
#include "CutScene_Shout.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"

CCutScene_Shout::CCutScene_Shout(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CCutScene_Shout::CCutScene_Shout(const CCutScene_Shout& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CCutScene_Shout::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Ranrok_Shout")))
		return E_FAIL;

	return S_OK;

}

HRESULT CCutScene_Shout::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"Ranrok_Shout";


	return S_OK;
}

void CCutScene_Shout::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CCutScene_Shout::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CCutScene_Shout::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CCutScene_Shout::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;

	CEffectParts* pShout = Get_PartObject<CEffectParts>("Shout");

	_float4* vPos = static_cast<_float4*>(pArg);
	_vector vPosVector = XMLoadFloat4(vPos);
	pShout->Set_Visible(true);

	pShout->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);

	return S_OK;
}

HRESULT CCutScene_Shout::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCutScene_Shout::Ready_Child()
{
	return S_OK;
}

CCutScene_Shout* CCutScene_Shout::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutScene_Shout* pInstance = new CCutScene_Shout(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCutScene_Shout");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CCutScene_Shout::Clone(void* pArg, CGameObject* pOwner)
{
	CCutScene_Shout* pInstance = new CCutScene_Shout(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCutScene_Shout");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCutScene_Shout::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CCutScene_Shout::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CCutScene_Shout::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CCutScene_Shout::Bind_ShaderResources()
{
	return S_OK;
}


