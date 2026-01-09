#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	typedef struct tagAnimStateDesc
	{
		_float CurrentTime;
		_float Duration;
		_int MeshBoneCount;
		_int BoneCount;

		_int CurrentAnimIndex;
		_int PrevAnimIndex;
		_float PrevTime;
		_float BlendRatio;

		_int RootBoneIndex;
		_int PlayHeadBone;
		_int   HeadBoneIndex;
		_float HeadAimWeight;

		_int SkipCount;
		_int IsSkip;
		_int padding1;
		_int padding2;

		_float3 TargetDir_Local;
		_float padding3;

		_float4x4 PreTransformMatrix;
		_float4 RootInitRot;

		_int     UseUpperBody;         
		_float   UpperBlend;           
		_int   SecondAnimIndex;
		_float SecondAnimTime;
	}ANIMSTATE_DESC;

	typedef struct tagBoneInsertionDesc {
		_uint iStartOffset;
		_uint iMatrixCount;
		_uint iPad0;
		_uint iPad1;
	}BONEINSERTION_DESC;


private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
#pragma region Model
	_float		Get_Radius() const { return m_fRadius; }
	_float3& Get_RadiusOffset() { return m_vRadiusOffset; }
	_float4x4* Get_PreTransformMatrixPtr() { return &m_PreTransformMatrix; }
	_float4x4	Get_PreTransformMatrix() const { return m_PreTransformMatrix; };
	virtual HRESULT Render(_uint iMeshIndex);
	virtual HRESULT Render_Indexed(_uint iMeshIndex, _uint IndexCount, _uint StartIndexLocation, _uint BaseVertexLocation);
	MODEL Get_Type() { return m_eType; }
	void Set_DisableRootMotionScale(_bool Disable) { m_bDisableRootMotionScale = Disable; }
	void Set_TargetPos(_float4 vTargetPos) { m_vTargetPos = vTargetPos; }
	void Set_HeadPos(_float4 vHeadPos) { m_vHeadPos = vHeadPos; }
	void Play_HeadBone(_bool bPlay) { m_bHeadBone = bPlay; }
	void Set_HeadAimWeight(_float fWeight) { m_fHeadAimWeight = fWeight; }
	_float Get_HeadAimWeight() { return m_fHeadAimWeight; }
	_bool Get_PlayHeadBone() const { return m_bHeadBone; }
	_int Get_SkipBoneIndex(_int Index) { return m_SkipBoneindex[Index]; }
	vector<_uint> Get_BoneMask(_int iIndex){ return m_BoneMask[iIndex]; }
#pragma endregion 
#pragma region Animation
	_bool			Play_Animation(_float fTimeDelta, class CTransform* pTransform = nullptr); // 애니메이션에 델타타임을 넣어줌
	_bool			Play_Anim(_float fTimeDelta, CTransform* pTransform);
	_bool			Play_Dual_Anim(_float fTimeDelta, CTransform* pTransform);
	_bool			IsBlending() const;
	void			Set_AnimationIndex(_uint iIndex, _bool isLoop = true, _float fAmount = 1.f, _bool bRatio = false, _float fAnimSpeed = 1.f, _bool bRootBone = true, _bool bQueuedAnim = false, _bool bInit = true);
	void			Set_Second_AnimationIndex(_uint BoneIndex, _uint iIndex, _bool isLoop = false);
	_bool			IsFinishedAnim() const { return m_bIsFinishedAnim; }
	_bool			IsFinishedSecondAnim() const { return m_bIsSecondFinishedAnim; }
	_bool			IsLoopAnim() const { return m_bIsLoop; }
	void			Set_CurrentTrackPosition(_float TrackPosition);
	const _char*	Get_AnimList(_uint iIndex);
	_float			Get_CurrentTrackPosition();
	_float			Get_CurrentTrackProgressRatio();
	_float			Get_TrackProgressRatio(_uint iIndex);

	size_t			Get_AnimSize() { return m_Animations.size(); }
	void			Set_PlayAnim(_bool bPlayAnim) { m_bPlayAnim = bPlayAnim; }
	_bool			Get_PlayAnim() const { return m_bPlayAnim; }
	_int			Get_AnimIndex() const { return m_iCurrentAnimIndex; }
	_int			Get_SecondAnimIndex() const { return m_iCurrSecondAnimIndex; }
	_float			Get_AnimSpeed();
	void			Set_AnimSpeed(_float fSpeed);

	HRESULT			Anim_Event(_float fRatio, _uint AnimIndex, function<void()> Event);

	void			Set_BlendDuration(_float Duration) { m_fBlendDuration = Duration; }

	_float4			Get_RootBoneMomentum() { return m_RootBoneMomentum; }

	void			Start_ChainAnimation();
	void			Update_ChainAnimation();
	void			Set_ChainAnimation(pair<_uint, _bool> chainAnim);
	void			IsRootBone(_bool bRootBone) { m_bRootBone = bRootBone; }
#pragma endregion
#pragma region Mesh
	const _char*		Get_MeshName(_uint iIndex);
	HRESULT				Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT				Capture_BoneMatrices(_uint iMeshIndex, vector<_float4x4>& pDestination);
	_int				Get_MeshInfluencedBoneNum(_uint iMeshIndex);
	_uint				Get_NumMeshes() const { return m_iNumMeshes; }
	void				Update_RootBone(_float Amount = 1.f);
	void				Initialize_RootBone();
	vector<_float4x4>	Get_OffsetMatrix(_int iIndex);
	void				Apply_CPU_HeadAim();
	void				Apply_CPUMask_ToBones();
	void				Mark_CPUChain(_int boneIdx);
	class CMesh*		Get_Mesh(_uint iIndex) { return m_Meshes[iIndex]; }
#pragma endregion
#pragma region Bone
	const _float4x4*	Get_BoneMatrixPtr(const _char* pBoneName);
	_matrix				Get_BoneMatrix(const _char* pBoneName);
	_matrix				Get_BoneMatrix(const _char* pBoneName) const;
	const _float4x4*	Get_BoneLocalMatrixPtr(const _char* pBoneName);
	_matrix				Get_BoneLocalMatrix(const _char* pBoneName) const;
	static _int			Get_BoneIndex(const _char* pBoneName, vector<class CBone*> Bones);	// 본의 벡터와 이름을 넘겨주면 인덱스를 넘겨줌 ( n 순회 )
	_int				Get_BoneIndex(const _char* pBoneName) const;
	_matrix				Get_BoneMatrix(_uint iBoneIndex);
	void				Combined_BoneMatrix();
	void				Combined_BoneMatrix(_int iStartBoneIndex, _uint iBoneCount);
	_int				Find_BoneIndex(const _char* pBoneName);
	_uint				Get_BoneAbsoluteCount();
	HRESULT				Set_BoneCombinedTransformation(const _char* pBoneName, _fmatrix newTransformation);
	HRESULT				Capture_BoneBuffer(class CMotion_Trail* pTrail, const _float4x4& CurrentWorldMatrix);
	void				IsSkip(_bool bSkip) { m_IsSkip = bSkip; }
//	HRESULT				Set_BoneLocalTransformationMatrix(const _char* pBoneName, _fmatrix newTransformation);

#pragma endregion
#pragma region Material
	HRESULT					Bind_Material(_uint iMeshIndex, class CShader* pShader);
	HRESULT					Begin(_uint iMeshIndex, class CShader* pShader);
	_int					Get_UsingPass(_uint iMeshIndex, CShader* pShader);
#pragma endregion

	void					ComputeAnimation(_uint AnimIndex, _uint MeshIndex);
	void					ComputeLocal(_uint AnimIndex, _uint MeshIndex);
	void					ComputeInsertionBoneBuffer(BONEINSERTION_DESC& CBDesc, ID3D11ShaderResourceView* pSRV);
	void					Bind_OutPut_SRV_VS(_uint iIndex, _uint iBufferIndex);
	void					Bind_OutPut_SRV_VS_Prev(_uint iIndex, _uint iBufferIndex);
	void					ComputeAnimation_Second(_uint AnimIndex);
	void					InItialize_BoneIndex();

	void					Initialize_BoneMasks();

public:
	HRESULT Ready_PhysXMeshes(_fmatrix& PreTransformMatrix, _uint iLevel);
#ifdef EDITOR_PROJECT	
	HRESULT Assimp_Model_Load(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);
	HRESULT Ready_Meshes(MODEL eType, const aiScene* pAIScene, _fmatrix& PreTransformMatrix);
	HRESULT Save_PhysXTriMeshes(const _char* pModelFilePath);
	HRESULT Ready_Materials(const aiScene* pAIScene, const _char* pModelFilePath);
	HRESULT Ready_Materials_FromFile(const aiScene* pAIScene, const _char* pModelFilePath);
	HRESULT Ready_Materials_Independent(MODEL eType, const aiScene* pAIScene, const _char* pModelFilePath);
	HRESULT Ready_Materials_FromFile_Anim(const aiScene* pAIScene, const _char* pModelFilePath);
	HRESULT Ready_Animations(const aiScene* pAIScene);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	// 바이너리
	_bool SaveAssimpModel(const _char* filename);
	_int SaveNodeRecursive(const aiNode* pAINode, vector<SaveNode>& outNodes, _int parentIndex);
	//
	_bool PickingMesh(_float3* pPosition, _float2* pUV, _uint iMeshIndex, _fmatrix WorldMatrix);
#endif // EDITOR_PROJECT


private:
	MODEL						m_eType = {};						// 모델의 타입

	_float4x4					m_PreTransformMatrix = {};			// 사전에 루트본에 곱해줄 매트릭스
	vector<class CMesh*>		m_Meshes;							// 메쉬의 벡터
	vector<PSX::PxTriangleMesh*>m_TriMeshes = {};			// 피직스 메쉬의 갯수
	_uint						m_iNumPhysXMeshes = {};				// 피직스 메쉬의 갯수
	_uint						m_iNumMeshes = {};					// 메쉬의 갯수
	_uint						m_iNumMaterials = {};				// 머테리얼의 갯수
	_float						m_fAmount = { 1.f };
	vector<class CMaterial*>	m_Materials;						// 머테리얼의 벡터

	vector<class CBone*>		m_Bones;							// 본의 벡터, 정렬순서는 루트본부터 트리의 마지막 노드까지

	_int						m_iCurrentAnimIndex = { -1 };		// 현재 선택된 애니메이션 ( m_Animations의 인덱스 )
	_uint						m_iNumAnimations = {};				// 애니메이션의 종류
	_bool						m_bIsLoop = { false };				// 루프애니메이션인지
	_bool						m_bIsFinishedAnim = { false };			// 대상 애니메이션이 끝났는지
	_bool						m_bIsFinishedLerp = { false };			// 럴프 애니메이션이 끝났는지
	_bool						m_bPlayAnim = { true };

	_int						m_iPreAnimIndex = { -1 };
	_float						m_fBlendTime = { 0.f };
	_float						m_fBlendDuration = { 0.3f };

	_float3						m_vRadiusOffset = {};			// 컬링용 Radius Offset
	_float						m_fSecondBlendTime = {};
	_float						m_fSecondBlendDuration = { 0.3f };
	_float						m_fSecondRatio = {};
	_bool						m_bSecondStopBlend = {};

	vector<class CAnimation*>	m_Animations;						// 애니메이션의 벡터, 

	_matrix						m_PrevAnimationMatrix = XMMatrixIdentity(); // 루트본의 이전 위치
	_matrix						m_DeltaAnimationMatrix = {};		// 루트본의 델타애니메이션
	class CTransform*			m_pTransform = { nullptr };			// 모델 대상의 트랜스폼

	_float						fRatio = {};
	_int						m_iCurrSecondAnimIndex = { -1 };
	_int						m_iBoneIndex[ENUM_CLASS(BLEND_BONE::END)] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	vector<vector<_uint>>		m_BoneMask;
	vector<_bool>				m_CPUBoneMask;

	// 바이너리
	SaveModel*					m_pSaveModel = { nullptr };
	//


	vector<_float4x4>			m_BoneMatrix;

	_float3						m_vPrevRootPos = { 0.f, 0.f, 0.f };
	_float4						m_vPrevRootRot = { 0.f, 0.f, 0.f,0.f };
	_matrix						m_BoneTransformationMatrix = {};
	_float4						m_vInitialRootRot = {};

	_float						m_fRadius = { 0.f };			// 컬링용 Radius
	_int						m_iRootBoneIndex = { -1 };			// 루트본의 인덱스
	_uint						m_iIndexAnimPlayableMesh = { 0 };
	_float4						m_OutScale = {};
	_float4						m_OutRotation = {};
	_float4						m_OutTranslation = {};


	vector<_uint>				m_iBoneMask;
	_bool						m_bInitialRootPos = { false };
	_bool						m_bInitialRootRotSaved = { false };
	_bool						m_bLoopRestarted = false;
	_bool						m_bIsSecondFinishedAnim = { false };
	_bool						m_bIsSecondLoop = { false };
	_bool						m_bRatio = { false };
	_bool						m_bRootBone = {};
	_float4x4					m_RootMatrix = {};
	_bool						m_bDisableRootMotionScale = {false};
	_float4						m_vTargetPos = {};
	_float4						m_vHeadPos = {};
	_bool						m_bHeadBone = { false };
	_float						m_fHeadAimWeight = { 0.f };
	_int						m_iSkipBoneCount = {};
	vector<_int>				m_SkipBoneindex = {};
	_int						m_iQueuedAnimIndex = -1;
	_bool						m_bQueuedLoop = false;
	_bool						m_bQueuedAnim = { false };
	_float4						m_RootBoneMomentum = {};
	_bool						m_IsSkip = {false};
	_bool						m_bSecondAnim = { false };
	_int						m_iMaskBlendIndex = {};
	vector<pair<_uint, _bool>>	m_ChainAnim = {};
	_uint						m_iChainIndex = 0;
	_bool						m_bPlayingChain = false;



private:
#ifdef EDITOR_PROJECT
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
#endif


#pragma region Compute

private:
	HRESULT			Create_ComputeShader();
	HRESULT			Create_ComputeShaderLocal();
	HRESULT			Create_ComputeShaderBoneInsertion();
	HRESULT			Create_ParentVB();
	HRESULT			Create_BoneLocalVB();
	HRESULT			Create_SkipBoneVB();
	HRESULT			Create_BoneMaskBuffers();
	HRESULT			Create_Temp();
	HRESULT			Create_BoneMatrixVB();
	HRESULT			Create_Const();
	HRESULT			Create_ParentSrv();
	HRESULT			Create_BoneLocalSrv();

	HRESULT			Create_Local();

	_uint			m_iNumBuffer = {};

	vector<_int>	m_Parent = {};
	vector<_float4x4>	m_BoneLocal = {};

	vector<ANIMSTATE_DESC> m_AnimRanges;
	vector<_float4x4> m_BoneMatrixCPU;

	class CComputeShader* m_pComputeShader = nullptr;
	class CComputeShader* m_pCS_AnimLocal = nullptr;
	class CComputeShader* m_pCS_BoneInsertion = nullptr;

	ID3D11Buffer* m_pConstantBuffer = { nullptr };
	ID3D11Buffer* m_pParentBuffer = { nullptr };
	ID3D11Buffer* m_pBoneMatrixBuffer = { nullptr };
	ID3D11Buffer* m_pPrevBoneMatrixBuffer = { nullptr };
	ID3D11Buffer* m_pBoneLocalBuffer = { nullptr };
	ID3D11Buffer* m_pSkipBoneBuffer = {  };
	vector<ID3D11Buffer*> m_pMaskBuffers = {nullptr};
	ID3D11Buffer* m_pLocalMatrixBuffer = { nullptr };
	ID3D11Buffer* m_pInsertionCB = { nullptr };

	ID3D11ShaderResourceView* m_pParentSRV = { nullptr };
	ID3D11ShaderResourceView* m_pBoneLocalSRV = { nullptr };
	ID3D11ShaderResourceView* m_pBoneMatrixSRV = { nullptr };
	ID3D11ShaderResourceView* m_pPrevBoneMatrixSRV = { nullptr };
	ID3D11ShaderResourceView* m_pLocalMatrixSRV = { nullptr };
	ID3D11ShaderResourceView* m_pSkipBoneSRV = { nullptr };
	vector<ID3D11ShaderResourceView*> m_pMaskSRVs = {  };

	ID3D11UnorderedAccessView* m_pBoneMatrixUAV = { nullptr };
	ID3D11UnorderedAccessView* m_pLocalMatrixUAV = { nullptr };


#pragma endregion

private:

	// 바이너리
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _uint iLevel);
	void LoadAdditionalAnimations(const char* ModelFilePath);
	bool LoadData(const _char* filename);
	void LoadAnim(const _char* fileName);
	void ParseMaterialXml(const _char* filePath);
	//
	virtual HRESULT Initialize(void* pArg) override;

private:
	// 바이너리
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath, _uint iLevel);
	HRESULT Ready_Bones(const vector<SaveNode>& allNodes, _int currentIndex, _int parentIndex);
	HRESULT Ready_Animations(const vector<CBone*>& Bones);
	//

public:
	// 바이너리
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity(), _uint iLevel = 0);
	//
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr);

	virtual void Free(); 
public:
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
