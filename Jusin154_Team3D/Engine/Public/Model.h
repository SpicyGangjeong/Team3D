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
		_float Speed;
		_int BoneCount;
		_float4x4 PreTransformMatrix;

	}ANIMSTATE_DESC;

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
#pragma endregion 
#pragma region Animation
	_bool	Play_Animation(_float fTimeDelta, class CTransform* pTransform = nullptr); // 애니메이션에 델타타임을 넣어줌
	_bool	Play_Anim(_float fTimeDelta, CTransform* pTransform);
	_bool	Play_Dual_Anim(_float fTimeDelta, CTransform* pTransform);
	void	Set_AnimationIndex(_uint iIndex, _bool isLoop = true, _float fAmount = 1.f, _bool bRatio = false,_float fAnimSpeed = 1.f);
	void	Set_Second_AnimationIndex(_uint BoneIndex,_uint iIndex, _bool isLoop = false);
	_bool	IsFinishedAnim() const { return m_bIsFinishedAnim; }
	_bool	IsFinishedSecondAnim() const { return m_bIsSecondFinishedAnim; }
	_bool	IsLoopAnim() const { return m_bIsLoop; }
	void	Set_CurrentTrackPosition(_float TrackPosition);
	const _char* Get_AnimList(_uint iIndex);
	_float Get_CurrentTrackPosition();
	_float Get_CurrentTrackProgressRatio();
	_float Get_TrackProgressRatio(_uint iIndex);

	size_t Get_AnimSize() { return m_Animations.size(); }
	void Set_PlayAnim(_bool bPlayAnim) { m_bPlayAnim = bPlayAnim; }
	_bool Get_PlayAnim() const { return m_bPlayAnim; }
	_int Get_AnimIndex() const { return m_iCurrentAnimIndex; }
	_int Get_SecondAnimIndex() const { return m_iCurrSecondAnimIndex; }
	_float Get_AnimSpeed();
	void Set_AnimSpeed(_float fSpeed);

	HRESULT Anim_Event(_float fRatio, _uint AnimIndex, function<void()> Event);

	void Set_BlendDuration(_float Duration) { m_fBlendDuration = Duration; }
#pragma endregion
#pragma region Mesh
	const _char* Get_MeshName(_uint iIndex);
	_uint	Get_NumMeshes() const { return m_iNumMeshes; }
	void Update_RootBone(_float Amount = 1.f);
	void Initialize_RootBone();
#pragma endregion
#pragma region Bone
	HRESULT					Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	_matrix Get_BoneMatrix(const _char* pBoneName) const;
	_matrix Get_BoneLocalMatrix(const _char* pBoneName) const;
	static _int				Get_BoneIndex(const _char* pBoneName, vector<class CBone*> Bones);	// 본의 벡터와 이름을 넘겨주면 인덱스를 넘겨줌 ( n 순회 )
	_int					Get_BoneIndex(const _char* pBoneName) const;
	_matrix					Get_BoneMatrix(_uint iBoneIndex);
	void					Combined_BoneMatrix();


#pragma endregion
#pragma region Material
	HRESULT Bind_Material(_uint iMeshIndex, class CShader* pShader);

#pragma endregion

	void			ComputeAnimation(_uint AnimIndex);
	void			ComputeAnimation_Second(_uint AnimIndex);
	void			InItialize_BoneIndex();

	void			Initialize_BoneMasks();

public:
	HRESULT Ready_PhysXMeshes(_fmatrix& PreTransformMatrix);
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
	_int SaveNodeRecursive(const aiNode* pAINode, std::vector<SaveNode>& outNodes, _int parentIndex);
	//
#endif // EDITOR_PROJECT


private:
	MODEL						m_eType = {};						// 모델의 타입

	_float4x4					m_PreTransformMatrix = {};			// 사전에 루트본에 곱해줄 매트릭스
	vector<class CMesh*>		m_Meshes;							// 메쉬의 벡터
	vector<PSX::PxTriangleMesh*> m_TriMeshes = {};			// 피직스 메쉬의 갯수
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
	class CTransform* m_pTransform = { nullptr };			// 모델 대상의 트랜스폼

	_float						m_fRatio = {};
	_int						m_iCurrSecondAnimIndex = { -1 };
	_int						m_iBoneIndex[ENUM_CLASS(BLEND_BONE::END)] = { -1,-1,-1,-1,-1,-1 };
	vector<vector<_uint>>		m_BoneMask;



	// 바이너리
	SaveModel* m_pSaveModel = { nullptr };
	list<SaveModel> m_SaveModel;
	//

	vector<_float4x4> m_BoneMatrix;

	_float3					m_vPrevRootPos = { 0.f, 0.f, 0.f };
	_float4					m_vPrevRootRot = { 0.f, 0.f, 0.f,0.f };
	_matrix					m_BoneTransformationMatrix = {};
	_float4					m_vInitialRootRot = {};

	_float						m_fRadius = { 0.f };			// 컬링용 Radius
	_int						m_iRootBoneIndex = { -1 };			// 루트본의 인덱스
	_vector					m_vector[3];

	vector<_uint>			m_iBoneMask;
	_bool					m_bInitialRootPos = { false };
	_bool					m_bInitialRootRotSaved = { false };
	_bool					m_bLoopRestarted = false;
	_bool						m_bIsSecondFinishedAnim = { false };
	_bool						m_bIsSecondLoop = { false };
	_bool m_bRatio = { false };

private:
#ifdef EDITOR_PROJECT
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
#endif


#pragma region Compute

private:
	HRESULT			Create_ParentVB();
	void			Create_Temp();
	void			Create_LocalPosVB();

	_uint					m_iNumBuffer = {};

	vector<PARENT_DESC>		m_Parent = {};
	vector<ANIMSTATE_DESC> m_AnimRanges;

	LOCALPOS_DESC* m_pLocalPos[2] = { nullptr };

	class CComputeShader* m_pComputeShader = nullptr;

	ID3D11Buffer* m_pConstantBuffer = { nullptr };

	ID3D11Buffer* m_pParentBuffer = { nullptr };
	ID3D11Buffer* m_pLocalPosBuffer = { nullptr };
#pragma endregion

private:
	// 바이너리
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	void LoadAdditionalAnimations(const char* ModelFilePath);
	bool LoadData(const _char* filename);
	void LoadAnim(const _char* fileName);
	//
	virtual HRESULT Initialize(void* pArg) override;

private:
	// 바이너리
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const std::vector<SaveNode>& allNodes, _int currentIndex, _int parentIndex);
	HRESULT Ready_Animations(const vector<CBone*>& Bones);
	//

public:
	// 바이너리
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	//
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr);

	virtual void Free(); 
public:
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
