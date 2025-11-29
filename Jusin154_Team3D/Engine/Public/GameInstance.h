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

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

	_float Random_Normal();
	_float Random_Float(_float fMin, _float fMax);
	_int   Random_Int(_int iMin, _int iMax);
	void BillBoard(CTransform* pTransform);

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
#pragma endregion

#pragma region ASSET_MANAGER
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
	const _float4* Get_CamPosition();
	const _vector Get_CamXMPosition();
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool isIn_WorldFrustum(_fvector vWorldPos, _float fRadius);
	_bool isIn_LocalFrustum(_fvector vLocalPos, _float fRadius);

#pragma endregion
#pragma region LIGHT_MANAGER
	void			  Add_Light(_uint _iCurrentLevel, class CLight* _pLight);
	void			  Delete_Light(_uint _iCurrentLevel, class CLight* _pLight);
	const LIGHT_DESC* Get_Light_Info(_uint _iCurrentLevel, _uint _iLightIndex);
	HRESULT			  Render_Lights(_uint _iCurrentLevel, class CShader* pShader, class CVIBuffer* pVIBuffer);
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
	HRESULT Begin_MRT_Include_BackBuffer(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT Begin_MRT_NO_DepthStencil(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RenderTarget(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D);
	HRESULT Paste_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D);
	HRESULT Accumulate_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTarget_SrcA, const _wstring& wstrRenderTarget_SrcB, const _wstring& wstrRenderTarget_Target, SHADER_PASS_DEFERRED ePass);
	HRESULT Refit_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTargetInput, const _wstring& wstrRenderTargetOutput, SHADER_PASS_DEFERRED ePass);
	HRESULT Finish_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTargetOriginal, const _wstring& wstrRenderTargetBloomed, SHADER_PASS_DEFERRED ePass);
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
	const _float* Get_CurrentCameraFar();
	void Force_CamPosition(_fvector vPos);
#pragma endregion

#pragma region SHADOW
	HRESULT Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc);
	HRESULT Bind_Shadow_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType) const;
	const _float4x4* Get_ShadowMatricesPtr();
	const SHADOW_LIGHT_DESC* Get_ShadowDesc();
#pragma endregion
#pragma region SOUND_MANAGER
#pragma endregion

#pragma region PICKING
	_bool	isPicking(_float3* pOut);
#pragma endregion
#pragma region PhysX_Manager
	PSX::PxMaterial* Create_Material(_float3* vMatInfo);
	void RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh);
	void RegistHeight(const _tchar* pName, PSX::PxHeightFieldDesc& Desc);
	PSX::PxRigidDynamic* Add_DynamicActor(CRigidBody_Dynamic& RigidBody);
	PSX::PxRigidStatic* Add_StaticActor(CRigidBody_Static& RigidBody);
	PSX::PxRevoluteJoint* Create_PxRevoluteJoint(PSX::PxRigidActor* pActorFrame, PSX::PxTransform& pxLocalWallFrame, PSX::PxRigidActor* pActorObject, PSX::PxTransform& pxLocalActorFrame);

	PSX::PxController*	Add_CapsuleController(PSX::PxCapsuleControllerDesc& Desc);
	PSX::PxController*	Add_BoxController(PSX::PxBoxControllerDesc& Desc);
	PSX::PxController*	Get_Controller(_uint iControllerIndex);
	void				ReleaseController(_uint iControllerIndex);
	void				Attach_Actor(PSX::PxActor& Actor);
	void				Detach_Actor(PSX::PxActor& pActor);
	void				Release_Actor(PSX::PxActor& Actor);
#ifdef EDITOR_PROJECT
	HRESULT ConvertToTriMeshes(vector<class CMesh*>& Meshes, vector<class PSX::PxTriangleMesh*>& pxTriMeshes, _fmatrix WorldMatrix = XMMatrixIdentity());
	HRESULT SaveTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes);
#endif // EDITOR_PROJECT
	HRESULT LoadTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes); // 모델 불러왔던 경로에 그대로 있음
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

public:
	void Add_ModelToMap(const _char* filePath, CModel* pModel);

	void Add_SaveModel(const _char* filePath, SaveModel sModel);

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
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*		m_pPrototype_Manager = { nullptr };
	class CGameObject_Manager*		m_pObject_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	class CShadow*					m_pShadow = { nullptr };
	class CCamera_Manager*			m_pCamera_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CMouse_Manager*			m_pMouse_Manager = { nullptr };
	class CCollider_Manager*		m_pCollider_Manager = { nullptr };
	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CThreadHolder*			m_pThreadHolder = { nullptr };

#ifdef _DEBUG
private:
	_float							m_fTimer_PriorityUpdate = { 0.f };
	_float							m_fTimer_Update = { 0.f };
	_float							m_fTimer_LateUpdate = { 0.f };
	_float							m_fTimer_DrawCall = { 0.f };
	_float							m_fTimer_Present = { 0.f };
	_float							m_fTimer_FrameCount = { 0.f };

	vector<const _char*>			m_FilePaths = {};
	map<const _char*, CModel*>			m_ModelMap;

#endif // _DEBUG
	map<const _char*, SaveModel>		m_sModelMap;
public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END
