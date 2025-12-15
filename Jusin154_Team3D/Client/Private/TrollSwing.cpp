#include "pch.h"
#include "TrollSwing.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CTrollSwing::CTrollSwing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTrollSwing::CTrollSwing(const CTrollSwing& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CTrollSwing::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/TrollSwing")))
		return E_FAIL;

	return S_OK;

}

HRESULT CTrollSwing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"TrollSwing";


	m_fDuration = 4.2f;

	return S_OK;
}

void CTrollSwing::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CTrollSwing::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



}

void CTrollSwing::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CTrollSwing::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_vector vPos = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::POSITION);

	vPos += m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK) * 2.f;

	CEffectParts* pRockPT = Get_PartObject<CEffectParts>("Rock_PT_25");
	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("Smoke");

	pRockPT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos + XMVectorSet(0.f, 0.2f, 0.f, 0.f));


	pRockPT->Set_Visible(true);
	pSmoke->Set_Visible(true);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first);


	if (pPlayer == nullptr)
		return E_FAIL;

	_float fDistance = XMVectorGetX(XMVector4Length(pPlayer->Get_WorldPostion() - Get_WorldPostion()));

	_float fShakeValue = clamp(50.f / fDistance, 3.f, 10.f);

	pPlayer->Start_CameraShake(1.2f, fShakeValue);

	return S_OK;
}

HRESULT CTrollSwing::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrollSwing::Ready_Child()
{
	return S_OK;
}

CTrollSwing* CTrollSwing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrollSwing* pInstance = new CTrollSwing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrollSwing");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CTrollSwing::Clone(void* pArg, CGameObject* pOwner)
{
	CTrollSwing* pInstance = new CTrollSwing(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrollSwing");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTrollSwing::OnCollision(CGameObject* pOther, void* pDesc)
{
	_int iIndex = CollisionCheck();

	if (iIndex < 0)
		return;

	if (m_isCollisionEnter == true)
		return;

	m_isCollisionEnter = true;

	_vector vPos = XMVectorSet(m_Hitbuffer.touches[iIndex].position.x, m_Hitbuffer.touches[iIndex].position.y, m_Hitbuffer.touches[iIndex].position.z, 1.f);


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}


}

void CTrollSwing::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}
#ifdef _DEBUG
void CTrollSwing::Describe_Entity()
{

}
#endif

HRESULT CTrollSwing::Bind_ShaderResources()
{
	return S_OK;
}


