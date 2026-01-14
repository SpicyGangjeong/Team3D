#include "pch.h"
#include "PotionScreen.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "Goblin_BattleAxe.h"
#include "TrailObject.h"

CPotionScreen::CPotionScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CPotionScreen::CPotionScreen(const CPotionScreen& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CPotionScreen::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/PotionScreen")))
		return E_FAIL;

	return S_OK;

}

HRESULT CPotionScreen::Initialize(void* pArg)
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

void CPotionScreen::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CPotionScreen::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CPotionScreen::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CPotionScreen::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	CEffectParts* pPotion_ScreenFX = Get_PartObject<CEffectParts>("Potion_ScreenFX");

	pPotion_ScreenFX->Set_Visible(true);


	return S_OK;
}

HRESULT CPotionScreen::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPotionScreen::Ready_Child()
{
	return S_OK;
}

CPotionScreen* CPotionScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPotionScreen* pInstance = new CPotionScreen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPotionScreen");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CPotionScreen::Clone(void* pArg, CGameObject* pOwner)
{
	CPotionScreen* pInstance = new CPotionScreen(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPotionScreen");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CPotionScreen::OnCollision(CGameObject* pOther, void* pDesc)
{


}

void CPotionScreen::Free()
{
	__super::Free();


}
#ifdef _DEBUG
void CPotionScreen::Describe_Entity()
{

}
#endif

HRESULT CPotionScreen::Bind_ShaderResources()
{
	return S_OK;
}


