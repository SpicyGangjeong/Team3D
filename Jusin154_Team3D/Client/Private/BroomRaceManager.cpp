#include "pch.h"
#include "BroomRaceManager.h"

#include "GameInstance.h"
#include "RaceRing.h"
#include "Layer.h"
#include "BroomRacerAI.h"
#include "Player.h"
#include "Broom.h"
#include "InfoInstance.h"

CBroomRaceManager::CBroomRaceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBroomRaceManager::CBroomRaceManager(const CBroomRaceManager& Prototype)
	: CGameObject(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
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

	m_pInfoInstance->Add_Event(TEXT("BROOMRACENPCINTERACT"), [this](void* p) {this->RaceReady(); });

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
	case ENUM_CLASS(RACE_STATE::FINISH):
		break;
	case ENUM_CLASS(RACE_STATE::END):
	{
		for (auto& racer : m_Racers) 
    {
			if (racer.pAI) 
      {
				racer.pAI->Get_Broom()->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));
			}
			if (racer.pRacer) 
      {
				if (m_pGameInstance->Key_Up(DIK_ESCAPE)) 
        {
					m_bRaceEnd = true;
				}
			}
		}
	}
		break;
	}

}

void CBroomRaceManager::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_bRaceReady == true)
	{
		m_fRedayTime += fTimeDelta;
	}

	if (m_fRedayTime >= 2.f)
	{
		m_bRaceReady = false;
		m_fRedayTime = 0.f;
		m_pInfoInstance->Event_CallBack(TEXT("Ready_Race"));
		m_eRaceState = ENUM_CLASS(RACE_STATE::COUNTDOWN);
	}

	if (m_eRaceState == ENUM_CLASS(RACE_STATE::RACING))
	{
		if (m_iLastRing < m_pRaceRings.size())
		{
			m_fRacingTimer += fTimeDelta;
			m_pInfoInstance->Set_Broom_Timer(m_fRacingTimer);
		}
		else
		{
			m_fRacingTimer = 0.f;
			if (m_bRaceStart == true)
			{
				m_pInfoInstance->Event_CallBack(TEXT("RaceEnd"));
				m_iCount = 3;
				m_bRaceStart = false;
				m_eRaceState = ENUM_CLASS(RACE_STATE::END);
			}
		}
	}

	if (m_bRaceEnd) {
		m_fDelay += fTimeDelta;
		for (auto& racer : m_Racers) {
			if (racer.pRacer) {
				if (m_fDelay >= 1.f) {
					racer.pRacer->Get_Broom()->Set_Ride(false);

					racer.pRacer->Get_Component<CFSM>()->Change_State(FSMSTATE::IDLE);

					CTransform* pTransform = racer.pRacer->Get_Component<CTransform>();
					CCharacter_Controller* pCharacter = racer.pRacer->Get_Component<CCharacter_Controller>();
					pCharacter->Set_Position(XMLoadFloat4(&m_OriginPos));
					pTransform->Set_State(STATE::POSITION, XMLoadFloat4(&m_OriginPos));
					m_bRaceEnd = false;
					m_fDelay = 0.f;
				}
			}
		}
	}



#ifdef _DEBUG
	//Describe_Entity();
#endif // _DEBUG
}

void CBroomRaceManager::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CBroomRaceManager::Render()
{
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

	//if (GUI::Button("Race Start"))
	//{
	m_eRaceState = ENUM_CLASS(RACE_STATE::READY);
	const _float SPAWN_DISTANCE = 80.f;

	if (FAILED(Load_RaceRing()))
	{
		MSG_BOX("Failed Load RaceRing");
	}
	CBroomRacerAI::RacerDesc Desc = {};
	for (_uint i = 1; i < 4; i++)
	{
		Desc.pRacerManager = this;
		Desc.iIndex = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroomRacerAI>(g_iStaticLevel, NEXT_LEVEL, LAYER_RACERAI, &Desc)))
			return;
	}
	m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Set_RaceInfo();

	for (auto& racer : m_Racers)
	{
		CTransform* pRingTransform =
			m_pRaceRings[0]->Get_Component<CTransform>();

		//_vector ringPos = pRingTransform->Get_State(STATE::POSITION);
		//_vector ringLook = pRingTransform->Get_State(STATE::RIGHT);
		//ringLook = XMVector3Normalize(ringLook);

		//_vector spawnPos = ringPos - ringLook * SPAWN_DISTANCE;


		//if (racer.pAI)
		//{
		//	CTransform* pTransform =
		//		racer.pAI->Get_Component<CTransform>();
		//	_float fRand = m_pGameInstance->Real_Random_Float(-10.f, 10.f);
		//	spawnPos.m128_f32[0] += fRand;

		//	pTransform->Set_State(STATE::POSITION, spawnPos);
		//	pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));

		//	racer.pAI->Get_Broom()->Set_Move(false);
		//}
		//else if (racer.pRacer)
		//{
		//	CCharacter_Controller* pCharacter = racer.pRacer->Get_Component<CCharacter_Controller>();
		//	pCharacter->Set_Position(spawnPos);
		//	CTransform* pTransform = racer.pRacer->Get_Component<CTransform>();

		//	//				pTransform->Set_State(STATE::POSITION, spawnPos);
		//	pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
		//	racer.pRacer->Get_Component<CFSM>()->Change_State(FSMSTATE::BROOM_RIDE);
		//	racer.pRacer->Get_Broom()->Set_Move(false);
		//}

		//else if (racer.pRacer)
		//{
		//	CTransform* pTransform =
		//		racer.pRacer->Get_Component<CTransform>();
		//	CCharacter_Controller* pCharacter = racer.pRacer->Get_Component<CCharacter_Controller>();
		//	pCharacter->Set_Position(spawnPos);
		//	pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
		//	racer.pRacer->Get_Component<CFSM>()->Change_State(FSMSTATE::BROOM_RIDE);
		//	racer.pRacer->Get_Broom()->Set_Move(false);
		//}
	////if (GUI::Button("Race Start"))
	////{
	//m_eRaceState = ENUM_CLASS(RACE_STATE::READY);
	//const _float SPAWN_DISTANCE = 80.f;

	//if (FAILED(Load_RaceRing()))
	//{
	//	MSG_BOX("Failed Load RaceRing");
	//}
	//CBroomRacerAI::RacerDesc Desc = {};
	//for (_uint i = 1; i < 4; i++)
	//{
	//	Desc.pRacerManager = this;
	//	Desc.iIndex = i;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroomRacerAI>(g_iStaticLevel, NEXT_LEVEL, LAYER_RACERAI, &Desc)))
	//		return;
	//}
	//m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Set_RaceInfo();

		//m_bRaceReady = true;
		//}
		//if (m_eRaceState == ENUM_CLASS(RACE_STATE::READY))
		//{
		//	/*if (GUI::Button("Countdown"))
		//	{*/
		//	m_pInfoInstance->Event_CallBack(TEXT("Ready_Race"));
		//	m_eRaceState = ENUM_CLASS(RACE_STATE::COUNTDOWN);
		//	//}
		//}
	}
	//for (auto& racer : m_Racers)
	//{
	//	CTransform* pRingTransform =
	//		m_pRaceRings[0]->Get_Component<CTransform>();

	//	_vector ringPos = pRingTransform->Get_State(STATE::POSITION);
	//	_vector ringLook = pRingTransform->Get_State(STATE::RIGHT);
	//	ringLook = XMVector3Normalize(ringLook);

	//	_vector spawnPos = ringPos - ringLook * SPAWN_DISTANCE;


	//	if (racer.pAI)
	//	{
	//		CTransform* pTransform =
	//			racer.pAI->Get_Component<CTransform>();
	//		_float fRand = m_pGameInstance->Real_Random_Float(-10.f, 10.f);
	//		spawnPos.m128_f32[0] += fRand;

	//		pTransform->Set_State(STATE::POSITION, spawnPos);
	//		pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));

	//		racer.pAI->Get_Broom()->Set_Move(false);
	//	}
	//	else if (racer.pRacer)
	//	{
	//		CCharacter_Controller* pCharacter = racer.pRacer->Get_Component<CCharacter_Controller>();
	//		pCharacter->Set_Position(spawnPos);
	//		CTransform* pTransform = racer.pRacer->Get_Component<CTransform>();

	//		//				pTransform->Set_State(STATE::POSITION, spawnPos);
	//		pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
	//		racer.pRacer->Get_Component<CFSM>()->Change_State(FSMSTATE::BROOM_RIDE);
	//		racer.pRacer->Get_Broom()->Set_Move(false);
	//	}

	//	else if (racer.pRacer)
	//	{
	//		CTransform* pTransform =
	//			racer.pRacer->Get_Component<CTransform>();
	//		CCharacter_Controller* pCharacter = racer.pRacer->Get_Component<CCharacter_Controller>();
	//		pCharacter->Set_Position(spawnPos);
	//		pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
	//		racer.pRacer->Get_Component<CFSM>()->Change_State(FSMSTATE::BROOM_RIDE);
	//		racer.pRacer->Get_Broom()->Set_Move(false);
	//	}


	//	m_bRaceReady = true;
	//	//}
	//	//if (m_eRaceState == ENUM_CLASS(RACE_STATE::READY))
	//	//{
	//	//	/*if (GUI::Button("Countdown"))
	//	//	{*/
	//	//	m_pInfoInstance->Event_CallBack(TEXT("Ready_Race"));
	//	//	m_eRaceState = ENUM_CLASS(RACE_STATE::COUNTDOWN);
	//	//	//}
	//	//}
	//}
}

#endif // _DEBUG

void CBroomRaceManager::Update_Countdown(_float fTimeDelta)
{
	m_fCountTimer += fTimeDelta;
	if (m_fCountTimer >= 1.f)
	{
		m_fCountTimer = 0.f;
		--m_iCount;

		if (m_iPerCount != m_iCount)
		{
			m_iPerCount = m_iCount;
			m_pInfoInstance->Event_CallBack(TEXT("Race_Count"), &m_iCount);
		}
		if (m_iCount <= 0)
		{
			size_t MaxRing = m_pRaceRings.size();
			m_pInfoInstance->Event_CallBack(TEXT("Race_Start"), &MaxRing);
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
	m_bRaceStart = true;
}



void CBroomRaceManager::Check_RingPassed()
{
	const float PASS_RADIUS = 5.f;

	for (auto& racer : m_Racers)
	{
		if (racer.curRing >= (_uint)m_pRaceRings.size())
			continue;
		CRaceRing* pRing = m_pRaceRings[racer.curRing];

		_vector ringPos = pRing->Get_WorldPostion();

		_vector ringFwd =
			pRing->Get_Component<CTransform>()->Get_State(STATE::RIGHT);
		ringFwd = XMVector3Normalize(ringFwd);

		_vector currPos = racer.pAI ? racer.pAI->Get_WorldPostion() : racer.pRacer->Get_WorldPostion();

		_vector prevPos = XMLoadFloat4(&racer.prevPos);

		_float Prev = XMVectorGetX(XMVector3Dot(prevPos - ringPos, ringFwd));
		_float Curr = XMVectorGetX(XMVector3Dot(currPos - ringPos, ringFwd));
#ifdef _DEBUG
		GUI::Text("%s", racer.pAI ? "AI" : "Player");
		GUI::SameLine();
		GUI::Text("CurRing %d", racer.curRing);
		GUI::Text("PrevSide %.2f", Prev);
		GUI::Text("CurrSide %.2f", Curr);
#endif // _DEBUG
		if (Prev * Curr <= 0.f && fabs(Prev - Curr) > 1e-4f)
		{
			_float t = Prev / (Prev - Curr);
			t = clamp(t, 0.f, 1.f);

			_vector hitPos = prevPos + (currPos - prevPos) * t;

			_vector v = hitPos - ringPos;
			_float side = XMVectorGetX(XMVector3Dot(v, ringFwd));
			_vector proj = hitPos - ringFwd * side;

			_float dist = XMVectorGetX(XMVector3Length(proj - ringPos));

			if (dist <= PASS_RADIUS)
			{
				racer.curRing++;

				if (racer.curRing >= m_pRaceRings.size())
				{
					racer.curRing = (_uint)m_pRaceRings.size();

					if (racer.pAI) {
						racer.pAI->Get_Broom()->Set_Hover(true);
						racer.pAI->Get_Broom()->Set_Move(false);
					}
					else if (racer.pRacer) {
						m_iLastRing++;
						racer.pRacer->Get_Broom()->Set_Hover(true);
						racer.pRacer->Get_Broom()->Set_Move(false);
					}

					XMStoreFloat4(&racer.prevPos, currPos);
					continue;
				}

				racer.pAI ? SetTargetRing(racer.pAI) : SetTargetRing(racer.pRacer);
			}
		}

		XMStoreFloat4(&racer.prevPos, currPos);
	}
}

void CBroomRaceManager::Finish()
{
}

void CBroomRaceManager::RaceReady()
{
	//if (GUI::Button("Race Start"))
//{
	m_eRaceState = ENUM_CLASS(RACE_STATE::READY);
	const _float SPAWN_DISTANCE = 30.f;

	if (FAILED(Load_RaceRing()))
	{
		MSG_BOX("Failed Load RaceRing");
	}
	CBroomRacerAI::RacerDesc Desc = {};
	for (_uint i = 1; i < 4; i++)
	{
		Desc.pRacerManager = this;
		Desc.iIndex = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroomRacerAI>(g_iStaticLevel, NEXT_LEVEL, LAYER_RACERAI, &Desc)))
			return;
	}
	m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Set_RaceInfo();

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
			CTransform* pTransform =
				racer.pAI->Get_Component<CTransform>();
			_float fRand = m_pGameInstance->Real_Random_Float(-10.f, 10.f);
			spawnPos.m128_f32[2] += fRand;

			pTransform->Set_State(STATE::POSITION, spawnPos);
			pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));

			racer.pAI->Get_Broom()->Set_Move(false);
		}
		else if (racer.pRacer)
		{
			CTransform* pTransform =
				racer.pRacer->Get_Component<CTransform>();
			CCharacter_Controller* pCharacter = racer.pRacer->Get_Component<CCharacter_Controller>();
			XMStoreFloat4(&m_OriginPos, racer.pRacer->Get_WorldPostion());
			pCharacter->Set_Position(spawnPos);
			pTransform->Set_State(STATE::POSITION, spawnPos);
			pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
			racer.pRacer->Get_Component<CFSM>()->Change_State(FSMSTATE::BROOM_RIDE);
			racer.pRacer->Get_Broom()->Set_Move(false);

		}
	}

	m_bRaceReady = true;
	//}
	//if (m_eRaceState == ENUM_CLASS(RACE_STATE::READY))
	//{
	//	/*if (GUI::Button("Countdown"))
	//	{*/
	//	m_pInfoInstance->Event_CallBack(TEXT("Ready_Race"));
	//	m_eRaceState = ENUM_CLASS(RACE_STATE::COUNTDOWN);
	//	//}
	//}
}

void CBroomRaceManager::SetTargetRing(CGameObject* pRacer)
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
				m_pInfoInstance->Event_CallBack(TEXT("CurrentRing"));
				m_iLastRing++;

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

	if (Info.pAI)
		info.pAI->Set_RaceRing(m_pRaceRings[info.curRing]);
	else if (Info.pRacer)
		info.pRacer->Set_RaceRing(m_pRaceRings[info.curRing]);

	m_Racers.push_back(info);
	SAFE_ADDREF(Info.pRacer);
}

void CBroomRaceManager::Push_RaceRing(CRaceRing* Ring)
{
	m_pRaceRings.push_back(Ring);
	SAFE_ADDREF(Ring);
}

HRESULT CBroomRaceManager::Load_RaceRing()
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/RaceRing/RaceRing_Data.xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("Ring");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	for (auto* Object = root->FirstChildElement("Object"); Object; Object = Object->NextSiblingElement("Object"))
	{
		CRaceRing::RACERING_DESC Desc = {};

		Desc.pBroomRaceManager = this;

		/* Transform */
		auto* Rotation = Object->FirstChildElement("Scale");
		Rotation->QueryFloatAttribute("x", &Desc.vScale.x);
		Rotation->QueryFloatAttribute("y", &Desc.vScale.y);
		Rotation->QueryFloatAttribute("z", &Desc.vScale.z);

		auto* Scale = Object->FirstChildElement("Rotation");
		Scale->QueryFloatAttribute("x", &Desc.vRotation.x);
		Scale->QueryFloatAttribute("y", &Desc.vRotation.y);
		Scale->QueryFloatAttribute("z", &Desc.vRotation.z);

		auto* Position = Object->FirstChildElement("Position");
		Position->QueryFloatAttribute("x", &Desc.vPosition.x);
		Position->QueryFloatAttribute("y", &Desc.vPosition.y);
		Position->QueryFloatAttribute("z", &Desc.vPosition.z);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRaceRing>(g_iStaticLevel, NEXT_LEVEL, LAYER_RING, &Desc)))
			return E_FAIL;
	}


	return S_OK;
}

