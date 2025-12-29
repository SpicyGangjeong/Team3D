#pragma once
#include "Prototype_Manager.h"
#include "ThreadHolder.h"
#include "GameObject_Manager.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;
	mutex m_mtxLoadModelLock = {};
public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

	_float Random_Normal();
	_float Random_Float(_float fMin, _float fMax);
	_int   Random_Int(_int iMin, _int iMax);
	_int   Real_Random_Int(_int iMin, _int iMax);
	_float Real_Random_Float(_float fMin, _float fMax);
	_float2 Get_ViewPortSize();
	void BillBoard(CTransform* pTransform);
	void SlowMotion(_float fSlowIntense, _float fTime);

#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin(const _float4* pClearColor);
	void Render_End();
	HRESULT Bind_DepthStencil(class CShader* pShader, const _char* pContantName);
	void Get_BackBufferPTR(ID3D11Texture2D** pTexture2D);
#pragma endregion

#pragma region TIMER_MANAGER
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);
	void	Compute_FrameCount();
	void	Present_TimeCost() const;
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(class CLevel* pNewLevel);
	_uint	Get_CurrentLevelID();
	_uint	Get_NextLevelID();
	_bool	Check_LevelShouldChange();
	void	Set_LevelToChange();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	CComponent* Clone_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag, void* pArg, class CGameObject* pOwner);
	CComponent* Find_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag);
	HRESULT Add_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag, CComponent* pPrototype);

	template<typename T>
	HRESULT Add_Prototype(_uint iLevelNumber, T* pPrototype)
	{
		if (FAILED(m_pPrototype_Manager->CPrototype_Manager::Add_Prototype<T>(iLevelNumber, pPrototype)))
			return E_FAIL;

		return S_OK;
	}
	template<typename T>
	T* Clone_Prototype(_uint iLevelNumber, void* pArg, class CGameObject* pOwner = nullptr)
	{
		return m_pPrototype_Manager->CPrototype_Manager::Clone_Prototype<T>(iLevelNumber, pArg, pOwner);
	}

	template<typename T>
	_string Asset_Description(_uint iLevel, const _char* pComponentName, CComponent** ppOut, void* pDesc, class CGameObject* pOwner = nullptr, _wstring wstrGroupName = L"")
	{
		return m_pPrototype_Manager->CPrototype_Manager::Asset_Description<T>(iLevel, pComponentName, ppOut, pDesc, pOwner, wstrGroupName);
	}

#pragma endregion

#pragma region OBJECT_MANAGER
	template<typename T>
	HRESULT Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr, class CGameObject* pOwner = nullptr, T** ppOut = nullptr) {

		T* pGameObject = Clone_Prototype<T>(iPrototypeLevelIndex, pArg, pOwner);
		if (nullptr == pGameObject) {
			return E_FAIL;
		}
		HRESULT hr = m_pObject_Manager->CGameObject_Manager::Add_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, pGameObject);
		if (hr == S_OK && nullptr != ppOut) {
			*ppOut = pGameObject;
		}
		return hr;
	}

	class CLayer* Get_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);
	void Clear_Objects_With_Layers(_uint iLevelIndex);

#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
	void Render_PreShadow(const _float4x4& ViewMatrix, const _float4x4& ProjMatrix);
	RENDER Get_CurrentRenderPass();
	HRESULT Bind_PreShadowMatrix(class CShader* pShader, const _char* pConstants, D3DTS eType);
	HRESULT Bind_PrevMatrix(class CShader* pShader, const _char* pConstants, D3DTS eType);
#pragma endregion

#pragma region KEY_MANAGER
	bool		Key_Pressing(int _iKey);
	bool		Key_Up(int _iKey);
	bool		Key_Down(int _iKey);
	_bool		Mouse_Pressing(int _iKey);
	_bool		Mouse_Up(int _iKey);
	_bool		Mouse_Down(int _iKey);
	_bool		Mouse_StartMove();
	_bool		Mouse_Moving();
	_bool		Mouse_StopMove();
#pragma endregion

#pragma region MOUSE_MANAGER
	POINT	Get_MouseViewPortPos();
	_bool	Toggle_MouseCenter();
	_float3	Get_MouseMove();
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	void    Picking();
	HRESULT Ray_WorldToLocal(_fmatrix* InvWorldMatrix);
	_bool   MousePicking_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3& pOut);
	_bool   MousePicking_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3& pOut);
	//void Mouse_Render();
#pragma endregion

#pragma region PIPELINE
	void Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix);
	const _float4x4* Get_Transform_Float4x4(D3DTS eState);
	_matrix Get_Transform_Matrix(D3DTS eState);
	_matrix Get_ShadowTransform_Matrix(D3DTS eState, SHADOW eShadowType);
	const _float4* Get_CamPosition();
	const _vector Get_CamXMPosition();
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool IsIn_WorldFrustum(_fvector vWorldPos, _float fRadius);
	_bool IsIn_LocalFrustum(_fvector vLocalPos, _float fRadius);
	pair<_bool, _ubyte> IsIn_ShadowViewFrustum(_fvector vWorldCenter, _float fRadius);
	HRESULT Bind_CascadeSplitRatio(class CShader* pShader, const _char* pConstantName, _bool bNear);
	HRESULT Bind_CascadeBias(class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_GlobalSRV(class CShader* pShader, const _tchar* wszKeyGlobalSRV, const _char* pConstantName);
	HRESULT Load_GlobalSRV(const _tchar* wszKeyGlobalSRV, filesystem::path pathSRVFolder);

	HRESULT Ready_Shadow_Light(const _float4& vShadowDirRPYQuat);
	HRESULT Bind_Shadow_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType, SHADOW eShadowType) const;
	const _float4x4* Get_ShadowMatricesPtr(_uint iShadowBoxIndex);
	_float  Get_ShadowBoxFar(_uint iShadowBoxIndex);
	HRESULT Begin_OutLine_Write(_uint iDSSMask);
	HRESULT End_OutLine_Write();
#pragma endregion
#pragma region LIGHT_MANAGER
	void			  Add_Light(_uint _iCurrentLevel, class CLight* _pLight);
	void			  Add_Light_Group(_uint _iCurrentLevel, class CLight* _pLight);
	void			  Delete_Light(_uint _iCurrentLevel, class CLight* _pLight);
	const LIGHT_DESC* Get_Light_Info(_uint _iCurrentLevel, _uint _iLightIndex);
	HRESULT			  Render_Lights(_uint _iCurrentLevel, class CShader* pShader, class CVIBuffer* pVIBuffer);
	list<class CLight*>* Get_LightList(_uint _iCurrentLevel);
	_uint				Get_NumLight(_uint _iCurrentLevel);
#pragma endregion
#pragma region COLLIDER_MANAGER
	HRESULT Add_ColliderGroup(_uint iColliderGroup, class CCollider* pBounding);
	void	Refresh_Collider_Manager();
	void	Collide(_uint iCollideGroupA, _uint iCollideGroupB);
#pragma endregion
#pragma region OBSTACLE_MANAGER
#pragma endregion
#pragma region RENDERTARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strRenderTargetKey, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMultiRenderTargetKey, const _wstring& strRenderTargetKey);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT Begin_MRT_NonClear(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT Begin_MRT_Include_BackBuffer(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr , _bool isClear = true);
	HRESULT Begin_MRT_NO_DepthStencil(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RenderTarget(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RenderTargetTo(const _wstring& strSrcTag, ID3D11Texture2D* pDst2D);
	HRESULT Copy_RenderTargetFrom(const _wstring& strDstTag, ID3D11Texture2D* pSrc2D);
	HRESULT Copy_RenderTargetAToB(const _wstring& strATag, const _wstring& strBTag);
	HRESULT Accumulate_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTarget_SrcA, const _wstring& wstrRenderTarget_SrcB, const _wstring& wstrRenderTarget_Target, SHADER_PASS_DEFERRED ePass);
	HRESULT Refit_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTargetInput, const _wstring& wstrRenderTargetOutput, SHADER_PASS_DEFERRED ePass);
	HRESULT Finish_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTargetOriginal, const _wstring& wstrRenderTargetBloomed, SHADER_PASS_DEFERRED ePass);
	HRESULT Bind_CS_RenderTarget(_uint iIndex, const _wstring& strTargetTag);
	HRESULT Clear_RenderTarget(const _wstring& strRenderTargetKey);
	ID3D11ShaderResourceView* Get_RenderTarget_SRV(const _wstring& strRenderTargetKey);
#ifdef _DEBUG
	void    RenderTarget_Debuger();
	HRESULT Render_RenderTarget_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG
#pragma endregion
#pragma region CAMERA_MANAGER
	HRESULT Clear_Cameras(_uint iLevel);
	HRESULT Release_Camera(_uint iLevel, const _wstring& wstrCameraKey);
	HRESULT Add_Camera(_uint iLevel, class CCamera* pCamera, const _wstring& strCameraKey);
	HRESULT Bind_Camera(_uint iLevel, const _wstring& strCameraKey, _bool bIgnorePriority);
	HRESULT IsBinded_Camera(const _wstring& strCameraKey);
	_vector Get_CameraLook();
	_float	Get_CameraFov();
	_float	Get_CameraNear();
	const _float* Get_CurrentCameraFar();
	void Force_CamPosition(_fvector vPos);
#pragma endregion
#pragma region SOUND_MANAGER
#pragma endregion

#pragma region PICKING
	_bool	isPicking(_float3* pOut);
#pragma endregion
#pragma region PhysX_Manager
	PSX::PxMaterial* Create_Material(_float3* vMatInfo);
	void RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh, _uint iLevel);
	void RegistHeight(const _tchar* pName, PSX::PxHeightFieldDesc& Desc, _uint iLevel);
	PSX::PxRigidDynamic* Add_DynamicActor(CRigidBody_Dynamic& RigidBody, _uint iLevel);
	PSX::PxRigidStatic* Add_StaticActor(CRigidBody_Static& RigidBody, _uint iLevel);
	PSX::PxJoint* Create_PxJoint(PHYSX_JOINT eType, PSX::PxRigidActor* pActor0, PSX::PxTransform& pxLocalFrame0, PSX::PxRigidActor* pActor1, PSX::PxTransform& pxLocalFrame1);
	PSX::PxD6Joint* Create_PxD6Joint(PSX::PxRigidDynamic* pActor0, PSX::PxRigidDynamic* pActor1, const PSX::PxTransform& pxJointWorldPos);

	_bool SphereCast(_float fRadius, _float3 vStartPos, _float3 vDir, _float fDistance, PSX::PxHitFlags flagHitsData, PSX::PxQueryFlags flagQuery, PSX::PxSweepBuffer& hitBuffer);
	_bool SphereCast(_float fRadius, _fvector vStartPos, _gvector vDir, _float fDistance, PSX::PxHitFlags flagHitsData, PSX::PxQueryFlags flagQuery, PSX::PxSweepBuffer& hitBuffer);
	_bool RayCast(_float3 _vStartPos, _float3 _vDir, _float fDistance, PSX::PxRaycastHit* pRayHitArray, _uint iMaxHitCapacity, _uint& iOutHitCount);
	_bool RayCast(_fvector _vStartPos, _gvector _vDir, _float fDistance, PSX::PxRaycastHit* pRayHitArray, _uint iMaxHitCapacity, _uint& iOutHitCount);

	PSX::PxController*	Add_CapsuleController(PSX::PxCapsuleControllerDesc& Desc);
	PSX::PxController*	Add_BoxController(PSX::PxBoxControllerDesc& Desc);
	PSX::PxController*	Get_Controller(_uint iControllerIndex);
	void				ReleaseController(_uint iControllerIndex);
	void				Attach_Actor(PSX::PxActor& Actor, _uint iLevel);
	void				Detach_Actor(PSX::PxActor& pActor, _uint iLevel);
	void				Release_Actor(PSX::PxActor& Actor);
	void				ApplyFilterData(PSX::PxRigidActor* pRigidActor);
	HRESULT ConvertToTriMeshes(vector<class CMesh*>& Meshes, vector<class PSX::PxTriangleMesh*>& pxTriMeshes, _fmatrix WorldMatrix = XMMatrixIdentity());
#ifdef EDITOR_PROJECT
	HRESULT SaveTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes);
#endif // EDITOR_PROJECT
	HRESULT LoadTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes); // 모델 불러왔던 경로에 그대로 있음
	void Add_Editor_Plane(PHYSX_USERDATA& PlaneData);
#pragma endregion
#pragma region THREADHOLDER
	template<class Function, class... Args>
	future<invoke_result_t<Function, Args...>> EnqueueJob(Function&& func, Args&&... args)
	{
		return m_pThreadHolder->EnqueueJob(
			forward<Function>(func),
			forward<Args>(args)...
		);
	}
#pragma endregion

#pragma region FOG
	void	Set_Fog(_float fFogDensity, _float fPow);
	void	Set_FogColor(_float4& vFogColor);

	HRESULT Bind_FogValue(class CShader* pShader);
#pragma endregion

#pragma region RESOURCE_MANGER
	public:
		ID3D11ShaderResourceView* Add_Resource(const _char* pFilePath, _uint iLevel = 0);
		void Clear_LevelResources(_uint iLevel);
#pragma endregion

#pragma region FONT_MANAGER
	public:
		HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
		HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float vScale = 1.f);
		HRESULT Render_Rotation_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float Rotation = 0.f, _float vScale = 1.f);
		HRESULT Perspective_Render_Text(_matrix View, _matrix Proj, const _wstring& strFontTag, const _tchar* pText, const _fvector& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float vScale = 1.f);
		_float FontSizeX(const _wstring& strFontTag, const _tchar* pText);
#pragma endregion

#pragma region VOLUMETRIC
		ID3D11ShaderResourceView*	Get_VolumeSRV();
		_float*						Get_DepthPackExponentPtr();
		void						Setting_Volumetirc(_float fDensity, _float fLightIntensity, _float fAsymmetryParameter, _float fDepthPackExponent);
		void						Update_Volumetric();
#pragma endregion


public:
	void Add_ModelToMap(const _char* filePath, CModel* pModel);

	void Add_SaveModel(const _char* filePath, SaveModel& sModel);

	SaveModel* Load_SaveModel(const _char* filePath);
#ifdef EDITOR_PROJECT
	_bool SaveAssimpModel(const _char* filename);
	void Save_ModelFilePath(const _char* FilePath);
	const _char* Load_ModelFilePath(_uint iIndex);
	const _char* Load_BinaryModelFilePath(_uint iIndex);
	size_t BinaryModelFilePathCount();
	size_t ModelFilePathCount();
#endif

private:
	_float Update_SlowMotion(_float fTimeDelta);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*		m_pPrototype_Manager = { nullptr };
	class CGameObject_Manager*		m_pObject_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	class CCamera_Manager*			m_pCamera_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CMouse_Manager*			m_pMouse_Manager = { nullptr };
	class CCollider_Manager*		m_pCollider_Manager = { nullptr };
	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CThreadHolder*			m_pThreadHolder = { nullptr };
	class CFog*						m_pFog = { nullptr };
	class CResource_Manager*		m_pResource_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CVolumetric*				m_pVolumetric = { nullptr };

	mt19937 m_Rng{ random_device{}() };
	_float2							m_vViewPortSize = {};

private:
	_bool							m_isSlowMotion = {};
	_float2							m_vSlowTime = {};
	_float							m_fSlowIntense = {};
#ifdef _DEBUG
private:
	_float							m_fTimer_PriorityUpdate = { 0.f };
	_float							m_fTimer_Update = { 0.f };
	_float							m_fTimer_LateUpdate = { 0.f };
	_float							m_fTimer_DrawCall = { 0.f };
	_float							m_fTimer_Present = { 0.f };
	_float							m_fTimer_Picking = { 0.f };
	_float							m_fTimer_PhysX = { 0.f };
	_float							m_fTimer_Level = { 0.f };
	_float							m_fTimer_FrameCount = { 0.f };
	
	_float							m_fTimer_Render_Priority = { 0.f };
	_float							m_fTimer_Render_Shadow = { 0.f };
	_float							m_fTimer_Render_NonBlend = { 0.f };
	_float							m_fTimer_Render_SSAO = { 0.f };
	_float							m_fTimer_Render_SSAO_BLUR = { 0.f };
	_float							m_fTimer_Render_LightAcc = { 0.f };
	_float							m_fTimer_Render_Blur = { 0.f };
	_float							m_fTimer_Render_Combined = { 0.f };
	_float							m_fTimer_Render_Occlusion = { 0.f };
	_float							m_fTimer_Render_EnvironmentPostProcess = { 0.f };
	_float							m_fTimer_Render_Fog = { 0.f };
	_float							m_fTimer_Render_Effect = { 0.f };
	_float							m_fTimer_Render_NonLight = { 0.f };
	_float							m_fTimer_Render_Blend = { 0.f };
	_float							m_fTimer_Render_WeightBlend = { 0.f };
	_float							m_fTimer_Render_PostProcessing = { 0.f };
	_float							m_fTimer_Render_LastColor = { 0.f };
	_float							m_fTimer_Render_Tone_Mapping = { 0.f };
	_float							m_fTimer_Render_UI = { 0.f };
	_float							m_fTimer_Render_UI_Overley = { 0.f };

	vector<const _char*>			    m_FilePaths = {};
	map<const _char*, CModel*>			m_ModelMap;

#endif // _DEBUG
	map<const _char*, SaveModel>		m_sModelMap;
public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END
