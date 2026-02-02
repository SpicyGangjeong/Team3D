#include "pch.h"
#include "BroomRaceManager.h"

#include "GameInstance.h"
#include "RaceRing.h"
#include "MapElement_Balloon.h"
#include "Layer.h"
#include "BroomRacerAI.h"
#include "Player.h"
#include "Broom.h"
#include "InfoInstance.h"
#include "InstancedProp.h"
#include "EffectPool.h"
#include "EffectParts.h"
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

	for (auto& RaceEnd : m_pRaceEndEffects)
	{
		RaceEnd->Priority_Update(fTimeDelta);
	}
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
		if (m_bCurrentRace == true)
		{
			if (m_pGameInstance->Key_Down(DIK_ESCAPE))
			{
				m_bRaceEnd = true;
				_float Alpha = 2.f;
				m_pInfoInstance->Event_CallBack(TEXT("UIFadeIn"), &Alpha);
				for (auto& racer : m_Racers)
				{
					if (racer.pAI)
					{
						racer.pAI->Get_Broom()->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));
					}
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

	for (auto& RaceEnd : m_pRaceEndEffects)
	{
		RaceEnd->Update(fTimeDelta);
	}

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
				CTransform* pRingTransform = m_pRaceRings[m_pRaceRings.size() - 1]->Get_Component<CTransform>();
				for (auto& RaceEnd : m_pRaceEndEffects)
				{
					RaceEnd->Set_Visible(true);

					_vector LastRingPos = pRingTransform->Get_State(STATE::POSITION);
					_vector ringRIght = pRingTransform->Get_State(STATE::RIGHT);
					ringRIght = XMVector3Normalize(ringRIght);

					_vector spawnPos = LastRingPos - ringRIght * 27.f;

					RaceEnd->Get_Component<CTransform>()->Set_WorldMatrix(XMMatrixTranslationFromVector(spawnPos));

				}	
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::FX_FIREWORKS, SD_CHANNEL_GROUP::EFFECT, false, 0.7f);
				m_pInfoInstance->Event_CallBack(TEXT("RaceEnd"));
				m_iCount = 3;
				m_bRaceStart = false;
				m_eRaceState = ENUM_CLASS(RACE_STATE::END);



			}
		}
	}

	if (m_bRaceEnd)
	{
		m_fDelay += fTimeDelta;
		for (auto& racer : m_Racers) {
			if (racer.pRacer) {
				if (m_fDelay >= 1.f) {
					racer.pRacer->Get_Broom()->Set_Ride(false);
					racer.pRacer->Get_Broom()->Set_Move(true);

					CTransform* pTransform = racer.pRacer->Get_Component<CTransform>();
					CCharacter_Controller* pCharacter = racer.pRacer->Get_Component<CCharacter_Controller>();
					pTransform->Set_State(STATE::POSITION, XMLoadFloat4(&m_OriginPos));
					pCharacter->Set_Position(XMLoadFloat4(&m_OriginPos));
					m_bRaceEnd = false;
					m_pInfoInstance->Event_CallBack(TEXT("RACEREADY"), &m_bRaceEnd);
					m_fDelay = 0.f;
					m_bCurrentRace = false;
					m_pGameInstance->Sound_Stop(SOUND::SD_KIND::BROOM_BOOST, SD_CHANNEL_GROUP::EFFECT);
					m_pGameInstance->Sound_Stop(SOUND::SD_KIND::BROOM_NORMAL, SD_CHANNEL_GROUP::EFFECT);
					m_pInfoInstance->Event_CallBack(TEXT("BroomRide"), &m_bCurrentRace);
					m_pRaceRings[m_pRaceRings.size() - 1]->Set_Target(false);

					m_pGameInstance->Sound_StopChannel(SD_CHANNEL_GROUP::BGM);
					m_pGameInstance->Sound_Play(SOUND::SD_KIND::BGM_GamePlay, SD_CHANNEL_GROUP::BGM, true, 0.2f);
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
	for (auto& RaceEnd : m_pRaceEndEffects)
	{
		RaceEnd->Late_Update(fTimeDelta);
	}
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


	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;


	{
		_string BasePath = "../Bin/Resources/Data/Effect/BroomRace/";

		array<_string, 3> Colors =
		{
			"Firewark_Blue",
			"Firewark_Red",
			"Firewark_Yellow"
		};

		for (_uint i = 0; i < m_pRaceEndEffects.size(); ++i)
		{
			_string EffectFinal = BasePath + Colors[i];

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEffectParts>(
				g_iStaticLevel, NEXT_LEVEL, LAYER_EFFECT, &PartsDesc, this, &m_pRaceEndEffects[i])))
			{
				return E_FAIL;
			}

			SAFE_ADDREF(m_pRaceEndEffects[i]);
			m_pRaceEndEffects[i]->Load(EffectFinal.c_str(), static_cast<LEVEL>(g_iStaticLevel));
			m_pRaceEndEffects[i]->Set_Visible(false);
		}
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

	for (auto& RaceEndEffects : m_pRaceEndEffects)
	{
		SAFE_RELEASE(RaceEndEffects);
	}
}
#ifdef _DEBUG

void CBroomRaceManager::Describe_Entity()
{

}

#endif // _DEBUG

void CBroomRaceManager::Update_Countdown(_float fTimeDelta)
{
	m_fCountTimer += fTimeDelta;
	if (!m_bCountDown) {
		m_pGameInstance->Sound_Play(SOUND::SD_KIND::FX_COUNTDOWN, SD_CHANNEL_GROUP::EFFECT, false, 0.8f);
		m_bCountDown = true;
	}

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
	m_bCurrentRace = true;
	m_eRaceState = ENUM_CLASS(RACE_STATE::READY);
	const _float SPAWN_DISTANCE = 30.f;

	if (FAILED(Load_RaceRing()))
	{
		MSG_BOX("Failed Load RaceRing");
	}

	if (FAILED(Load_Balloons()))
	{
		MSG_BOX("Failed Load Balloons");
	}

	if (FAILED(Load_Broomrace_Effect()))
	{
		MSG_BOX("Failed Load Broomrace_Effect");
	}

	CInstancedProp::INSTANCE_PROP_DESC Instance_Desc = {};

	/* InstanceProp RouteMarker*/
	Instance_Desc.isShake = false;
	Instance_Desc.eShaderPass = SHADER_PASS_WORLDMODLE_INSTANCE::NONCULL;
	Instance_Desc.bEnableRigidbody = false;
	Instance_Desc.vRadius = _float2(0.f, 0.f);
	Instance_Desc.vSpeed = _float2(0.f, 0.f);
	Instance_Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SK_BRR_RouteMarker";
	Instance_Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/RouteMarker.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Instance_Desc))) {
		return;
	}

	CLayer* pLayer = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_RACERAI);

	if (nullptr != pLayer)
	{
		for (auto& pUnified : *pLayer->Get_Objects())
		{
			dynamic_cast<CBroomRacerAI*>(pUnified)->Set_RaceInfo();
		}
	}

	m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Set_RaceInfo();

	for (size_t i = 0; i < (_uint)m_Racers.size(); i++)
	{
		auto& racer = m_Racers[i];
	
		CTransform* pRingTransform =
			m_pRaceRings[0]->Get_Component<CTransform>();

		_vector ringPos = pRingTransform->Get_State(STATE::POSITION);
		_vector ringLook = pRingTransform->Get_State(STATE::RIGHT);
		ringLook = XMVector3Normalize(ringLook);

		_vector spawnPos = ringPos - ringLook * SPAWN_DISTANCE;

		if (racer.pAI)
		{
			CTransform* pTransform =
				racer.pAI->Get_Broom()->Get_Component<CTransform>();  

			_float lane = (_float)i - (_float)(m_Racers.size() - 1) * 0.5f;
			_float side = lane * 3.0f;

			side += m_pGameInstance->Real_Random_Float(-0.5f, 0.5f);


			_vector offset = XMVectorSet(0.f, 1.3f, 0.f, 0.f) + pTransform->Get_State(STATE::LOOK) * side;

			spawnPos += offset;

			pTransform->Set_State(STATE::POSITION, spawnPos);
			pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
			racer.pAI->Get_Broom()->Set_Move(false);
		}
		else if (racer.pRacer)
		{
			CTransform* pTransform = racer.pRacer->Get_Component<CTransform>();
			CCharacter_Controller* pCharacter = racer.pRacer->Get_Component<CCharacter_Controller>();
			XMStoreFloat4(&m_OriginPos, XMVectorSet(-552.794f, -27.676f, -396.840f, 1.f));
			pCharacter->Set_Position(spawnPos);
			pTransform->Set_State(STATE::POSITION, spawnPos);
			pTransform->LookAt(pRingTransform->Get_State(STATE::POSITION));
			racer.pRacer->Get_Broom()->Set_Ride(true);
			racer.pRacer->Get_Broom()->Set_Move(false);
			m_pRaceRings[racer.curRing]->Set_Target(true);;

		}
	}

	m_bRaceReady = true;

	m_pGameInstance->Sound_StopChannel(SD_CHANNEL_GROUP::BGM);
	m_pGameInstance->Sound_Play(SOUND::SD_KIND::BGM_RACE, SD_CHANNEL_GROUP::BGM, true, 0.8f);

	m_bCurrentRace = true;
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
				_int iRand = m_pGameInstance->Real_Random_Int(0, 1);
				if (iRand == 0)
				{
					m_pGameInstance->Sound_Play(SOUND::SD_KIND::FX_SWOOSH0, SD_CHANNEL_GROUP::EFFECT, false, 3.f);
				}
				else {
					m_pGameInstance->Sound_Play(SOUND::SD_KIND::FX_SWOOSH1, SD_CHANNEL_GROUP::EFFECT, false, 3.f);
				}

				m_pRaceRings[racer.curRing-1]->Set_Target(false);
					
				racer.pRacer->Set_RaceRing(m_pRaceRings[racer.curRing]);
				m_pInfoInstance->Event_CallBack(TEXT("CurrentRing"));
				m_iLastRing++;
				
				m_pRaceRings[racer.curRing]->Set_Target(true);
				

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

HRESULT CBroomRaceManager::Load_Balloons()
{
	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/Balloon/Ballon_Data.xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("Balloon");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	for (auto* Object = root->FirstChildElement("Object"); Object; Object = Object->NextSiblingElement("Object"))
	{
		CMapElement_Balloon::BALLOON_DESC Desc = {};

		auto* Value = Object->FirstChildElement("Value");
		Value->QueryBoolAttribute("isFloating", &Desc.isFloating);
		Value->QueryUnsignedAttribute("DiffuseIndex", &Desc.iDiffuseIndex);

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

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Balloon>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroomRaceManager::Load_Broomrace_Effect()
{
	CEffectPool* pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();

	if (nullptr == pEffectPool)
		return E_FAIL;

	tinyxml2::XMLDocument xmlDoc;

	string strPath = "../Bin/Resources/Data/Map/EffectPart/Bubble_Data.xml";

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(strPath.c_str())))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("EffectParts");

	if (nullptr == root)
	{
		MSG_BOX("Failed to Find root");
		return S_OK;
	}

	for (auto* Object = root->FirstChildElement("Object"); Object; Object = Object->NextSiblingElement("Object"))
	{
		_float4 vPosition = {};

		/* Transform */
		auto* Position = Object->FirstChildElement("Position");
		Position->QueryFloatAttribute("x", &vPosition.x);
		Position->QueryFloatAttribute("y", &vPosition.y);
		Position->QueryFloatAttribute("z", &vPosition.z);
		vPosition.w = 1.f;

		pEffectPool->Use_Skill(SKILL_TYPE::BROOM_RACE_BUBBLE, this, &vPosition);
	}


	

	return S_OK;
}

