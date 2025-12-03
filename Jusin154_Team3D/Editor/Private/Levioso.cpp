#include "pch.h"
#include "Levioso.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Player.h"
#include "TrailObject.h"


CLevioso::CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CLevioso::CLevioso(const CLevioso& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CLevioso::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CLevioso::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Levioso")))
		return E_FAIL;

	m_pLeviosoPJ_0 = Get_PartObject<CEditEffect>("Levioso_PJ0");
	m_pLeviosoPJ_1 = Get_PartObject<CEditEffect>("Levioso_PJ1");
	m_pLeviosoTrail = Get_PartObject<CTrailObject>("Levioso_Trail");
	m_pTrail_PT_0 = Get_PartObject<CEditEffect>("Trail_PT0");

	SAFE_ADDREF(m_pLeviosoPJ_0);
	SAFE_ADDREF(m_pLeviosoPJ_1);
	SAFE_ADDREF(m_pLeviosoTrail);
	SAFE_ADDREF(m_pTrail_PT_0);

	m_wstrEffectName = L"Levioso";

	m_fTurnSpeed = 10.f;
	m_fRange = 1.f;

	m_fDuration = 2.f;



	return S_OK;
}

void CLevioso::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pLeviosoPJ_0->Get_WorldPostion());

}

void CLevioso::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	m_pLeviosoPJ_0->Get_Component<CTransform>()->Translation(m_vOwnerLook * 0.25f);
	m_pLeviosoPJ_1->Get_Component<CTransform>()->Translation(m_vOwnerLook * 0.25f);


	m_pTrail_PT_0->Get_Component<CTransform>()->Translation(m_vOwnerLook * 0.25f);

	if (true == m_pGameInstance->SphereCast(0.125f, XMLoadFloat4(&m_vStartPos), m_vOwnerLook, XMVectorGetX(XMVector3Length(m_vOwnerLook * 1.f))
		, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eSTATIC, hitBuffer))
	{
		OnCollision();
	}

	_matrix WorldMat = m_pTrail_PT_0->Get_Component<CTransform>()->Get_XMWorldMatrix();

	m_pLeviosoTrail->Trail_Update(WorldMat, fTimeDelta);

}

void CLevioso::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return;

	_matrix WandWorld = pPlayer->Get_WandPos();

	//m_pLeviosoTrail->Get_Component<CTrail>()->Fixed_Trail(WandWorld);

	XMStoreFloat4(&m_vEndPos, m_pLeviosoPJ_0->Get_WorldPostion());

	__super::Late_Update(fTimeDelta);


}

HRESULT CLevioso::Pre_Setting(CGameObject* pObject)
{
	if (pObject == nullptr)
		return E_FAIL;

	m_pOwner = pObject;


	Reset_EditEffect();

	m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;

	m_fAccRotateTime = 0.f;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (pPlayer == nullptr)
		return E_FAIL;

	_vector WandPos = pPlayer->Get_WandPos().r[3];

	m_pLeviosoPJ_0->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());
	m_pLeviosoPJ_1->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());
	m_pTrail_PT_0->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());

	m_pLeviosoPJ_0->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);
	m_pLeviosoPJ_1->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);
	m_pTrail_PT_0->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);


	m_pLeviosoPJ_0->Set_Visible(true);
	m_pLeviosoPJ_1->Set_Visible(true);
	m_pTrail_PT_0->Set_Visible(true);

	m_pLeviosoTrail->Set_Visible(true);
	m_pLeviosoTrail->Get_Component<CTrail>()->Reset_Trail();

	m_vOwnerLook = XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));

	m_bVisible = true;

	return S_OK;
}

HRESULT CLevioso::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevioso::Ready_Child()
{

	return S_OK;
}

CLevioso* CLevioso::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevioso* pInstance = new CLevioso(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLevioso");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CLevioso::Clone(void* pArg, CGameObject* pOwner)
{
	CLevioso* pInstance = new CLevioso(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLevioso");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLevioso::OnCollision(CGameObject* pOther, void* pDesc)
{
	_vector vPos = XMVectorSet(hitBuffer.block.position.x, hitBuffer.block.position.y, hitBuffer.block.position.z, 1.f);


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}

	m_fAccTime = 0.f;
	m_fDuration = 3.f; //적중하면 지속시간 3초

	//CEditEffect* pLevioso_Hit =  Get_PartObject<CEditEffect>("Levioso_Hit");
	//CEditEffect* pLevioso_Bottom_Wind = Get_PartObject<CEditEffect>("Levioso_Bottom_Wind");
	//CEditEffect* pLevioso_Rotate0 = Get_PartObject<CEditEffect>("Levioso_Rotate0");
	//CEditEffect* pLevioso_Rotate1 = Get_PartObject<CEditEffect>("Levioso_Rotate1");
	//CEditEffect* pLevioso_Tornado = Get_PartObject<CEditEffect>("Levioso_Tornado");

	m_pLeviosoPJ_0->Set_Visible(false);
	m_pLeviosoPJ_1->Set_Visible(false);
	m_pTrail_PT_0->Set_Visible(false);
	m_pLeviosoTrail->Set_Visible(false);
}

void CLevioso::Free()
{
	__super::Free();


	SAFE_RELEASE(m_pLeviosoPJ_0);
	SAFE_RELEASE(m_pLeviosoPJ_1);
	SAFE_RELEASE(m_pLeviosoTrail);
	SAFE_RELEASE(m_pTrail_PT_0);
}
#ifdef _DEBUG

void CLevioso::Describe_Entity()
{
	GUI::Begin("LEVIOSO");

	GUI::DragFloat("TURN SPEED", &m_fTurnSpeed);
	GUI::DragFloat("RANGE", &m_fRange);
	GUI::End();

}

#endif // _DEBUG

HRESULT CLevioso::Bind_ShaderResources()
{
	return S_OK;
}


