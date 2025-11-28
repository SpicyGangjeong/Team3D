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
#include "Shadow.h"
#include "Camera_Manager.h"
#include "RenderTarget_Manager.h"
#include "Key_Manager.h"
#include "Mouse_Manager.h"
#include "Collider_Manager.h"
#include "Picking.h"
#include "PhysX_Manager.h"
#include "ThreadHolder.h"

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
	m_pThreadHolder = CThreadHolder::Create(4);
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
	m_pPipeLine = CPipeLine::Create();
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
	m_pShadow = CShadow::Create();
	if (nullptr == m_pShadow) {
		return E_FAIL;
	}
	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;
	m_pCollider_Manager = CCollider_Manager::Create(*ppDevice, *ppContext, EngineDesc.iNumCollidableGroup);
	if (nullptr == m_pCollider_Manager) {
		return E_FAIL;
	}
	m_pPhysX_Manager = CPhysX_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPhysX_Manager) {
		return E_FAIL;
	}

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pKey_Manager->Update();
	m_pMouse_Manager->Update();
	//m_pSound_Manager->Update();

	m_pPicking->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pPipeLine->Update();

	m_pObject_Manager->Update(fTimeDelta);

	m_pPhysX_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
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
	m_fTimer_FrameCount = Get_TimeDelta(TEXT("Timer_FrameCount"));
#endif // _DEBUG
}

void CGameInstance::Present_TimeCost() const
{
#ifdef _DEBUG
	_float fTotal = m_fTimer_PriorityUpdate
		+ m_fTimer_Update
		+ m_fTimer_LateUpdate
		+ m_fTimer_DrawCall
		+ m_fTimer_Present;

	GUI::Begin("Previous_Frame_Timer");
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
		GUI::ProgressBar(m_fTimer_DrawCall / fTotal, ImVec2(200.f, 0.f));
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
	GUI::Text("GameInstance RefCNT : %d", m_iRefCnt.load());
	static float values[60] = {};
	static int values_offset = 0;
	static double refresh_time = 0.0;
	if (refresh_time == 0.0)
		refresh_time = GUI::GetTime();
	while (refresh_time < GUI::GetTime()) // Create data at fixed 60 Hz rate for the demo
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

_bool CGameInstance::isIn_WorldFrustum(_fvector vWorldPos, _float fRadius)
{
	return m_pPipeLine->isIn_WorldFrustum(vWorldPos, fRadius);
}

_bool CGameInstance::isIn_LocalFrustum(_fvector vLocalPos, _float fRadius)
{
	return m_pPipeLine->isIn_LocalFrustum(vLocalPos, fRadius);
}

void CGameInstance::Add_Light(_uint _iCurrentLevel, CLight* _pLight)
{
	m_pLight_Manager->Add_Light(_iCurrentLevel, _pLight);
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

HRESULT CGameInstance::Begin_MRT_Include_BackBuffer(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pRenderTarget_Manager->Begin_MRT_Include_BackBuffer(strMRTTag, pDSV);
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

HRESULT CGameInstance::Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D)
{
	return m_pRenderTarget_Manager->Copy_RenderTarget(strTargetTag, pTexture2D);
}
HRESULT CGameInstance::Paste_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D)
{
	return m_pRenderTarget_Manager->Paste_RenderTarget(strTargetTag, pTexture2D);
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
const _float* CGameInstance::Get_CurrentCameraFar()
{
	return m_pCamera_Manager->Get_CurrentCameraFar();
}
void CGameInstance::Force_CamPosition(_fvector vPos)
{
	return m_pCamera_Manager->Force_CamPosition(vPos);
}
HRESULT CGameInstance::Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc)
{
	return m_pShadow->Ready_Shadow_Light(Desc);
}
HRESULT CGameInstance::Bind_Shadow_Resource(CShader* pShader, const _char* pConstantName, D3DTS eType) const
{
	return m_pShadow->Bind_Shadow_Resource(pShader, pConstantName, eType);
}
const _float4x4* CGameInstance::Get_ShadowMatricesPtr()
{
	return m_pShadow->Get_ShadowMatricesPtr();
}
const SHADOW_LIGHT_DESC* CGameInstance::Get_ShadowDesc()
{
	return m_pShadow->Get_ShadowDesc();
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

void CGameInstance::Add_SaveModel(const _char* filePath, SaveModel sModel)
{
	m_sModelMap[filePath] = sModel;
}

SaveModel* CGameInstance::Load_SaveModel(const _char* filePath)
{
	auto iter = m_sModelMap.find(filePath);
	return &iter->second;
}

#pragma region PhysX_Manager
PSX::PxMaterial* CGameInstance::Create_Material(_float3* vMatInfo)
{
	return m_pPhysX_Manager->Create_Material(vMatInfo);
}

void CGameInstance::RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh)
{
	return m_pPhysX_Manager->RegistTriMesh(pName, pPxTriMesh);
}

void CGameInstance::RegistHeight(const _tchar* pName, PSX::PxHeightFieldDesc& Desc)
{
	return m_pPhysX_Manager->RegistHeight(pName, Desc);
}

PSX::PxRigidDynamic* CGameInstance::Add_DynamicActor(CRigidBody_Dynamic& RigidBody)
{
	return m_pPhysX_Manager->Add_DynamicActor(RigidBody);
}
PSX::PxRigidStatic* CGameInstance::Add_StaticActor(CRigidBody_Static& RigidBody)
{
	return m_pPhysX_Manager->Add_StaticActor(RigidBody);
}
PSX::PxRevoluteJoint* CGameInstance::Create_PxRevoluteJoint(PSX::PxRigidActor* pActorFrame, PSX::PxTransform& pxLocalWallFrame, PSX::PxRigidActor* pActorObject, PSX::PxTransform& pxLocalActorFrame)
{
	return m_pPhysX_Manager->Create_PxRevoluteJoint(pActorFrame, pxLocalWallFrame, pActorObject, pxLocalActorFrame);
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
void CGameInstance::Attach_Actor(PSX::PxActor& Actor)
{
	m_pPhysX_Manager->Attach_Actor(Actor);
}
void CGameInstance::Detach_Actor(PSX::PxActor& Actor)
{
	m_pPhysX_Manager->Detach_Actor(Actor);
}
void CGameInstance::Release_Actor(PSX::PxActor& Actor)
{
	m_pPhysX_Manager->Detach_Actor(Actor);
}
#ifdef EDITOR_PROJECT
HRESULT CGameInstance::ConvertToTriMeshes(vector<class CMesh*>& Meshes, vector<class PSX::PxTriangleMesh*>& pxTriMeshes, _fmatrix WorldMatrix)
{
	return m_pPhysX_Manager->ConvertToTriMeshes(Meshes, pxTriMeshes, WorldMatrix);
}
HRESULT CGameInstance::SaveTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes)
{
	return m_pPhysX_Manager->SaveTriMeshes(pPath, TriMeshes);
}
#endif // EDITOR_PROJECT

HRESULT CGameInstance::LoadTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes)
{
	return m_pPhysX_Manager->LoadTriMeshes(pPath, TriMeshes);
}
#pragma endregion

bool		CGameInstance::Key_Pressing(int _iKey)
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Key_Pressing(_iKey);
	}
	return false;
}
bool		CGameInstance::Key_Up(int _iKey)
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Key_Up(_iKey);
	}
	return false;
}
bool		CGameInstance::Key_Down(int _iKey)
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Key_Down(_iKey);
	}
	return false;
}
_bool CGameInstance::Mouse_Pressing(int _iKey)
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_Pressing(_iKey);
	}
	return false;
}
_bool CGameInstance::Mouse_Up(int _iKey)
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_Up(_iKey);
	}
	return false;
}
_bool CGameInstance::Mouse_Down(int _iKey)
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_Down(_iKey);
	}
	return false;
}
_bool CGameInstance::Mouse_StartMove()
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_StartMove();
	}
	return false;
}
_bool CGameInstance::Mouse_Moving()
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_Moving();
	}
	return false;
}
_bool CGameInstance::Mouse_StopMove()
{
	if (false == (GUI::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))) {
		return m_pKey_Manager->Mouse_StopMove();
	}
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

void CGameInstance::Release_Engine()
{
	SAFE_RELEASE(m_pThreadHolder);

	DestroyInstance();

	SAFE_RELEASE(m_pPicking);
	SAFE_RELEASE(m_pCollider_Manager);
	SAFE_RELEASE(m_pShadow);
	SAFE_RELEASE(m_pCamera_Manager);
	SAFE_RELEASE(m_pRenderTarget_Manager);
	SAFE_RELEASE(m_pPipeLine);
	SAFE_RELEASE(m_pKey_Manager);
	SAFE_RELEASE(m_pMouse_Manager);
	SAFE_RELEASE(m_pTimer_Manager);
	SAFE_RELEASE(m_pRenderer);
	SAFE_RELEASE(m_pObject_Manager);
	SAFE_RELEASE(m_pPhysX_Manager);
	SAFE_RELEASE(m_pLevel_Manager);
	SAFE_RELEASE(m_pPrototype_Manager);
	SAFE_RELEASE(m_pLight_Manager); // Light Manager�� m_pObject_Manager ���� ���� �ҷ����� 
	SAFE_RELEASE(m_pGraphic_Device);
}

void CGameInstance::Free()
{
	__super::Free();

}
