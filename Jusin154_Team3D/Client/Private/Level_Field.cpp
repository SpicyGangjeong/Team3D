#include "pch.h"
#include "Level_Field.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "InfoInstance.h"
#include "Player.h"
#include "EffectPool.h"
#include "Light_Main.h"
#include "Layer.h"
#include "Camera_Cinematic.h"
#include "TimeSocket.h"
#include "Camera_Debug.h"
#include "Terrain.h"
#include "SkyBox.h"
#include "Ranrok.h"
#include "UI_Manager.h"


CLevel_Field::CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{
	m_pInfoInstance = CInfoInstance::GetInstance();
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CLevel_Field::Initialize(void* pArg)
{
	if (FAILED(Ready_Lights())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Volumetric())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_SkyBox())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Background())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Camera())) {
		return E_FAIL;
	}

	if (FAILED(Reday_Layer_EffectPool())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Markers())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_UI(LAYER_UI))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Effect(LAYER_EFFECT))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Monster())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_CutScene())) {
		return E_FAIL;
	}

	m_bLevel = true;
	m_pInfoInstance->Event_CallBack(TEXT("UIManagerFadeIn"));

	return S_OK;
}

HRESULT CLevel_Field::Initialize()
{
	assert(false);
	return E_FAIL;
}

void CLevel_Field::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG


	if (m_pGameInstance->Key_Up(DIK_F1))
	{
		//m_pGameInstance->Set_LevelToChange();
	}

	m_pInfoInstance->Update(fTimeDelta);

	if (true == m_pGameInstance->Check_LevelShouldChange()) {
		m_pInfoInstance->Change_Level();
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::RESTART)))) {
			return;
		}
	}


	if (m_bLevel != m_bCurrentLevel)
	{
		m_bCurrentLevel = m_bLevel;
		UI_STATE eState = UI_STATE::GAMEPLAYER;
		m_pInfoInstance->Event_CallBack(TEXT("Canvas_Change"), &eState);
	}


}

HRESULT CLevel_Field::Render()
{
	_float fDeltaTimeSeconds = m_pGameInstance->Get_TimeDelta(TEXT("Timer_60"));

	static _float fAccumulatedTimeSeconds = 0.0f;
	static _uint  iFrameCountForFps = 0;
	static _float fCurrentFps = 0.0f;
	static _float fAverageFrameTimeMilliseconds = 0.0f;

	fAccumulatedTimeSeconds += fDeltaTimeSeconds;
	++iFrameCountForFps;

	if (fAccumulatedTimeSeconds >= 1.0f)
	{
		if (fAccumulatedTimeSeconds > 0.0f)
		{
			fCurrentFps = static_cast<_float>(iFrameCountForFps) / fAccumulatedTimeSeconds;
			if (fCurrentFps > 0.0f)
			{
				fAverageFrameTimeMilliseconds = 1000.0f / fCurrentFps;
			}
		}

		fAccumulatedTimeSeconds = 0.0f;
		iFrameCountForFps = 0;
	}

	_float fCurrentFrameTimeMilliseconds = fDeltaTimeSeconds * 1000.0f;

	_tchar szWindowTitle[256] = {};

	// 현재 프레임 시간 + 평균 FPS / 평균 프레임 타임(ms)
	_stprintf_s(
		szWindowTitle,
		TEXT("필드레벨입니다 | Frame: %.3f ms | Avg: %.3f ms | FPS: %.1f"),
		fCurrentFrameTimeMilliseconds,
		fAverageFrameTimeMilliseconds,
		fCurrentFps
	);

	SetWindowText(g_hWnd, szWindowTitle);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Lights()
{
	CLight_Main* pLight = { nullptr };

	LIGHT_DESC Desc = {};
	if (m_isDay)
	{
		Desc.vDiffuse = _float4(0.6529f, 0.6157f, 0.7843f, 1.0f);
		Desc.vAmbient = _float4(0.6275f, 0.6275f, 0.6275f, 0.0314f);
		Desc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 0.05f);
	}
	else
	{
		Desc.vDiffuse = _float4(0.0471f, 0.0745f, 0.1294f, 0.2549f);
		Desc.vAmbient = _float4(0.1686f, 0.1765f, 0.1373f, 0.0f);
		Desc.vSpecular = _float4(0.0f, 0.0f, 0.0f, 0.0f);
	}


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLight_Main>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT, &Desc, nullptr, &pLight))) {
		return E_FAIL;
	}

	_float4 vDiffuse = _float4(0.745f, 0.797f, 0.8f, 0.f);
	_float4 vAmbient = _float4(0.1f, 0.13f, 0.13f, 0.f);
	_float4 vSpecular = _float4(0.05f, 0.05f, 0.05f, 0.f);

#if gimch || 진우
	vDiffuse = _float4(0.361f, 0.451f, 0.451f, 0.204f);
	vAmbient = _float4(0.161f, 0.161f, 0.161f, 0.0f);
	vSpecular = _float4(0.05f, 0.05f, 0.05f, 0.f);
#endif // gimch




	pLight->Get_Component<CLight>()->Set_Color(vDiffuse, vAmbient, vSpecular);


	return S_OK;
}

HRESULT CLevel_Field::Ready_Volumetric()
{
	m_pGameInstance->Setting_Volumetirc(1.812f, 0.003f, 0.56f, 1.f, 0.031f);


	return S_OK;
}

HRESULT CLevel_Field::Ready_Camera()
{
#ifdef _DEBUG
	CCamera_Debug::CAMERA_DEBUG_DESC            Camera_Desc{};
	Camera_Desc.fFovy = XMConvertToRadians(60.0f);
	Camera_Desc.fNear = 0.1f;
	Camera_Desc.fFar = 500.f;
	Camera_Desc.vEye = _float3(0.f, 10.f, -10.f);
	Camera_Desc.vAt = _float3(0.f, 0.f, 0.f);
	Camera_Desc.fSpeedPerSec = 5.f;
	Camera_Desc.pCameraKey = CAMERA_DEBUG;
	Camera_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Camera_Desc.fMouseSensor = 0.1f;
	Camera_Desc.iPriority = 53;
	Camera_Desc.pFollowTarget = { nullptr };
	Camera_Desc.pLookTarget = { nullptr };

	CCamera_Debug* pCamera = { nullptr };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Debug>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))) {
		return E_FAIL;
	}

	m_pGameInstance->Add_Camera(NEXT_LEVEL, pCamera, CAMERA_DEBUG);

	if (FAILED(m_pGameInstance->Bind_Camera(NEXT_LEVEL, CAMERA_DEBUG, true))) {
		return E_FAIL;
	}
#endif // _DEBUG

	{
		CCamera_Cinematic::Camera_Cinematic_DESC            Camera_Desc{};
		Camera_Desc.fSpeedPerSec = 5.f;
		Camera_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
		Camera_Desc.fFovy = XMConvertToRadians(60.0f);
		Camera_Desc.fNear = 0.1f;
		Camera_Desc.fFar = 500.f;
		Camera_Desc.pCameraKey = CAMERA_CINEMATIC;
		Camera_Desc.iPriority = 65;
		Camera_Desc.bEnableTransitionLerp = true;
		Camera_Desc.bEnableLookLerp = true;
		Camera_Desc.bEnableFollowLerp = true;
		Camera_Desc.vTransitionTime.y = 1.f;
		Camera_Desc.pFollowTarget = { nullptr };
		Camera_Desc.pLookTarget = { nullptr };

		CCamera_Cinematic* pCamera = { nullptr };
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Cinematic>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))) {
			return E_FAIL;
		}
		m_pGameInstance->Add_Camera(NEXT_LEVEL, pCamera, CAMERA_CINEMATIC);
	}

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_SkyBox()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSkyBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_SKYBOX))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Field::Ready_Background()
{
	m_pGameInstance->Setting_Volumetirc(1.812f, 0.003f, 0.56f, 1.f, 0.031f);



	CInfoInstance::GetInstance()->Load_MapObjects("Dungeon_Map_Data", LAYER_BACKGROUND);
	CInfoInstance::GetInstance()->Load_WorldDecal("Duengon_Decal_Data", LAYER_BACKGROUND);
	CInfoInstance::GetInstance()->Load_PointLights("Duengon_PointLight_Data", LAYER_BACKGROUND);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Camera()
{
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Sound()
{
	return S_OK;
}

HRESULT CLevel_Field::Ready_Markers()
{

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CPlayer::PLAYERDESC playerDesc = {};
	playerDesc.vPos = _float4(-17.96f, 18.74f, 9.11f, 1.f);
	playerDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPlayer>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &playerDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Monster()
{
	CRanrok::RANROKDESC RanrokDesc = {};
	RanrokDesc.vPos = _float4(75.550f, 33.734f, 164.013f, 1.f);
	RanrokDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRanrok>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER, &RanrokDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Field::Reday_Layer_EffectPool()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEffectPool>(g_iStaticLevel, NEXT_LEVEL, LAYER_EFFECTPOOL))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_CutScene()
{
	CMonster* pMonster = { nullptr };
	CCamera* pCamera = { nullptr };
	{
		CLayer* pLayer = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_MONSTER);
		if (nullptr != pLayer) {
			pMonster = pLayer->Get_Object<CRanrok>();
		}
	}
	{
		pCamera = m_pGameInstance->Get_Camera(NEXT_LEVEL, CAMERA_CINEMATIC);
	}

	if (nullptr == pMonster || nullptr == pCamera) {
		return E_FAIL;
	}
	Load_CutSceneXML("../Bin/Resources/Data/CutScene/RanrokCutScene.xml", pCamera, pMonster);
	return S_OK;
}

pair<CLevel*, function<void()>> CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg)
{
	CLevel_Field* pInstance = new CLevel_Field(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLevel_Field");
		SAFE_RELEASE(pInstance);
	}

	return { pInstance, [pInstance]() { pInstance->Ready_Layer_Camera(); pInstance->Ready_Layer_Sound(); } };
}

void CLevel_Field::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}

static _bool IsTrueText(const char* text)
{
	if (!text){ return false; }
	return (strcmp(text, "true") == 0) || (strcmp(text, "1") == 0);
}

static _bool TryReadFloatAttr(tinyxml2::XMLElement* element, const _char* attrName, _float& outValue)
{
	if (!element || !attrName) {
		return false;
	}

	const _char* text = element->Attribute(attrName);
	if (!text || text[0] == '\0'){
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

void CLevel_Field::Load_CutSceneXML(const string& path, CCamera* pCamera, CMonster* pMonster)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS){
		return;
	}

	TimeLine* pTimeLine = new TimeLine();
	tinyxml2::XMLElement* pContentsNode = doc.FirstChildElement("RanrokIntro");
	if (!pContentsNode){
		return;
	}

	pTimeLine->m_vTimer.y = 0.f;
	pContentsNode->QueryFloatAttribute("fDuration", &pTimeLine->m_vTimer.y);

	pTimeLine->m_Sockets.clear();

	for (tinyxml2::XMLElement* pSocketNode = pContentsNode->FirstChildElement("Socket");
		pSocketNode; pSocketNode = pSocketNode->NextSiblingElement("Socket"))
	{
		SOCKETCONTENTS SocketContents = {};

		SocketContents .fRatio = 0.f;
		_int fStartPosition = 0;
		pSocketNode->QueryIntAttribute("fStartPosition", &fStartPosition);
		SocketContents.fRatio = CMyTools::Saturate(fStartPosition / pTimeLine->m_vTimer.y);
		// Info
		tinyxml2::XMLElement* pInfoNode = pSocketNode->FirstChildElement("Info");
		if (pInfoNode != nullptr)
		{
			const _char* pEventNameText = pInfoNode->Attribute("strEventName");
			SocketContents.strEventName = pEventNameText ? pEventNameText : "";

			SocketContents.eTypeParam = TIMESOCKET_PARAM::END;
			if (const _char* pTypeText = pInfoNode->Attribute("eTypeParam"))
			{
				CTimeSocket::TryParse_TimeSocketParam(string_view(pTypeText), SocketContents.eTypeParam);
			}

			SocketContents.eTypeFunc = TIMESOCKET_FUNC::END;
			if (const _char* pFuncText = pInfoNode->Attribute("eTypeFunc")) 
			{
				CTimeSocket::TryParse_TimeSocketFunc(string_view(pFuncText), SocketContents.eTypeFunc);
			}

			SocketContents.pEventTarget = nullptr;
			SocketContents.funcEvent = nullptr;

			if (const _char* pTargetText = pInfoNode->Attribute("pTarget"))
			{
				if (strcmp(pTargetText, "CAMERA_CINEMATIC") == 0)
				{
					CCamera* pEventTarget = pCamera;
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
			}
		}

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

				if (string_view(pTypeIdText) == "Ranrok")
				{
					SocketContents.pOtherTarget =
						m_pGameInstance->Get_Layer(NEXT_LEVEL, SocketContents.wstrLayerName)->Get_Object<CRanrok>();
				}
				else if (string_view(pTypeIdText) == "CAMERA_CINEMATIC")
				{
					SocketContents.pOtherTarget = pCamera;
				}
				else if (string_view(pTypeIdText) == "CAMERA_SHOULDER")
				{
					SocketContents.pOtherTarget = m_pGameInstance->Get_Camera(NEXT_LEVEL, CAMERA_SHOULDER);
				}

				break;
			}

			case TIMESOCKET_PARAM::TRANSFORM:
			{
				tinyxml2::XMLElement* pTransformNode = pParam0Node->FirstChildElement("Transform");
				ReadPxTransform(pTransformNode, SocketContents.pxTransform);
				break;
			}
			case TIMESOCKET_PARAM::END:
			default:
				break;
			}
		}

		// -------- vFlags / Param_10 / Param_11 ----------
		ReadFlags16(pSocketNode->FirstChildElement("vFlags"), SocketContents.vFlags);
		ReadUInt4(pSocketNode->FirstChildElement("Param_10"), SocketContents.vParam_10);
		ReadFloat4(pSocketNode->FirstChildElement("Param_11"), SocketContents.vParam_11);
		CTimeSocket* pSocket = CTimeSocket::Create(&SocketContents);
		if (!pSocket) {
			continue;
		}
		pTimeLine->m_Sockets.push_back(pSocket);
	}
	pTimeLine->m_Sockets.sort([](CTimeSocket* a, CTimeSocket* b) {
		return a->m_Contents.fRatio < b->m_Contents.fRatio;
		});
	pair<_string, TimeLine*> pairResult = { _string(pContentsNode->Name()), pTimeLine };
	m_pInfoInstance->Load_Events(pairResult);
}

#ifdef _DEBUG
void CLevel_Field::Describe_Entity()
{
	GUI::Begin("CutScene");
	if (GUI::Button("ReadyCutScene")) {
		Ready_Layer_CutScene();
	}
	if (GUI::Button("DeActiveCutScene")) {
		_string strDeActiveEvent = "RanrokIntro";
		m_pInfoInstance->DeActive_ActiveEvent(strDeActiveEvent);
	}
	GUI::End();
}
#endif // _DEBUG
