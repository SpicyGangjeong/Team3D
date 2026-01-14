#include "pch.h"
#include "Revelio.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"


CRevelio::CRevelio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRevelio::CRevelio(const CRevelio& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CRevelio::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Revelio")))
		return E_FAIL;

	return S_OK;

}

HRESULT CRevelio::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_pRevelioPT_Y = Get_PartObject<CEffectParts>("WandPT_Y");
	m_pRevelioPT_B = Get_PartObject<CEffectParts>("WandPT_B");
	m_pRevelioPT_R = Get_PartObject<CEffectParts>("WandPT_R");
	m_pWand_Light = Get_PartObject<CEffectParts>("Wand_Light");

	SAFE_ADDREF(m_pRevelioPT_Y);
	SAFE_ADDREF(m_pRevelioPT_B);
	SAFE_ADDREF(m_pRevelioPT_R);
	SAFE_ADDREF(m_pWand_Light);

	m_wstrEffectName = L"Revelio";



	m_fDuration = 3.f;

	return S_OK;
}

void CRevelio::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRevelio::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return;

	_matrix WandWorld = pPlayer->Get_WandPos();

	/*_vector pPos = pPlayer->Get_WandPos().r[3];*/

	m_pRevelioPT_Y->Get_Component<CTransform>()->Set_WorldMatrix(WandWorld);
	m_pRevelioPT_R->Get_Component<CTransform>()->Set_WorldMatrix(WandWorld);
	m_pRevelioPT_B->Get_Component<CTransform>()->Set_WorldMatrix(WandWorld);
	m_pWand_Light->Get_Component<CTransform>()->Set_WorldMatrix(WandWorld);

}

void CRevelio::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRevelio::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return E_FAIL;

	_matrix WandWorld = pPlayer->Get_WandPos();

	/*_vector pPos = pPlayer->Get_WandPos().r[3];*/

	m_pRevelioPT_Y->Get_Component<CTransform>()->Set_WorldMatrix(WandWorld);
	m_pRevelioPT_R->Get_Component<CTransform>()->Set_WorldMatrix(WandWorld);
	m_pRevelioPT_B->Get_Component<CTransform>()->Set_WorldMatrix(WandWorld);
	m_pWand_Light->Get_Component<CTransform>()->Set_WorldMatrix(WandWorld);

	m_pRevelioPT_Y->Set_Visible(true);
	m_pRevelioPT_R->Set_Visible(true);
	m_pRevelioPT_B->Set_Visible(true);
	m_pWand_Light->Set_Visible(true);


	CEffectParts* pRevelio_ScreenFX = Get_PartObject<CEffectParts>("Revelio_ScreenFX");
	CEffectParts* pRevelio_ScreenFX_Reverse = Get_PartObject<CEffectParts>("Revelio_ScreenFX_Reverse");
	CEffectParts* pRevelio_Smoke = Get_PartObject<CEffectParts>("Revelio_Smoke");
	CEffectParts* pRevelio_Distortion = Get_PartObject<CEffectParts>("Revelio_Distortion");
	CEffectParts* pBottom_PT = Get_PartObject<CEffectParts>("Bottom_PT");

	pRevelio_ScreenFX->Set_Visible(true);
	pRevelio_ScreenFX_Reverse->Set_Visible(true);
	pRevelio_Smoke->Set_Visible(true);
	pRevelio_Distortion->Set_Visible(true);
	pBottom_PT->Set_Visible(true);


	CCharacter_Controller* pCCT = m_pOwner->Get_Component<CCharacter_Controller>();

	_vector vFootPos = {};
	_vector vPos = {};

	if (pCCT != nullptr)
	{
		vFootPos = pCCT->Get_FootPosition();
		vPos = pCCT->Get_Position();

		pRevelio_Smoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pBottom_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pRevelio_Distortion->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);


	}

	m_pGameInstance->Sound_Play(SOUND::SD_KIND::VOICE_REVELIO, SD_CHANNEL_GROUP::EFFECT, false, 0.7f);

	return S_OK;
}

HRESULT CRevelio::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRevelio::Ready_Child()
{
	return S_OK;
}

CRevelio* CRevelio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRevelio* pInstance = new CRevelio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRevelio");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRevelio::Clone(void* pArg, CGameObject* pOwner)
{
	CRevelio* pInstance = new CRevelio(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRevelio");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRevelio::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CRevelio::Free()
{
	__super::Free();

	//if(m_pPhysHitBox != nullptr)
	//	if (m_pPhysHitBox->Get_Depth() == false)
	//		SAFE_RELEASE(m_pPhysHitBox);


	SAFE_RELEASE(m_pRevelioPT_Y);
	SAFE_RELEASE(m_pRevelioPT_B);
	SAFE_RELEASE(m_pRevelioPT_R);
	SAFE_RELEASE(m_pWand_Light);
}
#ifdef _DEBUG

void CRevelio::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CRevelio::Bind_ShaderResources()
{
	return S_OK;
}


