#include "pch.h"
#include "BroomRaceManager.h"

#include "GameInstance.h"
#include "RaceRing.h"
#include "Layer.h"

CBroomRaceManager::CBroomRaceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBroomRaceManager::CBroomRaceManager(const CBroomRaceManager& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CBroomRaceManager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroomRaceManager::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

#ifdef _DEBUG
	m_pGripShape = (GeometricPrimitive::CreateSphere(m_pContext, 1.f, 10, false, false));
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 1.2f, 10, false, false));
#endif // _DEBUG

	return S_OK;
}

void CBroomRaceManager::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	Check_RingPassed();
}

void CBroomRaceManager::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CBroomRaceManager::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}


HRESULT CBroomRaceManager::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(__super::Ready_Components(&Desc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroomRaceManager::Bind_ShaderResources()
{
	return S_OK;
}

CBroomRaceManager* CBroomRaceManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroomRaceManager* pInstance = new CBroomRaceManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroomRaceManager");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CBroomRaceManager::Clone(void* pArg, CGameObject* pOwner)
{
	CBroomRaceManager* pInstance = new CBroomRaceManager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroomRaceManager");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CBroomRaceManager::Free()
{
	__super::Free();

	for (auto& Racer : m_Racers)
	{
		SAFE_RELEASE(Racer.pRacer);
	}
	m_Racers.clear();

	for (auto& Ring : m_pRaceRings)
	{
		SAFE_RELEASE(Ring);
	}
	m_pRaceRings.clear();
}
#ifdef _DEBUG

void CBroomRaceManager::Describe_Entity()
{

}

#endif // _DEBUG

void CBroomRaceManager::Check_RingPassed()
{
	for (auto& racer : m_Racers)
	{
		CRaceRing* pRing = m_pRaceRings[racer.curRing];
		_vector ringPos = pRing->Get_WorldPostion();
		_vector broomPos = racer.pRacer->Get_WorldPostion();

		_vector ringFwd =pRing->Get_Component<CTransform>()->Get_State(STATE::LOOK);
		ringFwd = XMVector3Normalize(ringFwd);

		_float prevSide = XMVectorGetX(XMVector3Dot(ringFwd, racer.prevPos - ringPos));
		_float currSide = XMVectorGetX(XMVector3Dot(ringFwd, broomPos - ringPos));

		_float dist = XMVectorGetX(XMVector3Length(broomPos - ringPos));

		const _float PASS_RADIUS = 50.f;

		GUI::Text("CurRing %d", racer.curRing);

#ifdef _DEBUG
		m_pGripShape->Draw(m_pTransformCom->Get_XMWorldMatrix(), m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW), m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ), DirectX::Colors::Green, nullptr, true);
		m_pSubShape->Draw(pRing->Get_Component<CTransform>()->Get_XMWorldMatrix(), m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW), m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ), DirectX::Colors::Purple, nullptr, true);
#endif // _DEBUG

		if (prevSide < 0.f && currSide >= 0.f && dist < PASS_RADIUS)
		{
			racer.curRing++;
			if (racer.curRing >= m_pRaceRings.size())
				racer.curRing = 0;
		}

		racer.prevPos = broomPos;
	}
}

CRaceRing* CBroomRaceManager::GetTargetRing(CGameObject* pRacer)
{
	for (auto& racer : m_Racers)
	{
		if (racer.pRacer == pRacer)
			return m_pRaceRings[racer.curRing];
	}
	return nullptr;
}

void CBroomRaceManager::Push_BroomRacer(RacerInfo Info)
{
	RacerInfo info;
	info.pRacer = Info.pRacer;
	info.curRing = Info.curRing;
	info.prevPos = Info.prevPos;

	m_Racers.push_back(info);
	SAFE_ADDREF(Info.pRacer);
}

void CBroomRaceManager::Push_RaceRing(CRaceRing* Ring)
{
	m_pRaceRings.push_back(Ring);
	SAFE_ADDREF(Ring);
}

