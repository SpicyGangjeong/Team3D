#include "pch.h"
#include "GameInstance.h"
#include "Prototype_Manager.h"
#include "GameObject_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Renderer.h"
#include "Camera_Manager.h"
#include "RenderTarget_Manager.h"
#include "Key_Manager.h"
#include "Mouse_Manager.h"
#include "Collider_Manager.h"
#include "Picking.h"
#include "PhysX_Manager.h"
#include "ThreadHolder.h"
#include "Fog.h"
#include "Resource_Manager.h"
#include "Font_Manager.h"
#include "Volumetric.h"

IMPLEMENT_SINGLETON(CGameInstance)


CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	g_iStaticLevel = EngineDesc.iStaticLevel;
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device) {
		return E_FAIL;
	}
	{
		m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels, *ppDevice, *ppContext);
		if (nullptr == m_pPrototype_Manager) {
			return E_FAIL;
		}
		{
			m_pRenderTarget_Manager = CRenderTarget_Manager::Create(*ppDevice, *ppContext);
			if (nullptr == m_pRenderTarget_Manager) {
				return E_FAIL;
			}
			m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
			if (nullptr == m_pRenderer) {
				return E_FAIL;
			}
		}
	}
#ifdef 기무리
	m_pThreadHolder = CThreadHolder::Create(12);
#endif // 기무리
#ifndef 기무리
	m_pThreadHolder = CThreadHolder::Create(6);
#endif // !기무리


	if (nullptr == m_pThreadHolder) {
		return E_FAIL;
	}
	m_pObject_Manager = CGameObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager) {
		return E_FAIL;
	}
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager) {
		return E_FAIL;
	}
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager) {
		return E_FAIL;
	}
	m_pMouse_Manager = CMouse_Manager::Create(EngineDesc.hWnd, *ppDevice, *ppContext);
	if (nullptr == m_pMouse_Manager) {
		return E_FAIL;
	}
	m_pKey_Manager = CKey_Manager::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pKey_Manager) {
		return E_FAIL;
	}
	m_pPipeLine = CPipeLine::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPipeLine) {
		return E_FAIL;
	}
	m_pLight_Manager = CLight_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pLight_Manager) {
		return E_FAIL;
	}
	m_pCamera_Manager = CCamera_Manager::Create(*ppDevice, *ppContext, EngineDesc.iNumLevels);
	if (nullptr == m_pCamera_Manager) {
		return E_FAIL;
	}
	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;
	m_pCollider_Manager = CCollider_Manager::Create(*ppDevice, *ppContext, EngineDesc.iNumCollidableGroup);
	if (nullptr == m_pCollider_Manager) {
		return E_FAIL;
	}
	m_pPhysX_Manager = CPhysX_Manager::Create(*ppDevice, *ppContext, EngineDesc.iNumLevels);
	if (nullptr == m_pPhysX_Manager) {
		return E_FAIL;
	}

	m_pResource_Manager = CResource_Manager::Create(*ppDevice, 1000, EngineDesc.iNumLevels);
	if (nullptr == m_pResource_Manager) {
		return E_FAIL;
	}

	m_pFog = CFog::Create();
	if (nullptr == m_pFog) {
		return E_FAIL;
	}

	m_pVolumetric = CVolumetric::Create(*ppDevice, *ppContext);
	if (nullptr == m_pVolumetric) {
		return E_FAIL;
	}

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager) {
		return E_FAIL;
	}


	m_vViewPortSize = _float2((_float)EngineDesc.iWinSizeX, (_float)EngineDesc.iWinSizeY);

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	
	_float fExcuteTimeDelta =  Update_SlowMotion(fTimeDelta);

	m_pKey_Manager->Update();
	m_pMouse_Manager->Update();
	//m_pSound_Manager->Update();
#ifdef _DEBUG
	m_pResource_Manager->Describe_Entity();
	m_pFog->Update_Fog();
#endif // _DEBUG
	m_pPicking->Update();

	m_pObject_Manager->Priority_Update(fExcuteTimeDelta);

	m_pPipeLine->Update();

	m_pObject_Manager->Update(fExcuteTimeDelta);

	m_pPhysX_Manager->Update(fExcuteTimeDelta);

	m_pObject_Manager->Late_Update(fExcuteTimeDelta);

	m_pVolumetric->Dispatch();

	m_pLevel_Manager->Update(fExcuteTimeDelta);
	m_pObject_Manager->Clear_DeadObj();

	//m_pObstacle_Manager->Refresh_Region();
}

HRESULT CGameInstance::Draw()
{
#ifdef _DEBUG
	Compute_TimeDelta(TEXT("Timer_DrawCall"));
#endif // _DEBUG
	m_pRenderer->Render();

	m_pLevel_Manager->Render();
#ifdef _DEBUG
	Compute_TimeDelta(TEXT("Timer_DrawCall"));
#endif // _DEBUG
	return S_OK;
}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{
	m_pPrototype_Manager->Clear_Resource(iLevelIndex);
	m_pCamera_Manager->Clear_Cameras(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);
	m_pLight_Manager->Light_Clear(iLevelIndex);
	m_pResource_Manager->Clear_LevelResources(iLevelIndex);
	m_pPhysX_Manager->ClearScene(iLevelIndex);
}

_float CGameInstance::Random_Normal()
{
	return static_cast<_float>(rand()) / RAND_MAX;
}

_float CGameInstance::Random_Float(_float fMin, _float fMax)
{
	return fMin + Random_Normal() * (fMax - fMin);
}

_int CGameInstance::Random_Int(_int iMin, _int iMax)
{
	return iMin + (_int)(Random_Normal() * (iMax - iMin));
}

_int CGameInstance::Real_Random_Int(_int iMin, _int iMax)
{
	uniform_int_distribution<_int> rand(iMin, iMax);
	return rand(m_Rng);
}

_float CGameInstance::Real_Random_Float (_float iMin, _float iMax)
{
	uniform_real_distribution<_float> rand(iMin, iMax);
	return rand(m_Rng);
}

_float2 CGameInstance::Get_ViewPortSize()
{
	return m_vViewPortSize;
}

void CGameInstance::BillBoard(CTransform* pTransform)
{
	_matrix ScaleMatrix = {};

	_matrix BillBoardMatrix = Get_Transform_Matrix(D3DTS::VIEW_INV);


 	for (_uint i = 0; i < 3; ++i)
	{
		BillBoardMatrix.r[i] = XMVector3Normalize(BillBoardMatrix.r[i]);
	}

	_float3 vScale = pTransform->Get_Scale();

	_vector vPosition = pTransform->Get_State(STATE::POSITION);

	_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	ScaleMatrix = XMMatrixIdentity() * XMMatrixScaling(vScale.x, vScale.y, vScale.z);

	BillBoardMatrix.r[3] = vPosition;

	pTransform->Set_WorldMatrix(ScaleMatrix * RotationMatrix * BillBoardMatrix);

}

void CGameInstance::SlowMotion(_float fSlowIntense, _float fTime)
{
	if (fTime <= 0 || fSlowIntense <= 0)
		return;

	m_isSlowMotion = true;
	m_vSlowTime = _float2(0.f, fTime);
	m_fSlowIntense = fSlowIntense;
}

_float CGameInstance::Update_SlowMotion(_float fTimeDelta)
{
	_float fExcuteTimeDelta = {};

	if (m_isSlowMotion == false)
		return fTimeDelta;

	m_vSlowTime.x += fTimeDelta;

	if (m_vSlowTime.x >= m_vSlowTime.y) // 시간을 넘겼다면
	{
		m_isSlowMotion = false;
		m_vSlowTime.x = 0.f;
	}
	else
	{
		fExcuteTimeDelta = m_fSlowIntense * fTimeDelta;
	}

	return fExcuteTimeDelta;
}

#ifdef EDITOR_PROJECT

void CGameInstance::Save_ModelFilePath(const _char* FilePath)
{
	m_FilePaths.push_back(FilePath);
}

const _char* CGameInstance::Load_ModelFilePath(_uint iIndex)
{
	return	m_FilePaths[iIndex];
}

const _char* CGameInstance::Load_BinaryModelFilePath(_uint iIndex)
{
	auto iter = m_sModelMap.begin();
	std::advance(iter, iIndex);
	return iter->first;
}

size_t CGameInstance::BinaryModelFilePathCount()
{
	return m_sModelMap.size();
}


size_t CGameInstance::ModelFilePathCount()
{
	return m_FilePaths.size();
}

#endif // EDITOR_PROJECT
void CGameInstance::Render_Begin(const _float4* pClearColor)
{
	m_pGraphic_Device->Clear_BackBuffer_View(pClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();
}

void CGameInstance::Render_End()
{
#ifdef _DEBUG
	Compute_TimeDelta(TEXT("Timer_Present"));
#endif // _DEBUG
	m_pGraphic_Device->Present();
#ifdef _DEBUG
	Compute_TimeDelta(TEXT("Timer_Present"));
#endif // _DEBUG
}

HRESULT CGameInstance::Bind_DepthStencil(CShader* pShader, const _char* pContantName)
{
	return m_pGraphic_Device->Bind_DepthStencil(pShader , pContantName);
}

void CGameInstance::Get_BackBufferPTR(ID3D11Texture2D** pTexture2D)
{
	m_pGraphic_Device->Get_BackBufferPTR(pTexture2D);
}

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Compute_FrameCount()
{
#ifdef _DEBUG
	m_fTimer_PriorityUpdate = Get_TimeDelta(TEXT("Timer_PriorityUpdate"));
	m_fTimer_Update = Get_TimeDelta(TEXT("Timer_Update"));
	m_fTimer_LateUpdate = Get_TimeDelta(TEXT("Timer_LateUpdate"));
	m_fTimer_DrawCall = Get_TimeDelta(TEXT("Timer_DrawCall"));
	m_fTimer_Present = Get_TimeDelta(TEXT("Timer_Present"));
	m_fTimer_Picking = Get_TimeDelta(TEXT("Timer_Picking"));
	m_fTimer_PhysX = Get_TimeDelta(TEXT("Timer_PhysX"));
	m_fTimer_Level = Get_TimeDelta(TEXT("Timer_Level"));
	m_fTimer_FrameCount = Get_TimeDelta(TEXT("Timer_FrameCount"));

	m_fTimer_Render_Priority = Get_TimeDelta(TEXT("Timer_Render_Priority"));
	m_fTimer_Render_Shadow = Get_TimeDelta(TEXT("Timer_Render_Shadow"));
	m_fTimer_Render_NonBlend = Get_TimeDelta(TEXT("Timer_Render_NonBlend"));
	m_fTimer_Render_SSAO = Get_TimeDelta(TEXT("Timer_Render_SSAO"));
	m_fTimer_Render_SSAO_BLUR = Get_TimeDelta(TEXT("Timer_Render_SSAO_BLUR"));
	m_fTimer_Render_LightAcc = Get_TimeDelta(TEXT("Timer_Render_LightAcc"));
	m_fTimer_Render_Blur = Get_TimeDelta(TEXT("Timer_Render_Blur"));
	m_fTimer_Render_Combined = Get_TimeDelta(TEXT("Timer_Render_Combined"));
	m_fTimer_Render_Occlusion = Get_TimeDelta(TEXT("Timer_Render_Occlusion"));
	m_fTimer_Render_EnvironmentPostProcess = Get_TimeDelta(TEXT("Timer_Render_EnvironmentPostProcess"));
	m_fTimer_Render_Fog = Get_TimeDelta(TEXT("Timer_Render_Fog"));
	m_fTimer_Render_Effect = Get_TimeDelta(TEXT("Timer_Render_Effect"));
	m_fTimer_Render_NonLight = Get_TimeDelta(TEXT("Timer_Render_NonLight"));
	m_fTimer_Render_Blend = Get_TimeDelta(TEXT("Timer_Render_Blend"));
	m_fTimer_Render_WeightBlend = Get_TimeDelta(TEXT("Timer_Render_WeightBlend"));
	m_fTimer_Render_PostProcessing = Get_TimeDelta(TEXT("Timer_Render_PostProcessing"));
	m_fTimer_Render_LastColor = Get_TimeDelta(TEXT("Timer_Render_LastColor"));
	m_fTimer_Render_Tone_Mapping = Get_TimeDelta(TEXT("Timer_Render_Tone_Mapping"));
	m_fTimer_Render_UI = Get_TimeDelta(TEXT("Timer_Render_UI"));
	m_fTimer_Render_UI_Overley = Get_TimeDelta(TEXT("Timer_Render_UI_Overley"));
#endif // _DEBUG
}

void CGameInstance::Present_TimeCost() const
{
#pragma region TimeCost
#ifdef _DEBUG
	_float fTotal = m_fTimer_PriorityUpdate
		+ m_fTimer_Update
		+ m_fTimer_LateUpdate
		+ m_fTimer_DrawCall
		+ m_fTimer_Present
		+ m_fTimer_Picking
		+ m_fTimer_PhysX
		+ m_fTimer_Level;

	GUI::PushItemWidth(80);
	GUI::Begin("Previous_Frame_Timer", 0, IMGUI_GLOBAL_BEGIN_FLAG);

	if (GUI::CollapsingHeader("Detail"))
	{
		{
			GUI::ProgressBar(m_fTimer_PriorityUpdate / fTotal, ImVec2(200.f, 0.f));
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("Priority_Update %d", int(m_fTimer_PriorityUpdate / fTotal * 100.f));
		}
		{
			GUI::ProgressBar(m_fTimer_Update / fTotal, ImVec2(200.f, 0.f));
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("Update %d", int(m_fTimer_Update / fTotal * 100.f));
		}
		{
			GUI::ProgressBar(m_fTimer_LateUpdate / fTotal, ImVec2(200.f, 0.f));
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("Late_Update %d", int(m_fTimer_LateUpdate / fTotal * 100.f));
		}
		{
			GUI::ProgressBar(m_fTimer_Picking / fTotal, ImVec2(200.f, 0.f));
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("Picking %d", int(m_fTimer_Picking / fTotal * 100.f));
		}
		{
			GUI::ProgressBar(m_fTimer_PhysX / fTotal, ImVec2(200.f, 0.f));
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("PhysX %d", int(m_fTimer_PhysX / fTotal * 100.f));
		}
		{
			GUI::ProgressBar(m_fTimer_Level / fTotal, ImVec2(200.f, 0.f));
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("Level %d", int(m_fTimer_Level / fTotal * 100.f));
		}
		if (GUI::IsPopupOpen("Renderer_Timer"))
		{
			if (GUI::BeginPopup("Renderer_Timer")) {
				_float fRenderer_Total = m_fTimer_Render_Priority
					+ m_fTimer_Render_Shadow
					+ m_fTimer_Render_NonBlend
					+ m_fTimer_Render_SSAO
					+ m_fTimer_Render_SSAO_BLUR
					+ m_fTimer_Render_LightAcc
					+ m_fTimer_Render_Blur
					+ m_fTimer_Render_Combined
					+ m_fTimer_Render_Occlusion
					+ m_fTimer_Render_EnvironmentPostProcess
					+ m_fTimer_Render_Fog
					+ m_fTimer_Render_Effect
					+ m_fTimer_Render_NonLight
					+ m_fTimer_Render_Blend
					+ m_fTimer_Render_WeightBlend
					+ m_fTimer_Render_PostProcessing
					+ m_fTimer_Render_LastColor
					+ m_fTimer_Render_Tone_Mapping
					+ m_fTimer_Render_UI
					+ m_fTimer_Render_UI_Overley;
				{
					GUI::ProgressBar(m_fTimer_Render_Priority / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Priority %d", int(m_fTimer_Render_Priority / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_Shadow / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Shadow %d", int(m_fTimer_Render_Shadow / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_NonBlend / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_NonBlend %d", int(m_fTimer_Render_NonBlend / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_SSAO / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_SSAO %d", int(m_fTimer_Render_SSAO / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_SSAO_BLUR / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_SSAO_BLUR %d", int(m_fTimer_Render_SSAO_BLUR / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_LightAcc / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_LightAcc %d", int(m_fTimer_Render_LightAcc / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_Blur / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Blur %d", int(m_fTimer_Render_Blur / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_Combined / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Combined %d", int(m_fTimer_Render_Combined / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_Occlusion / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Occlusion %d", int(m_fTimer_Render_Occlusion / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_EnvironmentPostProcess / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_EnvironmentPostProcess %d", int(m_fTimer_Render_EnvironmentPostProcess / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_Fog / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Fog %d", int(m_fTimer_Render_Fog / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_Effect / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Effect %d", int(m_fTimer_Render_Effect / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_NonLight / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_NonLight %d", int(m_fTimer_Render_NonLight / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_Blend / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Blend %d", int(m_fTimer_Render_Blend / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_WeightBlend / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_WeightBlend %d", int(m_fTimer_Render_WeightBlend / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_PostProcessing / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_PostProcessing %d", int(m_fTimer_Render_PostProcessing / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_LastColor / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_LastColor %d", int(m_fTimer_Render_LastColor / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_Tone_Mapping / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_Tone_Mapping %d", int(m_fTimer_Render_Tone_Mapping / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_UI / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_UI %d", int(m_fTimer_Render_UI / fRenderer_Total * 100.f));
				}
				{
					GUI::ProgressBar(m_fTimer_Render_UI_Overley / fRenderer_Total, ImVec2(200.f, 0.f));
					GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
					GUI::Text("Render_UI_Overley %d", int(m_fTimer_Render_UI_Overley / fRenderer_Total * 100.f));
				}
				GUI::EndPopup();
			}
		}
		{
			GUI::ProgressBar(m_fTimer_DrawCall / fTotal, ImVec2(200.f, 0.f));
			GUI::SameLine();
			if (GUI::SmallButton("Renderer_Timer")) {
				GUI::OpenPopup("Renderer_Timer");
			}
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("DrawCall %d", int(m_fTimer_DrawCall / fTotal * 100.f));
		}
		{
			GUI::ProgressBar(m_fTimer_Present / fTotal, ImVec2(200.f, 0.f));
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("Present %d", int(m_fTimer_Present / fTotal * 100.f));
		}
		{
			GUI::ProgressBar(fTotal / m_fTimer_FrameCount, ImVec2(200.f, 0.f));
			GUI::SameLine(0.f, GUI::GetStyle().ItemInnerSpacing.x);
			GUI::Text("Timer_Occupancy %d", int(m_fTimer_Present / fTotal * 100.f));
		}
	}
	GUI::Text("GameInstance RefCNT : %d", m_iRefCnt.load());
	static float values[60] = {};
	static int values_offset = 0;
	static double refresh_time = 0.0;
	if (refresh_time == 0.0)
		refresh_time = GUI::GetTime();
	while (refresh_time < GUI::GetTime()) 
	{
		values[values_offset] = m_fTimer_FrameCount;
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		refresh_time += 1.0f / 60.0f;
	}
	{
		float average = 0.0f;
		for (int n = 0; n < IM_ARRAYSIZE(values); n++)
			average += values[n];
		average /= (float)IM_ARRAYSIZE(values);
		char overlay[32];
		sprintf_s(overlay, "avg %f", average);

		float minY = 0.f;
		float maxY = 0.025f;
		//float threshold = 166.f / 250.f; // 약 0.664f
		ImVec2 graph_size(0, 80.0f);
		ImVec2 start_pos = GUI::GetCursorScreenPos();

		GUI::PlotLines("##FrameCount", values, IM_ARRAYSIZE(values), values_offset,
			overlay, minY, maxY, ImVec2(0, 80.0f));

		// PlotLines의 실제 표시 영역 가져오기
		ImVec2 plot_min = GUI::GetItemRectMin();
		ImVec2 plot_max = GUI::GetItemRectMax();

		// 기준선 (60FPS)
		float threshold = 1.0f / 60.0f; // 0.0166초
		float normalized = (threshold - minY) / (maxY - minY);
		float y = plot_max.y - normalized * (plot_max.y - plot_min.y);

		// 빨간 선 그리기
		ImDrawList* draw_list = GUI::GetWindowDrawList();
		draw_list->AddLine(
			ImVec2(plot_min.x, y),
			ImVec2(plot_max.x, y),
			IM_COL32(255, 0, 0, 255),
			2.0f
		);
	}
	{
		GUI::Text("%f\t%f", GUI::GetIO().Framerate, GUI::GetIO().DeltaTime);
	}
	GUI::End();
#endif // _DEBUG
#pragma endregion
}

HRESULT CGameInstance::Change_Level(CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(pNewLevel);
}

_uint CGameInstance::Get_CurrentLevelID()
{
	return m_pLevel_Manager->Get_Current_LevelID();
}
_uint CGameInstance::Get_NextLevelID()
{
	return m_pLevel_Manager->Get_NextLevelID();
}

_bool CGameInstance::Check_LevelShouldChange()
{
	return m_pLevel_Manager->Check_LevelShouldChange();
}

void CGameInstance::Set_LevelToChange()
{
	return m_pLevel_Manager->Set_LevelToChange();
}

CComponent* CGameInstance::Clone_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag, void* pArg, CGameObject* pOwner)
{
	return m_pPrototype_Manager->Clone_Asset_Prototype(iTargetLevel, strPrototypeTag, pArg, pOwner);
}

CComponent* CGameInstance::Find_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag)
{
	return m_pPrototype_Manager->Find_Asset_Prototype(iTargetLevel, strPrototypeTag);
}

HRESULT CGameInstance::Add_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	return m_pPrototype_Manager->Add_Asset_Prototype(iTargetLevel, strPrototypeTag, pPrototype);
}

CLayer* CGameInstance::Get_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_Layer(iLayerLevelIndex, strLayerTag);
}
void CGameInstance::Clear_Objects_With_Layers(_uint iLevelIndex)
{
	m_pObject_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{	
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

void CGameInstance::Render_PreShadow(const _float4x4& ViewMatrix, const _float4x4& ProjMatrix)
{
	return m_pRenderer->Render_PreShadow(ViewMatrix, ProjMatrix);
}

RENDER CGameInstance::Get_CurrentRenderPass()
{
	return m_pRenderer->Get_CurrentRenderPass();
}

HRESULT CGameInstance::Bind_PreShadowMatrix(CShader* pShader, const _char* pConstants, D3DTS eType)
{
	return m_pRenderer->Bind_PreShadowMatrix(pShader, pConstants, eType);
}

HRESULT CGameInstance::Bind_PrevMatrix(CShader* pShader, const _char* pConstants, D3DTS eType)
{
	return m_pRenderer->Bind_PrevMatrix(pShader, pConstants, eType);
}

void CGameInstance::Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformStateMatrix);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4* CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

const _vector CGameInstance::Get_CamXMPosition()
{
	return m_pPipeLine->Get_CamXMPosition();
}

void CGameInstance::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
	m_pPipeLine->Transform_Frustum_ToLocalSpace(WorldMatrixInverse);
}

_bool CGameInstance::IsIn_WorldFrustum(_fvector vWorldPos, _float fRadius)
{
	return m_pPipeLine->IsIn_WorldFrustum(vWorldPos, fRadius);
}

_bool CGameInstance::IsIn_LocalFrustum(_fvector vLocalPos, _float fRadius)
{
	return m_pPipeLine->IsIn_LocalFrustum(vLocalPos, fRadius);
}

pair<_bool, _ubyte> CGameInstance::IsIn_ShadowViewFrustum(_fvector vWorldCenter, _float fRadius)
{
	return m_pPipeLine->IsIn_ShadowViewFrustum(vWorldCenter, fRadius);
}

HRESULT CGameInstance::Bind_CascadeSplitRatio(CShader* pShader, const _char* pConstantName, _bool bNear)
{
	return m_pPipeLine->Bind_CascadeSplitRatio(pShader, pConstantName, bNear);
}

HRESULT CGameInstance::Bind_CascadeBias(CShader* pShader, const _char* pConstantName)
{
	return m_pPipeLine->Bind_CascadeBias(pShader, pConstantName);
}

HRESULT CGameInstance::Bind_GlobalSRV(CShader* pShader, const _tchar* wszKeyGlobalSRV, const _char* pConstantName)
{
	return m_pPipeLine->Bind_GlobalSRV(pShader, wszKeyGlobalSRV, pConstantName);
}

HRESULT CGameInstance::Load_GlobalSRV(const _tchar* wszKeyGlobalSRV, filesystem::path pathSRVFolder)
{
	return m_pPipeLine->Load_GlobalSRV(wszKeyGlobalSRV, pathSRVFolder);
}

HRESULT CGameInstance::Ready_Shadow_Light(const _float4& vShadowDirRPYQuat)
{
	return m_pPipeLine->Ready_Shadow_Light(vShadowDirRPYQuat);
}

HRESULT CGameInstance::Bind_Shadow_Resource(CShader* pShader, const _char* pConstantName, D3DTS eType, SHADOW eShadowType) const
{
	return m_pPipeLine->Bind_Shadow_Resource(pShader, pConstantName, eType, eShadowType);
}

const _float4x4* CGameInstance::Get_ShadowMatricesPtr(_uint iShadowBoxIndex)
{
	return m_pPipeLine->Get_ShadowMatricesPtr(iShadowBoxIndex);
}

_float  CGameInstance::Get_ShadowBoxFar(_uint iShadowBoxIndex)
{
	return m_pPipeLine->Get_ShadowBoxFar(iShadowBoxIndex);
}

HRESULT CGameInstance::Begin_OutLine_Write(_uint iDSSRef)
{
	return m_pPipeLine->Begin_OutLine_Write(iDSSRef);
}

HRESULT CGameInstance::End_OutLine_Write()
{
	return m_pPipeLine->End_OutLine_Write();
}

void CGameInstance::Add_Light(_uint _iCurrentLevel, CLight* _pLight)
{
	m_pLight_Manager->Add_Light(_iCurrentLevel, _pLight);
}

void CGameInstance::Add_Light_Group(_uint _iCurrentLevel, CLight* _pLight)
{
	m_pLight_Manager->Add_Light_Group(_iCurrentLevel, _pLight);
}

void CGameInstance::Delete_Light(_uint _iCurrentLevel, CLight* _pLight)
{
	m_pLight_Manager->Delete_Light(_iCurrentLevel, _pLight);
}

const LIGHT_DESC* CGameInstance::Get_Light_Info(_uint _iCurrentLevel, _uint _iLightIndex)
{
	return m_pLight_Manager->Get_Light_Info(_iCurrentLevel, _iLightIndex);
}

HRESULT CGameInstance::Render_Lights(_uint _iCurrentLevel, CShader* pShader, CVIBuffer* pVIBuffer)
{
	return m_pLight_Manager->Render_Lights(_iCurrentLevel, pShader, pVIBuffer);
}


HRESULT CGameInstance::Add_ColliderGroup(_uint iColliderGroup, class CCollider* pBounding)
{
	return m_pCollider_Manager->Add_ColliderGroup(iColliderGroup, pBounding);
}
void	CGameInstance::Refresh_Collider_Manager()
{
	return m_pCollider_Manager->Refresh_Collider_Manager();
}
void	CGameInstance::Collide(_uint iCollideGroupA, _uint iCollideGroupB)
{
	return m_pCollider_Manager->Collide(iCollideGroupA, iCollideGroupB);
}


HRESULT CGameInstance::Add_RenderTarget(const _wstring& strRenderTargetKey, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pRenderTarget_Manager->Add_RenderTarget(strRenderTargetKey, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMultiRenderTargetKey, const _wstring& strRenderTargetKey)
{
	return m_pRenderTarget_Manager->Add_MRT(strMultiRenderTargetKey, strRenderTargetKey);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pRenderTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}

HRESULT CGameInstance::Begin_MRT_NonClear(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pRenderTarget_Manager->Begin_MRT_NonClear(strMRTTag, pDSV);
}

HRESULT CGameInstance::Begin_MRT_Include_BackBuffer(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool isClear)
{
	return m_pRenderTarget_Manager->Begin_MRT_Include_BackBuffer(strMRTTag, pDSV , isClear);
}

HRESULT CGameInstance::Begin_MRT_NO_DepthStencil(const _wstring& strMRTTag)
{
	return m_pRenderTarget_Manager->Begin_MRT_NO_DepthStencil(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pRenderTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RenderTarget(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pRenderTarget_Manager->Bind_RenderTarget(strTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Copy_RenderTargetTo(const _wstring& strSrcTag, ID3D11Texture2D* pDst2D)
{
	return m_pRenderTarget_Manager->Copy_RenderTargetTo(strSrcTag, pDst2D);
}
HRESULT CGameInstance::Copy_RenderTargetFrom(const _wstring& strDstTag, ID3D11Texture2D* pSrc2D)
{
	return m_pRenderTarget_Manager->Copy_RenderTargetFrom(strDstTag, pSrc2D);
}
HRESULT CGameInstance::Copy_RenderTargetAToB(const _wstring& strATag, const _wstring& strBTag)
{
	return m_pRenderTarget_Manager->Copy_RenderTargetAToB(strATag, strBTag);
}
HRESULT CGameInstance::Accumulate_RenderTarget(CVIBuffer_Rect* pVIBuffer, CShader* pShader, const _wstring& wstrRenderTarget_SrcA, const _wstring& wstrRenderTarget_SrcB, const _wstring& wstrRenderTarget_Target, SHADER_PASS_DEFERRED ePass)
{
	return m_pRenderTarget_Manager->Accumulate_RenderTarget(pVIBuffer, pShader, wstrRenderTarget_SrcA, wstrRenderTarget_SrcB, wstrRenderTarget_Target, ePass);
}
HRESULT CGameInstance::Refit_RenderTarget(CVIBuffer_Rect* pVIBuffer, CShader* pShader, const _wstring& wstrRenderTargetInput, const _wstring& wstrRenderTargetOutput, SHADER_PASS_DEFERRED ePass)
{
	return m_pRenderTarget_Manager->Refit_RenderTarget(pVIBuffer,	pShader, wstrRenderTargetInput, wstrRenderTargetOutput, ePass);
}
HRESULT CGameInstance::Finish_RenderTarget(CVIBuffer_Rect* pVIBuffer, CShader* pShader, const _wstring& wstrRenderTargetOriginal, const _wstring& wstrRenderTargetBloomed, SHADER_PASS_DEFERRED ePass)
{
	return m_pRenderTarget_Manager->Finish_RenderTarget(pVIBuffer, pShader, wstrRenderTargetOriginal, wstrRenderTargetBloomed, ePass);
}
HRESULT CGameInstance::Bind_CS_RenderTarget(_uint iIndex, const _wstring& strTargetTag)
{
	return m_pRenderTarget_Manager->Bind_CS_RenderTarget(iIndex, strTargetTag);
}

HRESULT CGameInstance::Clear_RenderTarget(const _wstring& strRenderTargetKey)
{
	return m_pRenderTarget_Manager->Clear_RenderTarget(strRenderTargetKey);
}

#ifdef _DEBUG
void CGameInstance::RenderTarget_Debuger()
{
	m_pRenderTarget_Manager->RenderTarget_Debuger();
}

HRESULT CGameInstance::Render_RenderTarget_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pRenderTarget_Manager->Render_RenderTarget_Debug(pShader, pVIBuffer);
}
#endif // _DEBUG
HRESULT CGameInstance::Clear_Cameras(_uint iLevel)
{
	return m_pCamera_Manager->Clear_Cameras(iLevel);
}
HRESULT CGameInstance::Release_Camera(_uint iLevel, const _wstring& wstrCameraKey)
{
	return m_pCamera_Manager->Release_Camera(iLevel, wstrCameraKey);
}
HRESULT CGameInstance::Add_Camera(_uint iLevel, CCamera* pCamera, const _wstring& strCameraKey)
{
	return m_pCamera_Manager->Add_Camera(iLevel, pCamera, strCameraKey);
}
HRESULT CGameInstance::Bind_Camera(_uint iLevel, const _wstring& strCameraKey, _bool bIgnorePriority)
{
	return m_pCamera_Manager->Bind_Camera(iLevel, strCameraKey, bIgnorePriority);
}
HRESULT CGameInstance::IsBinded_Camera(const _wstring& strCameraKey)
{
	return m_pCamera_Manager->IsBinded_Camera(strCameraKey);
}
_vector CGameInstance::Get_CameraLook()
{
	return m_pCamera_Manager->Get_CameraLook();
}
_float CGameInstance::Get_CameraFov()
{
	return m_pCamera_Manager->Get_CameraFov();
}
_float CGameInstance::Get_CameraNear()
{
	return m_pCamera_Manager->Get_CameraNear();
}
const _float* CGameInstance::Get_CurrentCameraFar()
{
	return m_pCamera_Manager->Get_CurrentCameraFar();
}
void CGameInstance::Force_CamPosition(_fvector vPos)
{
	return m_pCamera_Manager->Force_CamPosition(vPos);
}
_bool CGameInstance::isPicking(_float3* pOut)
{
	return m_pPicking->isPicking(pOut);
}
#ifdef EDITOR_PROJECT
_bool CGameInstance::SaveAssimpModel(const _char* filename)
{
	auto iter = m_ModelMap.find(filename);
	return iter->second->SaveAssimpModel(filename);
}

void CGameInstance::Add_ModelToMap(const _char* filePath, CModel* pModel)
{
	m_ModelMap[filePath] = pModel;
}
#endif

void CGameInstance::Add_SaveModel(const _char* filePath, SaveModel& sModel)
{
	lock_guard<mutex> lock(m_mtxLoadModelLock);
	m_sModelMap[filePath] = sModel;
}

SaveModel* CGameInstance::Load_SaveModel(const _char* filePath)
{
	lock_guard<mutex> lock(m_mtxLoadModelLock);
	map<const _char*, SaveModel>::iterator iter = m_sModelMap.find(filePath);
	if (iter == m_sModelMap.end()) {
		return nullptr;
	}
	SaveModel* pOut = &iter->second;
	return pOut;
}

#pragma region PhysX_Manager
PSX::PxMaterial* CGameInstance::Create_Material(_float3* vMatInfo)
{
	return m_pPhysX_Manager->Create_Material(vMatInfo);
}

void CGameInstance::RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh, _uint iLevel)
{
	return m_pPhysX_Manager->RegistTriMesh(pName, pPxTriMesh, iLevel);
}

void CGameInstance::RegistHeight(const _tchar* pName, PSX::PxHeightFieldDesc& Desc, _uint iLevel)
{
	return m_pPhysX_Manager->RegistHeight(pName, Desc, iLevel);
}

PSX::PxRigidDynamic* CGameInstance::Add_DynamicActor(CRigidBody_Dynamic& RigidBody, _uint iLevel)
{
	return m_pPhysX_Manager->Add_DynamicActor(RigidBody, iLevel);
	}
PSX::PxRigidStatic* CGameInstance::Add_StaticActor(CRigidBody_Static& RigidBody, _uint iLevel)
{
	return m_pPhysX_Manager->Add_StaticActor(RigidBody, iLevel);
}
PSX::PxJoint* CGameInstance::Create_PxJoint(PHYSX_JOINT eType, PSX::PxRigidActor* pActor0, PSX::PxTransform& pxLocalFrame0, PSX::PxRigidActor* pActor1, PSX::PxTransform& pxLocalFrame1)
{
	return m_pPhysX_Manager->Create_PxJoint(eType, pActor0, pxLocalFrame0, pActor1, pxLocalFrame1);
}
PSX::PxD6Joint* CGameInstance::Create_PxD6Joint(PSX::PxRigidDynamic* pActor0, PSX::PxRigidDynamic* pActor1, const PSX::PxTransform& pxJointWorldPos)
{
	return m_pPhysX_Manager->Create_PxD6Joint(pActor0, pActor1, pxJointWorldPos);
}
_bool CGameInstance::SphereCast(_float fRadius, _float3 vStartPos, _float3 vDir, _float fDistance, PSX::PxHitFlags flagHitsData, PSX::PxQueryFlags flagQuery, PSX::PxSweepBuffer& hitBuffer)
{
	return m_pPhysX_Manager->SphereCast(fRadius, vStartPos, vDir, fDistance, flagHitsData, flagQuery, hitBuffer);
}
_bool CGameInstance::SphereCast(_float fRadius, _fvector vStartPos, _gvector vDir, _float fDistance, PSX::PxHitFlags flagHitsData, PSX::PxQueryFlags flagQuery, PSX::PxSweepBuffer& hitBuffer)
{
	return m_pPhysX_Manager->SphereCast(fRadius, vStartPos, vDir, fDistance, flagHitsData, flagQuery, hitBuffer);
}
_bool CGameInstance::RayCast(_float3 _vStartPos, _float3 _vDir, _float fDistance, PSX::PxRaycastHit* pRayHitArray, _uint iMaxHitCapacity, _uint& iOutHitCount)
{
	return m_pPhysX_Manager->RayCast(_vStartPos, _vDir, fDistance, pRayHitArray, iMaxHitCapacity, iOutHitCount);
}
_bool CGameInstance::RayCast(_fvector _vStartPos, _gvector _vDir, _float fDistance, PSX::PxRaycastHit* pRayHitArray, _uint iMaxHitCapacity, _uint& iOutHitCount)
{
	return m_pPhysX_Manager->RayCast(_vStartPos, _vDir, fDistance, pRayHitArray, iMaxHitCapacity, iOutHitCount);
}
PSX::PxController* CGameInstance::Add_CapsuleController(PSX::PxCapsuleControllerDesc& Desc)
{
	return m_pPhysX_Manager->Add_CapsuleController(Desc);
}
PSX::PxController* CGameInstance::Add_BoxController(PSX::PxBoxControllerDesc& Desc)
{
	return m_pPhysX_Manager->Add_BoxController(Desc);
}
PSX::PxController* CGameInstance::Get_Controller(_uint iControllerIndex)
{
	return m_pPhysX_Manager->Get_Controller(iControllerIndex);
}
void CGameInstance::ReleaseController(_uint iControllerIndex)
{
	m_pPhysX_Manager->ReleaseController(iControllerIndex);
}
void CGameInstance::Attach_Actor(PSX::PxActor& Actor, _uint iLevel)
{
	m_pPhysX_Manager->Attach_Actor(Actor, iLevel);
}
void CGameInstance::Detach_Actor(PSX::PxActor& Actor, _uint iLevel)
{
	m_pPhysX_Manager->Detach_Actor(Actor, iLevel);
}
void CGameInstance::Release_Actor(PSX::PxActor& Actor)
{
	m_pPhysX_Manager->Release_Actor(Actor);
}
void CGameInstance::ApplyFilterData(PSX::PxRigidActor* pRigidActor)
{
	m_pPhysX_Manager->ApplyFilterData(pRigidActor);
}
HRESULT CGameInstance::ConvertToTriMeshes(vector<class CMesh*>& Meshes, vector<class PSX::PxTriangleMesh*>& pxTriMeshes, _fmatrix WorldMatrix)
{
	return m_pPhysX_Manager->ConvertToTriMeshes(Meshes, pxTriMeshes, WorldMatrix);
}
#ifdef EDITOR_PROJECT
HRESULT CGameInstance::SaveTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes)
{
	return m_pPhysX_Manager->SaveTriMeshes(pPath, TriMeshes);
}
void CGameInstance::Add_Editor_Plane(PHYSX_USERDATA& PlaneData)
{
	m_pPhysX_Manager->Add_Editor_Plane(PlaneData);
}
#endif // EDITOR_PROJECT

HRESULT CGameInstance::LoadTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes)
{
	return m_pPhysX_Manager->LoadTriMeshes(pPath, TriMeshes);
}
#pragma endregion

#pragma region FOG
void CGameInstance::Set_Fog(_float fFogDensity, _float fPow)
{
	m_pFog->Set_Fog(fFogDensity, fPow);
}
void CGameInstance::Set_FogColor(_float4& vFogColor)
{
	m_pFog->Set_FogColor(vFogColor);
}

HRESULT CGameInstance::Bind_FogValue(class CShader* pShader)
{
	return m_pFog->Bind_FogValue(pShader);
}
ID3D11ShaderResourceView* CGameInstance::Add_Resource(const _char* pFilePath, _uint iLevel)
{
	return m_pResource_Manager->Add_Texture(pFilePath, iLevel);
}

void CGameInstance::Clear_LevelResources(_uint iLevel)
{
	m_pResource_Manager->Clear_LevelResources(iLevel);
}

#pragma endregion // FOG

#pragma region INPUT

bool		CGameInstance::Key_Pressing(int _iKey)
{
#ifndef _DEBUG
	return m_pKey_Manager->Key_Pressing(_iKey);
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Key_Pressing(_iKey);
	}
#endif // _DEBUG
	return false;
}
bool		CGameInstance::Key_Up(int _iKey)
{
#ifndef _DEBUG
	return m_pKey_Manager->Key_Up(_iKey);
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Key_Up(_iKey);
	}
#endif // _DEBUG
	return false;
}
bool		CGameInstance::Key_Down(int _iKey)
{
#ifndef _DEBUG
	return m_pKey_Manager->Key_Down(_iKey);
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Key_Down(_iKey);
	}
#endif // _DEBUG
	return false;
}
_bool CGameInstance::Mouse_Pressing(int _iKey)
{
#ifndef _DEBUG
	return m_pKey_Manager->Mouse_Pressing(_iKey);
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_Pressing(_iKey);
	}
#endif // _DEBUG
	return false;
}
_bool CGameInstance::Mouse_Up(int _iKey)
{
#ifndef _DEBUG
	return m_pKey_Manager->Mouse_Up(_iKey);
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_Up(_iKey);
	}
#endif // _DEBUG
	return false;
}
_bool CGameInstance::Mouse_Down(int _iKey)
{
#ifndef _DEBUG
	return m_pKey_Manager->Mouse_Down(_iKey);
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_Down(_iKey);
	}
#endif // _DEBUG
	return false;
}
_bool CGameInstance::Mouse_StartMove()
{
#ifndef _DEBUG
	return m_pKey_Manager->Mouse_StartMove();
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_StartMove();
	}
#endif // _DEBUG
	return false;
}
_bool CGameInstance::Mouse_Moving()
{
#ifndef _DEBUG
	return m_pKey_Manager->Mouse_Moving();
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_Moving();
	}
#endif // _DEBUG
	return false;
}
_bool CGameInstance::Mouse_StopMove()
{
#ifndef _DEBUG
	return m_pKey_Manager->Mouse_StopMove();
#endif // !_DEBUG
#ifdef _DEBUG
	if (OPTIONAL_TRUE_KEYINPUTGUICHECK false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_StopMove();
	}
#endif // _DEBUG
	return false;
}
_float3 CGameInstance::Get_MouseMove()
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		_float3 vMouseMove = m_pMouse_Manager->Get_MouseMove();
		_float3 vKeyMove = m_pKey_Manager->Get_MouseMove();
		vMouseMove.z = vKeyMove.z;
		return vMouseMove;
	}
	return { 0.f, 0.f, 0.f };
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pKey_Manager->Get_DIMouseMove(eMouseState);
}

void    CGameInstance::Picking()
{
	return m_pMouse_Manager->Picking();
}
HRESULT CGameInstance::Ray_WorldToLocal(_fmatrix* InvWorldMatrix)
{
	return m_pMouse_Manager->Ray_WorldToLocal(InvWorldMatrix);
}
_bool   CGameInstance::MousePicking_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3& pOut)
{
	return m_pMouse_Manager->MousePicking_InLocalSpace(vPointA, vPointB, vPointC, pOut);
}
_bool   CGameInstance::MousePicking_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3& pOut)
{
	return m_pMouse_Manager->MousePicking_InWorldSpace(vPointA, vPointB, vPointC, pOut);
}
//void CGameInstance::Mouse_Render()
//{
//	m_pMouse_Manager->Mouse_Render();
//}
POINT	CGameInstance::Get_MouseViewPortPos()
{
	return m_pMouse_Manager->Get_MouseViewPortPos();
}
_bool	CGameInstance::Toggle_MouseCenter()
{
	return m_pMouse_Manager->Toggle_MouseCenter();
}

#pragma endregion // INPUT

#pragma region FONT_MANAGER

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float vScale)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, vScale);
}

HRESULT CGameInstance::Render_Rotation_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float Rotation, _float vScale)
{
	return m_pFont_Manager->Render_Rotate(strFontTag, pText, vPosition, vColor, Rotation, vScale);
}

HRESULT CGameInstance::Perspective_Render_Text(_matrix View, _matrix Proj, const _wstring& strFontTag, const _tchar* pText, const _fvector& vPosition, _fvector vColor, _float vScale)
{
	return m_pFont_Manager->Perspective_Render(View, Proj, strFontTag, pText, vPosition, vColor, vScale);
}

_float CGameInstance::FontSizeX(const _wstring& strFontTag, const _tchar* pText)
{
	return m_pFont_Manager->FontSizeX(strFontTag, pText);
}



#pragma endregion

#pragma region VOLUMETRIC
ID3D11ShaderResourceView* CGameInstance::Get_VolumeSRV()
{
	return m_pVolumetric->Get_VolumeSRV();
}

_float* CGameInstance::Get_DepthPackExponentPtr()
{
	return m_pVolumetric->Get_DepthPackExponentPtr();
}

void CGameInstance::Setting_Volumetirc(_float fDensity, _float fLightIntensity, _float fAsymmetryParameter, _float fDepthPackExponent)
{
	m_pVolumetric->Setting_Volumetirc(fDensity , fLightIntensity , fAsymmetryParameter, fDepthPackExponent);
}

#pragma endregion
void CGameInstance::Release_Engine()
{
	SAFE_RELEASE(m_pThreadHolder);

	DestroyInstance();

	SAFE_RELEASE(m_pFont_Manager);
	SAFE_RELEASE(m_pFog);
	SAFE_RELEASE(m_pPicking);
	SAFE_RELEASE(m_pCollider_Manager);
	SAFE_RELEASE(m_pCamera_Manager);
	SAFE_RELEASE(m_pRenderTarget_Manager);
	SAFE_RELEASE(m_pPipeLine);
	SAFE_RELEASE(m_pKey_Manager);
	SAFE_RELEASE(m_pMouse_Manager);
	SAFE_RELEASE(m_pTimer_Manager);
	SAFE_RELEASE(m_pVolumetric);
	SAFE_RELEASE(m_pRenderer);
	SAFE_RELEASE(m_pObject_Manager);
	SAFE_RELEASE(m_pPhysX_Manager);
	SAFE_RELEASE(m_pLevel_Manager);
	SAFE_RELEASE(m_pPrototype_Manager);
	SAFE_RELEASE(m_pLight_Manager); // Light Manager�� m_pObject_Manager ���� ���� �ҷ����� 
	SAFE_RELEASE(m_pResource_Manager);
	SAFE_RELEASE(m_pGraphic_Device);
}

void CGameInstance::Free()
{
	__super::Free();

}
