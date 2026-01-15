#include "pch.h"
#include "CutSceneInfo.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Monster.h"
#include "TriggerBox.h"
#include "Interaction_Key.h"
#include "Layer.h"
#include "Player.h"
#include "Troll.h"
#include "TimeSocket.h"
#include "Camera_Cinematic.h"
#include "Ranrok.h"
#include "ThestralCarriage.h"

CCutSceneInfo::CCutSceneInfo()
{
}
void CCutSceneInfo::Update(_float fTimeDelta)
{
	m_bIsActiveCutScene = false;
	if (m_pGameInstance->Key_Pressing(DIK_LBRACKET) && m_pGameInstance->Key_Pressing(DIK_RBRACKET)) {
		Set_AllActiveEventsExit();
	}
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
	Update_WaitEvents(fTimeDelta);
	Update_ActiveEvents(fTimeDelta);
}

void CCutSceneInfo::Load_CutScenes()
{
	LEVEL eLevel = (LEVEL)m_pGameInstance->Get_NextLevelID();
	Clear_AllEvents();
	switch (eLevel)
	{
	case LEVEL::GAMEPLAY:
		Clear_AllEvents();
		Ready_GameplayCutScenes();
		break;
	case LEVEL::FIELD:
		Clear_AllEvents();
		Ready_FieldCutScenes();
		break;
	default:
		Clear_ActiveEvents();
		break;
	}
}

void CCutSceneInfo::Active_Event(_string& strKey)
{
	map<_string, TimeLine*>::iterator iter = m_funcWaitEvents.find(strKey);
	if (iter != m_funcWaitEvents.end()) {
		Update_Start_Event(iter->first.c_str());
		SAFE_RELEASE(iter->second->m_pTriggerBox);
		m_funcActiveEvents.emplace(iter->first, iter->second);
		m_funcWaitEvents.erase(iter);
	}
}

HRESULT CCutSceneInfo::DeActive_ActiveEvent(_string& strKey)
{
	map<_string, TimeLine*>::iterator iter = m_funcActiveEvents.find(strKey);
	if (iter != m_funcActiveEvents.end()) {

		for (list<CTimeSocket*>::iterator socketIter = (*iter).second->m_Sockets.begin(); 
			socketIter != (*iter).second->m_Sockets.end(); ++socketIter) {
			SAFE_RELEASE((*socketIter));
		}
		SAFE_RELEASE(iter->second->m_pTriggerBox);
		Safe_Delete((*iter).second);
		m_funcActiveEvents.erase(iter);
	}
	return S_OK;
}

void CCutSceneInfo::Load_Events(pair<_string, TimeLine*>& pairTimeLine)
{
	m_funcWaitEvents.emplace(pairTimeLine);
}

void CCutSceneInfo::Update_ActiveEvents(_float fTimeDelta)
{
	_float fRatio = 0.f;
	map<_string, TimeLine*>::iterator iter = m_funcActiveEvents.begin();
	if (iter != m_funcActiveEvents.end()) {
		m_bIsActiveCutScene = true;
	}
	for (; iter != m_funcActiveEvents.end();) {
		list<CTimeSocket*>* pSockets = &iter->second->m_Sockets;
		iter->second->m_vTimer.x += fTimeDelta;
		fRatio = CMyTools::Saturate(iter->second->m_vTimer.x / iter->second->m_vTimer.y);
		for (list<CTimeSocket*>::iterator socketIter = pSockets->begin(); socketIter != pSockets->end();) {
#ifdef _DEBUG
			GUI::Begin("CutScene");
			if (GUI::TreeNodeEx((*socketIter)->m_Contents.strEventName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen)) {
				GUI::Text("fRatio : %.2f", fRatio);
				GUI::Text("fSecond : %.2f / %.2f", iter->second->m_vTimer.x, iter->second->m_vTimer.y);
				GUI::TreePop();
			}
			GUI::End();
#endif // _DEBUG

			if ((*socketIter)->Trigger(fRatio))
			{
				SAFE_RELEASE((*socketIter));
				socketIter = pSockets->erase(socketIter);
			}
			else
			{
				break;
			}
		}
		if (pSockets->empty()) {
			Update_End_Event((iter->first).c_str());
			SAFE_RELEASE(iter->second->m_pTriggerBox);
			Safe_Delete((*iter).second);
			iter = m_funcActiveEvents.erase(iter);
		}
		else {
			iter++;
		}
	}

	iter = m_funcActiveEvents.begin();
	if (iter != m_funcActiveEvents.end()) {
		m_bIsActiveCutScene = true;
	}
}

void CCutSceneInfo::Update_WaitEvents(_float fTimeDelta)
{
	for (map<_string, TimeLine*>::iterator iter = m_funcWaitEvents.begin(); iter != m_funcWaitEvents.end();) {
		if (nullptr != iter->second->m_pTriggerBox) {
			if (SUCCEEDED(iter->second->m_pTriggerBox->TryScanArea(fTimeDelta))) {
				Update_Start_Event((iter->first).c_str());
				SAFE_RELEASE(iter->second->m_pTriggerBox);
				m_funcActiveEvents.insert(*iter);
				iter = m_funcWaitEvents.erase(iter);
			}
			else {
				++iter;
			}
		}
		else {
			++iter;
		}
	}
}

HRESULT CCutSceneInfo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pInfoInstance = CInfoInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContex;

	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pInfoInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);

	if (FAILED(Ready_Events()))
		return E_FAIL;

	return S_OK;
}
HRESULT CCutSceneInfo::Clear_ActiveEvents()
{
	return Clear_Events(m_funcActiveEvents);
}
HRESULT CCutSceneInfo::Clear_Events(map<_string, TimeLine*>& Events)
{
	map<_string, TimeLine*>::iterator iter = Events.begin();
	for (; iter != Events.end();) {
		for (list<CTimeSocket*>::iterator socketIter = (*iter).second->m_Sockets.begin();
			socketIter != (*iter).second->m_Sockets.end(); ++socketIter) {
			SAFE_RELEASE((*socketIter));
		}
		SAFE_RELEASE(iter->second->m_pTriggerBox);
		Safe_Delete((*iter).second);
		iter = Events.erase(iter);
	}
	return S_OK;
}
HRESULT CCutSceneInfo::Clear_AllEvents()
{
	Clear_Events(m_funcActiveEvents);
	Clear_Events(m_funcWaitEvents);

	return S_OK;
}
HRESULT CCutSceneInfo::Ready_GameplayCutScenes()
{
	{
		Load_CutSceneXML("../Bin/Resources/Data/CutScene/CarriageIntro.xml");
		Load_CutSceneXML("../Bin/Resources/Data/CutScene/TrollIntroCutScene.xml");
	}
	return S_OK;
}
HRESULT CCutSceneInfo::Ready_FieldCutScenes()
{
	{
		Load_CutSceneXML("../Bin/Resources/Data/CutScene/RanrokCutScene.xml");
	}
	return S_OK;
}

HRESULT CCutSceneInfo::Ready_Events()
{
#pragma region START
	m_CutScene_StartEvents.emplace("RanrokIntro",
		[this]() {
			m_pGameInstance->Sound_StopChannel(SD_CHANNEL_GROUP::BGM);
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::BGM_RANROK_0, SD_CHANNEL_GROUP::BGM, true, 0.5f);
			m_pGameInstance->Get_Layer(g_iStaticLevel, LAYER_UI)->Get_Object<CInteraction_Key>()->Set_Active(false);
		});

	m_CutScene_StartEvents.emplace("TrollIntro",
		[this]() {
			m_pGameInstance->Sound_StopChannel(SD_CHANNEL_GROUP::BGM);
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::BGM_Battle_0, SD_CHANNEL_GROUP::BGM, true, 0.5f);
			m_pGameInstance->Get_Layer(g_iStaticLevel, LAYER_UI)->Get_Object<CInteraction_Key>()->Set_Active(false);
		});

#pragma endregion

#pragma region END
	m_CutScene_EndEvents.emplace("TrollIntro",
		[this]() {
			m_pInfoInstance->Load_ReparoObjects("Reparo_Data");
			m_pGameInstance->Get_Layer(g_iStaticLevel, LAYER_UI)->Get_Object<CInteraction_Key>()->Set_Active(true);
		});

	m_CutScene_EndEvents.emplace("RanrokIntro",
		[this]() {
			m_pGameInstance->Sound_StopChannel(SD_CHANNEL_GROUP::BGM);
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::BGM_Battle_1, SD_CHANNEL_GROUP::BGM, true, 0.5f);
			m_pGameInstance->Get_Layer(g_iStaticLevel, LAYER_UI)->Get_Object<CInteraction_Key>()->Set_Active(true);
		});

	m_CutScene_EndEvents.emplace("CarriageIntro",
		[this]() {
			m_pGameInstance->Sound_StopChannel(SD_CHANNEL_GROUP::BGM);
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::BGM_Land_DAY, SD_CHANNEL_GROUP::BGM, true, 0.5f);
			m_pGameInstance->Get_Layer(g_iStaticLevel, LAYER_UI)->Get_Object<CInteraction_Key>()->Set_Active(true);
		});
#pragma endregion


	return S_OK;
}

CCutSceneInfo* CCutSceneInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	CCutSceneInfo* pInstance = new CCutSceneInfo();

	if (FAILED(pInstance->Initialize(pDevice, pContex))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}
void CCutSceneInfo::Free()
{
	__super::Free();

	Clear_AllEvents();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
#ifdef _DEBUG
void CCutSceneInfo::Describe_Entity()
{
	GUI::Begin("CutScene");
	if (GUI::Button("ReadyCutScene")) {
		LEVEL eLevel = (LEVEL)m_pGameInstance->Get_NextLevelID();
		Clear_AllEvents();
		switch (eLevel)
		{
		case LEVEL::GAMEPLAY:
		{
			Ready_GameplayCutScenes();
		}
			break;
		case LEVEL::FIELD:
		{
			Ready_FieldCutScenes();
		}
			break;
		default:
			break;
		}
	}
	if (GUI::Button("DeActiveCutScene")) {
		Clear_ActiveEvents();
	}
	GUI::End();
}
#endif // _DEBUG


static _bool IsTrueText(const char* text)
{
	if (!text) { return false; }
	return (strcmp(text, "true") == 0) || (strcmp(text, "1") == 0);
}

static _bool TryReadFloatAttr(tinyxml2::XMLElement* element, const _char* attrName, _float& outValue)
{
	if (!element || !attrName) {
		return false;
	}

	const _char* text = element->Attribute(attrName);
	if (!text || text[0] == '\0') {
		return false;
	}

	return (element->QueryFloatAttribute(attrName, &outValue) == tinyxml2::XML_SUCCESS);
}

static _bool  TryReadUIntAttr(tinyxml2::XMLElement* element, const _char* attrName, _uint& outValue)
{
	if (!element || !attrName) return false;

	const _char* text = element->Attribute(attrName);
	if (!text || text[0] == '\0') {
		return false;
	}

	return (element->QueryUnsignedAttribute(attrName, &outValue) == tinyxml2::XML_SUCCESS);
}

static void ReadPxTransform(tinyxml2::XMLElement* transformNode, PSX::PxTransform& outTransform)
{
	outTransform.p = PSX::PxVec3(0.f, 0.f, 0.f);
	outTransform.q = PSX::PxQuat(0.f, 0.f, 0.f, 1.f);

	if (!transformNode) { return; }

	tinyxml2::XMLElement* rotQNode = transformNode->FirstChildElement("RotQ");
	tinyxml2::XMLElement* transNode = transformNode->FirstChildElement("Trans");
	if (!rotQNode || !transNode) {
		return;
	}

	TryReadFloatAttr(rotQNode, "X", outTransform.q.x);
	TryReadFloatAttr(rotQNode, "Y", outTransform.q.y);
	TryReadFloatAttr(rotQNode, "Z", outTransform.q.z);
	TryReadFloatAttr(rotQNode, "W", outTransform.q.w);

	TryReadFloatAttr(transNode, "X", outTransform.p.x);
	TryReadFloatAttr(transNode, "Y", outTransform.p.y);
	TryReadFloatAttr(transNode, "Z", outTransform.p.z);
}

static void ReadFlags16(tinyxml2::XMLElement* flagsNode, _boolean& outFlags)
{
	memset(&outFlags, 0, sizeof(_boolean));

	if (!flagsNode) {
		return;
	}

	for (tinyxml2::XMLElement* flagNode = flagsNode->FirstChildElement("Flag");
		flagNode;
		flagNode = flagNode->NextSiblingElement("Flag"))
	{
		int flagId = -1;
		if (flagNode->QueryIntAttribute("id", &flagId) != tinyxml2::XML_SUCCESS) {
			continue;
		}

		if (flagId < 0 || flagId >= 16) {
			continue;
		}

		outFlags.b[flagId] = IsTrueText(flagNode->GetText());
	}
}

static void ReadUInt4(tinyxml2::XMLElement* param10Node, _uint4& outValue)
{
	outValue = {};

	if (!param10Node) {
		return;
	}
	tinyxml2::XMLElement* uintNode = param10Node->FirstChildElement("uint");
	if (!uintNode) return;

	_uint temp = 0;
	if (TryReadUIntAttr(uintNode, "X", temp)) outValue.x = temp;
	if (TryReadUIntAttr(uintNode, "Y", temp)) outValue.y = temp;
	if (TryReadUIntAttr(uintNode, "Z", temp)) outValue.z = temp;
	if (TryReadUIntAttr(uintNode, "W", temp)) outValue.w = temp;
}

static void ReadFloat4(tinyxml2::XMLElement* param11Node, _float4& outValue)
{
	outValue = {};

	if (!param11Node) {
		return;
	}
	tinyxml2::XMLElement* floatNode = param11Node->FirstChildElement("float");
	if (!floatNode) {
		return;
	}

	TryReadFloatAttr(floatNode, "X", outValue.x);
	TryReadFloatAttr(floatNode, "Y", outValue.y);
	TryReadFloatAttr(floatNode, "Z", outValue.z);
	TryReadFloatAttr(floatNode, "W", outValue.w);
}


void CCutSceneInfo::Load_CutSceneXML(const string& path)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
		return;
	}

	TimeLine* pTimeLine = new TimeLine();
	tinyxml2::XMLElement* pContentsNode = doc.FirstChildElement();
	if (!pContentsNode) {
		return;
	}

	pTimeLine->m_vTimer.y = 0.f;
	pContentsNode->QueryFloatAttribute("fDuration", &pTimeLine->m_vTimer.y);

	pTimeLine->m_Sockets.clear();

	for (tinyxml2::XMLElement* pSocketNode = pContentsNode->FirstChildElement("Socket");
		pSocketNode; pSocketNode = pSocketNode->NextSiblingElement("Socket"))
	{
		SOCKETCONTENTS SocketContents = {};

		_float fStartPosition = 0.f;
		SocketContents.fRatio = 0.f;
		pSocketNode->QueryFloatAttribute("fStartPosition", &fStartPosition);
		SocketContents.fRatio = CMyTools::Saturate(fStartPosition / pTimeLine->m_vTimer.y);

		// Info
		_bool bUseSystem = false;
		tinyxml2::XMLElement* pInfoNode = pSocketNode->FirstChildElement("Info");
		if (pInfoNode != nullptr)
		{
			const _char* pEventNameText = pInfoNode->Attribute("strEventName");
			SocketContents.strEventName = pEventNameText ? pEventNameText : "";

			SocketContents.eTypeParam = TIMESOCKET_PARAM::END;
			const _char* pTypeText = pInfoNode->Attribute("eTypeParam");
			if (nullptr != pTypeText)
			{
				CTimeSocket::TryParse_TimeSocketParam(string_view(pTypeText), SocketContents.eTypeParam);
			}

			SocketContents.eTypeFunc = TIMESOCKET_FUNC::END;
			const _char* pFuncText = pInfoNode->Attribute("eTypeFunc");
			if (nullptr != pFuncText)
			{
				CTimeSocket::TryParse_TimeSocketFunc(string_view(pFuncText), SocketContents.eTypeFunc);
			}

			SocketContents.pEventTarget = nullptr;
			SocketContents.funcEvent = nullptr;
			const _char* pTargetText = pInfoNode->Attribute("pTarget");
			if (nullptr != pTargetText)
			{
				if (strcmp(pTargetText, "CAMERA_CINEMATIC") == 0)
				{
					CCamera* pEventTarget = m_pGameInstance->Get_Camera(NEXT_LEVEL, CAMERA_CINEMATIC);
					SocketContents.pEventTarget = pEventTarget;
					SocketContents.funcEvent = [pEventTarget](CTimeSocket& Socket) { pEventTarget->Trigger(Socket); };
				}
				else if (strcmp(pTargetText, "CAMERA_SHOULDER") == 0)
				{
					CCamera* pEventTarget = m_pGameInstance->Get_Camera(NEXT_LEVEL, CAMERA_SHOULDER);
					SocketContents.pEventTarget = pEventTarget;
					SocketContents.funcEvent = [pEventTarget](CTimeSocket& Socket) { pEventTarget->Trigger(Socket); };
				}
				else if (strcmp(pTargetText, "ACTOR_PLAYER") == 0)
				{
					CPlayer* pEventTarget = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>();
					SocketContents.pEventTarget = pEventTarget;
					SocketContents.funcEvent = [pEventTarget](CTimeSocket& Socket) { pEventTarget->Trigger(Socket); };
				}
				else if (strcmp(pTargetText, "ACTOR_RANROK") == 0)
				{
					CMonster* pEventTarget = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_MONSTER)->Get_Object<CRanrok>();
					SocketContents.pEventTarget = pEventTarget;
					SocketContents.funcEvent = [pEventTarget](CTimeSocket& Socket) { pEventTarget->Trigger(Socket); };
				}
				else if (strcmp(pTargetText, "ACTOR_CARRIAGE") == 0)
				{
					CThestralCarriage* pEventTarget = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_ITEM)->Get_Object<CThestralCarriage>();
					SocketContents.pEventTarget = pEventTarget;
					SocketContents.funcEvent = [pEventTarget](CTimeSocket& Socket) { pEventTarget->Trigger(Socket); };
				}
				else if (strcmp(pTargetText, "ACTOR_TROLL") == 0)
				{
					CTroll* pEventTarget = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_MONSTER)->Get_Object<CTroll>();
					SocketContents.pEventTarget = pEventTarget;
					SocketContents.funcEvent = [pEventTarget](CTimeSocket& Socket) { pEventTarget->Trigger(Socket); };
				}
				else if (strcmp(pTargetText, "SYSTEM") == 0)
				{
					bUseSystem = true;
				}
				assert((nullptr != SocketContents.pEventTarget) || bUseSystem);
				assert((nullptr != SocketContents.funcEvent) || bUseSystem);
			}
		}

		// -------- vFlags / Param_10 / Param_11 / Param_12 ----------
		ReadFlags16(pSocketNode->FirstChildElement("vFlags"), SocketContents.vFlags);
		ReadUInt4(pSocketNode->FirstChildElement("Param_10"), SocketContents.vParam_10);
		ReadFloat4(pSocketNode->FirstChildElement("Param_11"), SocketContents.vParam_11);
		const _char* pStringParma = pSocketNode->FirstChildElement("Param_12")->FirstChildElement("string")->Attribute("tag");
		SocketContents.vParam_12 = pStringParma;

		// -------- Param_0 ----------
		tinyxml2::XMLElement* pParam0Node = pSocketNode->FirstChildElement("Param_0");
		if (nullptr != pParam0Node)
		{
			switch (SocketContents.eTypeParam)
			{
			case TIMESOCKET_PARAM::TARGET:
			{
				tinyxml2::XMLElement* pOtherTargetNode = pParam0Node->FirstChildElement("OtherTarget");
				if (!pOtherTargetNode) break;

				const _char* pLayerNameText = pOtherTargetNode->Attribute("Layer");
				const _char* pTypeIdText = pOtherTargetNode->Attribute("TypeID");

				if (!pLayerNameText || !pTypeIdText) break;
				if (pLayerNameText[0] == '\0' || pTypeIdText[0] == '\0') break;

				SocketContents.wstrLayerName = CMyTools::ToWstring(pLayerNameText);
				SocketContents.wstrKeyName = CMyTools::ToWstring(pTypeIdText);

				if (string_view(pTypeIdText) == "ACTOR_RANROK")
				{
					SocketContents.pOtherTarget = m_pGameInstance->Get_Layer(NEXT_LEVEL, SocketContents.wstrLayerName)->Get_Object<CRanrok>();
				}
				else if (string_view(pTypeIdText) == "ACTOR_PLAYER")
				{
					SocketContents.pOtherTarget = m_pGameInstance->Get_Layer(NEXT_LEVEL, SocketContents.wstrLayerName)->Get_Object<CPlayer>();
				}
				else if (string_view(pTypeIdText) == "ACTOR_TROLL")
				{
					SocketContents.pOtherTarget = m_pGameInstance->Get_Layer(NEXT_LEVEL, SocketContents.wstrLayerName)->Get_Object<CTroll>();
				}
				else if (string_view(pTypeIdText) == "ACTOR_CARRIAGE")
				{
					SocketContents.pOtherTarget = m_pGameInstance->Get_Layer(NEXT_LEVEL, SocketContents.wstrLayerName)->Get_Object<CThestralCarriage>();
				}
				else if (string_view(pTypeIdText) == "CAMERA_CINEMATIC")
				{
					SocketContents.pOtherTarget = m_pGameInstance->Get_Camera(NEXT_LEVEL, CAMERA_CINEMATIC);
				}
				else if (string_view(pTypeIdText) == "CAMERA_SHOULDER")
				{
					SocketContents.pOtherTarget = m_pGameInstance->Get_Camera(NEXT_LEVEL, CAMERA_SHOULDER);
				}
				assert(nullptr != SocketContents.pOtherTarget);
			}break;
			case TIMESOCKET_PARAM::TRANSFORM:
			{
				tinyxml2::XMLElement* pTransformNode = pParam0Node->FirstChildElement("Transform");
				ReadPxTransform(pTransformNode, SocketContents.pxTransform);
				
			}break;
			case TIMESOCKET_PARAM::NOT_USE:
			{
				if (true == bUseSystem) {
					switch (SocketContents.eTypeFunc)
					{
					case TIMESOCKET_FUNC::STOP_CINEMATIC:
						SocketContents.funcEvent = [&](CTimeSocket& Socket) { m_pInfoInstance->DeActive_ActiveEvent(Socket.m_Contents.vParam_12); };
						break;
					case TIMESOCKET_FUNC::RESET_ENVIRONMENT:
						SocketContents.funcEvent = [&](CTimeSocket& Socket) { m_pGameInstance->ResetLevel_Environment();  };
						break;
					case TIMESOCKET_FUNC::SET_VOLUMETRIC:
					{
						tinyxml2::XMLElement* pTransformNode = pParam0Node->FirstChildElement("Transform");
						ReadPxTransform(pTransformNode, SocketContents.pxTransform);
						SocketContents.funcEvent = [&](CTimeSocket& Socket) {
							if (Socket.m_Contents.vFlags.b[0]) {
								m_pGameInstance->Setting_Volumetirc(
									Socket.m_Contents.pxTransform.q.x, Socket.m_Contents.pxTransform.q.y, Socket.m_Contents.pxTransform.q.z, Socket.m_Contents.pxTransform.q.w,
									Socket.m_Contents.pxTransform.p.x);
							}
						};
					}break;
					default:
						break;
					}
					assert(nullptr != SocketContents.funcEvent);
				}
			}break;
			case TIMESOCKET_PARAM::END:
			default:
				break;
			}
		}

		CTimeSocket* pSocket = CTimeSocket::Create(&SocketContents);
		if (!pSocket) {
			continue;
		}
		pTimeLine->m_Sockets.push_back(pSocket);
	}
	tinyxml2::XMLElement* pTriggerNode = pContentsNode->FirstChildElement("Trigger");
	{
		CTriggerBox::TRIGGERBOX_DESC Desc{};
		pTriggerNode->QueryFloatAttribute("W", &Desc.vPosition_Radius.w);
		if (Desc.vPosition_Radius.w > 0) {
			pTriggerNode->QueryFloatAttribute("X", &Desc.vPosition_Radius.x);
			pTriggerNode->QueryFloatAttribute("Y", &Desc.vPosition_Radius.y);
			pTriggerNode->QueryFloatAttribute("Z", &Desc.vPosition_Radius.z);
			pTimeLine->m_pTriggerBox = CTriggerBox::Create(m_pDevice, m_pContext, &Desc);
		}
	}
	
	pTimeLine->m_Sockets.sort([](CTimeSocket* a, CTimeSocket* b) {
		return a->m_Contents.fRatio < b->m_Contents.fRatio;
		});
	pair<_string, TimeLine*> pairResult = { _string(pContentsNode->Name()), pTimeLine };
	m_pInfoInstance->Load_Events(pairResult);
}

void CCutSceneInfo::Set_AllActiveEventsExit()
{
	map<_string, TimeLine*>::iterator iter = m_funcActiveEvents.begin();
	for (; iter != m_funcActiveEvents.end();++iter) {
		iter->second->m_vTimer.x = iter->second->m_vTimer.y - FLT_EPSILON;
		for (list<CTimeSocket*>::iterator socketIter = (*iter).second->m_Sockets.begin();
			socketIter != (*iter).second->m_Sockets.end(); ++socketIter) {
			(*socketIter)->Trigger(1.f - FLT_EPSILON);
		}
	}
}

void CCutSceneInfo::Update_Start_Event(const _char* pEventKey)
{
	auto iter = m_CutScene_StartEvents.find(pEventKey);

	if (iter != m_CutScene_StartEvents.end())
		iter->second();
}

void CCutSceneInfo::Update_End_Event(const _char* pEventKey)
{
	auto iter = m_CutScene_EndEvents.find(pEventKey);

	if (iter != m_CutScene_EndEvents.end())
		iter->second();
}
