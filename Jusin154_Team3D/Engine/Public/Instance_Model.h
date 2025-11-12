#pragma once

#include "Component.h"
#include "Assimp/scene.h"

NS_BEGIN(Engine)

class ENGINE_DLL CInstance_Model final : public CComponent
{
public:
	typedef struct tagInstanceDesc
	{
		_int		iNumInstance = {}; // 인스턴스 개수
		_float2		vLifeTime = {}; // 라이프 타임 min , max
		_float3		vSizeMin = {}; // 사이즈 min xyz
		_float3		vSizeMax = {}; // 사이즈 max xyz
		_float3		vRotationAngleMin = {};  // 로테이션 min x y z for angle
		_float3		vRotationAngleMax = {};// 로테이션 max x y z for angle
		_float2		vRotationSpeed = {}; // 로테이션 스피드 min , max
		_float2		vSpeed = {}; // 스피드 min , max
		_float3		vCenter = {}; // 센터 위치 
		_float3		vRange = {}; // 위치 범위 

		_float2		vMaskingUVMoveTime = {};
		_float2		vDiffuseUVMoveTime = {};
		_float2		vAniTime = {};
		_float2	    vAniIndex = {};

	}INSTANCE_DESC;

	typedef struct tagCSParticleDesc
	{
		_float   fTimeDelta = {};
		_float   fPadding = {};
		_float	 fPadding2 = {};
		_float   fPadding3 = {};

	}CS_PARTICLE_DESC;

	typedef struct tagCSParticleValueDesc
	{
		_float   fSpeed = {};
		_float	 fRotaionSpeed = {};

		_float4  vOriginRight = {};
		_float4  vOriginUp = {};
		_float4  vOriginLook = {};
		_float4  vOriginTranslation = {};

		_float2	 vMaskingUVMoveTime = {};
		_float2	 vDiffuseUVMoveTime = {};
		_float2	 vAniTime = {};

		_float2	 vAniIndex = {};

	}CS_PARTICLE_VALUE_DESC;


	struct ParticleOut
	{
		_float4 vRight = {};
		_float4 vUp = {};
		_float4 vLook = {};
		_float4 vTranslation = {};
		_float2 vLifeTime = {};

		_float2 vMaskingUVMoveTime = {};
		_float2 vDiffuseUVMoveTime = {};
		_float2 vAniTime = {};

		_float2 vAniIndex = {};

	}CS_PARTICLE_OUT;
public:
	CInstance_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_Model(const CInstance_Model& rhs);
	virtual ~CInstance_Model() = default;

public:
#ifdef EDITOR_PROJECT
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);

#endif	

	virtual HRESULT Initialize(void* pArg) override;
	void			Drop(_float fTimeDelta);
public:
	 HRESULT		Render(_uint iMeshIndx);
	 void			Instane_Buffer_ReStruct();
	 _uint			Get_NumMeshes() const { return m_iNumMeshes; }
	 HRESULT		Bind_CS_Output(_uint Index);
private:
#ifdef EDITOR_PROJECT
	HRESULT			Assimp_Model_Load(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);
	HRESULT			Ready_Meshes(MODEL eType, const aiScene* pAIScene, _fmatrix& PreTransformMatrix);
#endif	
	HRESULT			Create_Instance_Buffer(const INSTANCE_DESC* pDesc);
	HRESULT         Create_SubResource_Buffer();

private:
	ID3D11Buffer*			m_pVBInstance = { nullptr };

	INSTANCE_DESC			m_InstanceDesc = {};

	_uint					m_iInstanceStride = {};
	_uint					m_iNumInstance = {};

	_uint					m_iNumBuffer = {};
	_uint					m_iNumMeshes = {};
	_int					m_iRootBoneIndex = { -1 };

	MODEL					m_eType = {};					
	_float4x4				m_PreTransformMatrix = {};

	vector<class CMesh*>	m_Meshes = {};
	vector<class CBone*>	m_Bones = {}; // 나중에 혹시 애님메쉬를 인스턴싱 할 일이 있을지도 모르니 남겨놓음

private:
	class CComputeShader*	m_pComputeShader = {};
	ID3D11Buffer*			m_pConstantBuffer = { nullptr }; // 컴퓨트 쉐이드 전용 상수버퍼
	ID3D11Buffer*			m_pParticleValueBuffer = { nullptr }; // 컴퓨트 쉐이드 두번째 버퍼 (스피드, 로테이션)

public:
#ifdef EDITOR_PROJECT	
	static CInstance_Model*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);
#endif	
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr);
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END