#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	static _int Get_BoneIndex(const _char* pBoneName, vector<class CBone*> Bones);	// 본의 벡터와 이름을 넘겨주면 인덱스를 넘겨줌 ( n 순회 )
	void	Set_RootBoneIndex(_int iIndex) { m_iRootBoneIndex = iIndex; }
	void	Change_AnimationIndex(_int iAnimationIndex, _bool bIsLoop, _float fLerpDuration, _bool bIgnoreCurrentIndex = false);	// 애니메이션을 다른 인덱스로 변경함
	_bool	Play_Animation(_float fTimeDelta); // 애니메이션에 델타타임을 넣어줌
	void	Stop_Animation(); // 애니메이션을 정지 (초기상태로)
	void	Get_CurrentAnimationState();
	_bool	IsFinishedAnim() const { return m_bIsFinishedAnim; }
	_bool	IsFinishedLerp() const { return m_bIsFinishedLerp; }
	void	RefreshAnim(); // 애님을 현재 애님의 초기상태로 되돌림
	_float	Get_AnimProgressRatio();
	_int	Get_AnimProgressPostion(const char* pAnimChannelName);
	_float	Get_AnimEstimatedDuration();
	void	Set_AnimProgressPostion(const _char* pChannelName, _uint iPosition);
	void	Set_AnimPauseState(_bool bValue);// 애님을 현재상태로 정지
	void	ReallocateResources(class CTransform* pTransform = nullptr); // 애님 이동의 대상 트랜스폼(이동시켜야할 트랜스폼)을 재지정함
	_int	Get_RootBoneIndex() const { return m_iRootBoneIndex; }
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	_uint	Get_NumMeshes() const { return m_iNumMeshes; }
	MODEL	Get_Type() const { return m_eType; }
	_matrix Get_BoneMatrix(_uint iBoneIndex);

	_float4x4* Get_PreTransformMatrixPtr() { return &m_PreTransformMatrix; }
	void Get_BoneMatrices(_float4x4* pOut);
	_uint	Get_BonesNum() const { return (_uint)m_Bones.size(); }

public:
	HRESULT Bind_Material(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName, _uint iType, _uint iTextureIndex);
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	virtual HRESULT Render(_uint iMeshIndex);
	virtual HRESULT Render_Indexed(_uint iMeshIndex, _uint IndexCount, _uint StartIndexLocation, _uint BaseVertexLocation);

#ifdef EDITOR_PROJECT	
	HRESULT Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);
	HRESULT Assimp_Model_Load(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);
	HRESULT Ready_Meshes(MODEL eType, const aiScene* pAIScene, _fmatrix& PreTransformMatrix);
	HRESULT Ready_Materials(const aiScene* pAIScene, const _char* pModelFilePath);
	HRESULT Ready_Animations(const aiScene* pAIScene);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
#endif // EDITOR_PROJECT


private:
	MODEL						m_eType = {};						// 모델의 타입
	_float4x4					m_PreTransformMatrix = {};			// 사전에 루트본에 곱해줄 매트릭스

	_uint						m_iNumMeshes = {};					// 메쉬의 갯수
	vector<class CMesh*>		m_Meshes;							// 메쉬의 벡터

	_uint						m_iNumMaterials = {};				// 머테리얼의 갯수
	vector<class CMaterial*>	m_Materials;						// 머테리얼의 벡터

	vector<class CBone*>		m_Bones;							// 본의 벡터, 정렬순서는 루트본부터 트리의 마지막 노드까지

	_int						m_iCurrentAnimIndex = { -1 };		// 현재 선택된 애니메이션 ( m_Animations의 인덱스 )
	_uint						m_iNumAnimations = {};				// 애니메이션의 종류
	_bool						m_bIsLoop = { false };				// 루프애니메이션인지
	_bool						m_bIsFinishedAnim = { false };			// 대상 애니메이션이 끝났는지
	_bool						m_bIsFinishedLerp = { false };			// 럴프 애니메이션이 끝났는지
	vector<class CAnimation*>	m_Animations;						// 애니메이션의 벡터, 

	class CLerpAnim* m_pLerpAnim = { nullptr };			// 럴프전용 애니메이션

	_int						m_iRootBoneIndex = { -1 };			// 루트본의 인덱스
	_matrix						m_PrevAnimationMatrix = XMMatrixIdentity(); // 루트본의 이전 위치
	_matrix						m_DeltaAnimationMatrix = {};		// 루트본의 델타애니메이션
	class CTransform* m_pTransform = { nullptr };			// 모델 대상의 트랜스폼


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr);
virtual void Free(); public:
	void Describe_Entity() override;
};

NS_END