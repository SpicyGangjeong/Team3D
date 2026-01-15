#include "pch.h"
#include "CutScene_Lightning.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"

CCutScene_Lightning::CCutScene_Lightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CCutScene_Lightning::CCutScene_Lightning(const CCutScene_Lightning& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CCutScene_Lightning::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/CutSceneLightning")))
		return E_FAIL;

	return S_OK;

}

HRESULT CCutScene_Lightning::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"CutScene_Lightning";


	m_fDuration = 3.f;

	return S_OK;
}

void CCutScene_Lightning::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CCutScene_Lightning::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CCutScene_Lightning::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CCutScene_Lightning::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;

	CEffectParts* pLightning = Get_PartObject<CEffectParts>("Lightning_1");
	CEffectParts* pScreenFlash = Get_PartObject<CEffectParts>("Screen_Flash");

	_float4* vPos = static_cast<_float4*>(pArg);
	_vector vPosVector = XMLoadFloat4(vPos);
	pLightning->Set_Visible(true);
	pScreenFlash->Set_Visible(true);


	pLightning->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);

	return S_OK;
}

HRESULT CCutScene_Lightning::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCutScene_Lightning::Ready_Child()
{
	return S_OK;
}

CCutScene_Lightning* CCutScene_Lightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutScene_Lightning* pInstance = new CCutScene_Lightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCutScene_Lightning");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CCutScene_Lightning::Clone(void* pArg, CGameObject* pOwner)
{
	CCutScene_Lightning* pInstance = new CCutScene_Lightning(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCutScene_Lightning");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCutScene_Lightning::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CCutScene_Lightning::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CCutScene_Lightning::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CCutScene_Lightning::Bind_ShaderResources()
{
	return S_OK;
}


