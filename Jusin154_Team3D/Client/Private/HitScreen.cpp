#include "pch.h"
#include "HitScreen.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "Goblin_BattleAxe.h"
#include "TrailObject.h"

CHitScreen::CHitScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CHitScreen::CHitScreen(const CHitScreen& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CHitScreen::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/HitScreen")))
		return E_FAIL;

	return S_OK;

}

HRESULT CHitScreen::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;




	m_fDuration = 4.f;



	return S_OK;
}

void CHitScreen::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CHitScreen::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CHitScreen::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CHitScreen::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	CEffectParts* pHit_Screen_FX = Get_PartObject<CEffectParts>("Hit_Screen_FX");

	pHit_Screen_FX->Set_Visible(true);


	return S_OK;
}

HRESULT CHitScreen::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHitScreen::Ready_Child()
{
	return S_OK;
}

CHitScreen* CHitScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHitScreen* pInstance = new CHitScreen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHitScreen");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CHitScreen::Clone(void* pArg, CGameObject* pOwner)
{
	CHitScreen* pInstance = new CHitScreen(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHitScreen");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CHitScreen::OnCollision(CGameObject* pOther, void* pDesc)
{


}

void CHitScreen::Free()
{
	__super::Free();


}
#ifdef _DEBUG
void CHitScreen::Describe_Entity()
{

}
#endif

HRESULT CHitScreen::Bind_ShaderResources()
{
	return S_OK;
}


