#include "pch.h"
#include "BroomRaceManager.h"

#include "GameInstance.h"
#include "RaceRing.h"
#include "Layer.h"
#include "BroomRacerAI.h"
#include "Player.h"
#include "Broom.h"

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


	return S_OK;
}

void CBroomRaceManager::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	switch (m_eRaceState)
	{
	case ENUM_CLASS(RACE_STATE::COUNTDOWN):
		Update_Countdown(fTimeDelta);
		break;
	case ENUM_CLASS(RACE_STATE::RACING):
		Check_RingPassed();
		break;
	}

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
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CBroomRaceManager::Render()
{
	m_pGameInstance->Render_Text(TEXT("Font_size20"), ToolTip.c_str(), _float2(g_iWinSizeX*0.5f, g_iWinSizeY*0.5f),XMVectorSet(1.f,1.f,1.f,1.f),5.f);

	return S_OK;
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
	if (GUI::Button("Race Start"))
	{
		m_eRaceState = ENUM_CLASS(RACE_STATE::READY);
		const _float SPAWN_DISTANCE = 80.f;

		for (auto& racer : m_Racers)
		{
			CTransform* pRingTransform =
				m_pRaceRings[0]->Get_Component<CTransform>();

			_vector ringPos = pRingTransform->Get_State(STATE::POSITION);
			_vector ringLook = pRingTransform->Get_State(STATE::RIGHT);
			ringLook = XMVector3Normalize(ringLook);

			_vector spawnPos = ringPos - ringLook * SPAWN_DISTANCE;

			if (racer.pAI)
			{
				CTransform* pBroomTransform =
					racer.pAI->Get_Broom()->Get_Component<CTransform>();
				_float fRand = m_pGameInstance->Real_Random_Float(-10.f, 10.f);
				spawnPos.m128_f32[2] += fRand;

				pBroomTransform->Set_State(STATE::POSITION, spawnPos);
				pBroomTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
				racer.pAI->Get_Broom()->Set_Move(false);
			}
			else if (racer.pRacer)
			{
				CTransform* pTransform =
					racer.pRacer->Get_Component<CTransform>();
				spawnPos.m128_f32[1] -= 2.f;
				pTransform->Set_State(STATE::POSITION,spawnPos);
				pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
				racer.pRacer->Get_Component<CFSM>()->Change_State(FSMSTATE::BROOM_RIDE);
				racer.pRacer->Get_Broom()->Set_Move(false);
			}
		}
	}
	if (m_eRaceState == ENUM_CLASS(RACE_STATE::READY))
	{
		if (GUI::Button("Countdown"))
		{
			m_eRaceState = ENUM_CLASS(RACE_STATE::COUNTDOWN);
		}
	}
}


#endif // _DEBUG

void CBroomRaceManager::Update_Countdown(_float fTimeDelta)
{
	m_fCountTimer += fTimeDelta;

	if (m_fCountTimer >= 1.f)
	{
		m_fCountTimer = 0.f;
		--m_iCount;

		if (m_iCount > 0)
		{
			ToolTip = to_wstring(m_iCount);
		}
		else
		{
			ToolTip = to_wstring(0);
			StartRaceMove();
		}
	}
}

void CBroomRaceManager::StartRaceMove()
{
	for (auto& racer : m_Racers)
	{
		if (racer.pAI)
		{
			racer.pAI->Get_Broom()->Set_Move(true);
		}
		else if (racer.pRacer)
		{
			racer.pRacer->Get_Broom()->Set_Move(true);
		}
	}

	m_eRaceState = ENUM_CLASS(RACE_STATE::RACING);
}



void CBroomRaceManager::Check_RingPassed()
{
	const float PASS_RADIUS = 5.f;

	for (auto& racer : m_Racers)
	{
		CRaceRing* pRing = m_pRaceRings[racer.curRing];

		_vector ringPos = pRing->Get_WorldPostion();

		_vector ringFwd =
			pRing->Get_Component<CTransform>()->Get_State(STATE::RIGHT);
		ringFwd = XMVector3Normalize(ringFwd);


		_vector currPos = racer.pAI ? racer.pAI->Get_WorldPostion() : racer.pRacer->Get_WorldPostion();

		_vector prevPos = racer.prevPos;

		_float d0 = XMVectorGetX(
			XMVector3Dot(prevPos - ringPos, ringFwd));
		_float d1 = XMVectorGetX(
			XMVector3Dot(currPos - ringPos, ringFwd));
#ifdef _DEBUG
		GUI::Text("%s", racer.pAI ? "AI" : "Player");
		GUI::SameLine();
		GUI::Text("CurRing %d", racer.curRing);
		GUI::Text("PrevSide %.2f", d0);
		GUI::Text("CurrSide %.2f", d1);
#endif // _DEBUG
		if (d0 * d1 <= 0.f && fabs(d0 - d1) > 1e-4f)
		{
			_float t = d0 / (d0 - d1);
			t = std::clamp(t, 0.f, 1.f);

			_vector hitPos =
				prevPos + (currPos - prevPos) * t;

			_vector v = hitPos - ringPos;
			_float side = XMVectorGetX(XMVector3Dot(v, ringFwd));
			_vector proj = hitPos - ringFwd * side;

			_float dist =
				XMVectorGetX(XMVector3Length(proj - ringPos));

			if (dist <= PASS_RADIUS)
			{
				if(racer.pRacer){ 
					pRing->Set_Target(false); 
				}
					

				racer.curRing = (racer.curRing + 1) % m_pRaceRings.size();

				if (racer.pRacer) {
					m_pRaceRings[racer.curRing]->Set_Target(true);
				}
					
				racer.pAI ? SetTargetRing(racer.pAI) : SetTargetRing(racer.pRacer);
			}
		}

		racer.prevPos = currPos;
	}
}


void CBroomRaceManager::SetTargetRing(CGameObject*pRacer)
{ 
	for (auto& racer : m_Racers)
	{
		if (racer.pAI)
		{
			if (racer.pAI == pRacer)
			{
				if (racer.pAI)
				{
					racer.pAI->Set_RaceRing(m_pRaceRings[racer.curRing]);
				}
			}
		}
		else if (racer.pRacer)
		{
			if (racer.pRacer == pRacer)
			{
				racer.pRacer->Set_RaceRing(m_pRaceRings[racer.curRing]);
			}
		}
	}
}

void CBroomRaceManager::Push_BroomRacer(RacerInfo Info)
{
	RacerInfo info;
	info.pRacer = Info.pRacer;
	info.pAI = Info.pAI;
	info.curRing = Info.curRing;
	info.prevPos = Info.prevPos;

	if(Info.pAI)
		info.pAI->Set_RaceRing(m_pRaceRings[info.curRing]);
	else if(Info.pRacer)
		info.pRacer->Set_RaceRing(m_pRaceRings[info.curRing]);

	m_Racers.push_back(info);
	SAFE_ADDREF(Info.pRacer);
}

void CBroomRaceManager::Push_RaceRing(CRaceRing* Ring)
{
	m_pRaceRings.push_back(Ring);
	SAFE_ADDREF(Ring);
}

