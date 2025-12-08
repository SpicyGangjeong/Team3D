#pragma once

#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CInstance_Model final : public CComponent
{
public:
	typedef struct tagInstanceDesc
	{
		_int		iNumInstance = { 1 }; // 인스턴스 개수

		_bool		isLoop = { true };
		_bool		isDrop = { false };

		_float2		vLifeTime = { 1.f , 1.f }; // 라이프 타임 min , max
		_float3		vSizeMin = { 1.f, 1.f ,1.f }; // 사이즈 min xyz
		_float3		vSizeMax = { 1.f ,1.f ,1.f }; // 사이즈 max xyz
		_float3		vRotationAngleMin = {};  // 로테이션 min x y z for angle
		_float3		vRotationAngleMax = {};// 로테이션 max x y z for angle
		_float2		vRotationSpeed = {}; // 로테이션 스피드 min , max
		_float2		vSpeed = {}; // 스피드 min , max
		_float3		vCenter = {}; // 센터 위치 
		_float3		vRange = {}; // 위치 범위 

		_float2		vMaskingUVMoveTime = { 1.f, 1.f };
		_float2		vDiffuseUVMoveTime = { 1.f, 1.f };
		_float2     vDistortionUVMoveTime = { 1.f , 1.f };
		_float2		vNoiseUVMoveTime = { 1.f ,1.f };
		_float2		vAniTime = { 1.f , 1.f };
		_float2	    vAniIndex = {};

		_bool		isMoveForward = {};
		_float2		vGravity = { 0.f , 9.8f };

		_bool		isSinWave = {};
		_float3     vSinMinAmount = { 0.f , 0.f , 0.f };
		_float3     vSinMaxAmount = { 0.f , 0.f , 0.f };

		_bool		isTurn;
		_float3     vDeltaAngleMin = {};
		_float3     vDeltaAngleMax = {};

		_bool       isAxisTurn;
		_float3     vDeltaAxisAngleMin = {};
		_float3     vDeltaAxisAngleMax = {};

		_bool       isPivotMove = {};
		_float2		vDrag = {};
		_float3     vPivotMin = {};
		_float3     vPivotMax = {};

		_bool       isSizeLerp = {};
		_float3     vDeltaSize = {};
		_float2     vSizeDrag = {};

		_float2     vDelay = {};
		_bool       isNoWorld = {};

		_float2     vDissolveUVMoveTime = { 1.f , 1.f };

		_float2		vDropAttenuation = { 1.f , 1.f };

		_float		fPadding0 = {};
		_float3		vPadding1 = {};

		_float		fSizeLerpOption = {};
		_float		fMoveLerpOption = {};

		_bool		isDetphCompareStop = {};
		_bool		isPadding0 = {};

		_float4     vPadding2 = {};
		_float4     vPadding3 = {};
		_float4     vPadding4 = {};
		_float4     vPadding5 = {};
		_float4     vPadding6 = {};

		_bool		isPadding1 = {};
		_bool		isPadding2 = {};
		_bool		isPadding3 = {};
		_bool		isPadding4 = {};
		_bool		isPadding5 = {};

	}INSTANCE_DESC;

	typedef struct tagPreInstanceDesc
	{
		_int		iNumInstance = { 1 }; // 인스턴스 개수

		_bool		isLoop = { true };
		_bool		isDrop = { false };

		_float2		vLifeTime = { 1.f , 1.f }; // 라이프 타임 min , max
		_float3		vSizeMin = { 1.f, 1.f ,1.f }; // 사이즈 min xyz
		_float3		vSizeMax = { 1.f ,1.f ,1.f }; // 사이즈 max xyz
		_float3		vRotationAngleMin = {};  // 로테이션 min x y z for angle
		_float3		vRotationAngleMax = {};// 로테이션 max x y z for angle
		_float2		vRotationSpeed = {}; // 로테이션 스피드 min , max
		_float2		vSpeed = {}; // 스피드 min , max
		_float3		vCenter = {}; // 센터 위치 
		_float3		vRange = {}; // 위치 범위 

		_float2		vMaskingUVMoveTime = { 1.f, 1.f };
		_float2		vDiffuseUVMoveTime = { 1.f, 1.f };
		_float2     vDistortionUVMoveTime = { 1.f , 1.f };
		_float2		vNoiseUVMoveTime = { 1.f ,1.f };
		_float2		vAniTime = { 1.f , 1.f };
		_float2	    vAniIndex = {};

		_bool		isMoveForward = {};
		_float2		vGravity = { 0.f , 9.8f };

		_bool		isSinWave = {};
		_float3     vSinMinAmount = { 0.f , 0.f , 0.f };
		_float3     vSinMaxAmount = { 0.f , 0.f , 0.f };

		_bool		isTurn;
		_float3     vDeltaAngleMin = {};
		_float3     vDeltaAngleMax = {};

		_bool       isAxisTurn;
		_float3     vDeltaAxisAngleMin = {};
		_float3     vDeltaAxisAngleMax = {};

		_bool       isPivotMove = {};
		_float2		vDrag = {};
		_float3     vPivotMin = {};
		_float3     vPivotMax = {};

		_bool       isSizeLerp = {};
		_float3     vDeltaSize = {};
		_float2     vSizeDrag = {};

		_float2     vDelay = {};
		_bool       isNoWorld = {};

		_float2     vDissolveUVMoveTime = { 1.f , 1.f };

		_float2		vDropAttenuation = { 1.f , 1.f };

		_float		fPadding0 = {};
		_float3		vPadding1 = {};

		_float		fSizeLerpOption = {};
		_float		fMoveLerpOption = {};

		_bool		isDetphCompareStop = {};
		_bool		isPadding0 = {};

	}PRE_INSTANCE_DESC;

	typedef struct tagCSParticleDesc
	{
		_float4x4 WorldMatrix = {};

		_int	 isLoop = {};
		_int     isDrop = {};
		_int     isMoveForward = {};
		_int     isSinWave = {};

		_int	 isTurn = {};
		_int     isAxisTurn = {};
		_int     isPivotMove = {};
		_int     isSizeLerp = {};


		_int	 isNoWorld = {};
		_int     isDetphCompareStop = {};
		_int     isPadding1 = {};
		_int     isPadding2 = {};

		_float   fTimeDelta = {};
		_float	 fSizeLerpOption = {};
		_float   fMoveLerpOption = {};
		_float   fFar = {};

		_float4x4  ViewMatrix = {};
		_float4x4  ProjMatrix = {};

		_float2 vScreenSize;
		_float  fPadding0;
		_float  fPadding1;

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
		_float2  vDistortionUVMoveTime = {};
		_float2  vNoiseUVMoveTime = {};
		_float2  vDissolveUVMoveTime = {};
		_float2	 vAniTime = {};

		_float2	 vAniIndex = {};
		_float   fGravity = {};

		_float3   vSinAmount = {};
		_float3   vDeltaAngle = {};
		_float3	  vDeltaAxisAngle = {};

		_float    fDrag = {};
		_float3   vPivot = {};

		_float    fSizeDrag = {};
		_float3   vDeltaSize = {};

		_float2   vDelay = {};

		_bool	  isCompareStop = {};
		_float    fCollisionTime = {};
		_float    fDropAttenuation = {};

	}CS_PARTICLE_VALUE_DESC;

public:
	CInstance_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_Model(const CInstance_Model& rhs);
	virtual ~CInstance_Model() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);
#ifdef EDITOR_PROJECT
	HRESULT			Save_InstanceModel(HANDLE hFile);
	HRESULT			PreLoad(HANDLE hFile);
#endif	

	virtual HRESULT Initialize(void* pArg) override;
	void			Drop(_float fTimeDelta);
public:
	HRESULT			Render(_uint iMeshIndx);
	void			Instane_Buffer_ReStruct();
	_uint			Get_NumMeshes() const { return m_iNumMeshes; }
	HRESULT			Bind_CS_Output(_uint Index, _uint iBufferIndex);
	HRESULT         Bind_OutPut_SRV_VS(_uint Index, _uint iBufferIndex);
	INSTANCE_DESC	Get_EffectValue() { return m_InstanceDesc; }
	HRESULT			Load_InstanceModel(HANDLE hFile);
private:
		bool LoadData(const _char* filename);

private:
#ifdef EDITOR_PROJECT
	HRESULT			Assimp_Model_Load(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);
	HRESULT			Ready_Meshes(MODEL eType, const aiScene* pAIScene, _fmatrix& PreTransformMatrix);
	_bool			SaveAssimpModel(const _char* filename, const aiScene* pAIScene);
#endif

	HRESULT			Ready_Meshes();

	HRESULT			Change_NumInstance();
	HRESULT			Create_Instance_Buffer();
	HRESULT         Create_SubResource_Buffer();
	HRESULT			Create_CS();

private:
#ifdef EDITOR_PROJECT
	const aiScene*		m_pAIScene = { nullptr };
	Assimp::Importer	m_Importer;
#endif	

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
	SaveModel				m_SaveModel = {};
private:
	class CComputeShader*	m_pComputeShader = {};
	ID3D11Buffer*			m_pConstantBuffer = { nullptr }; // 컴퓨트 쉐이드 전용 상수버퍼
	ID3D11Buffer*			m_pParticleValueBuffer = { nullptr }; // 컴퓨트 쉐이드 두번째 버퍼 (스피드, 로테이션)

public:
	//인스턴싱으로 바꾸기
	static CInstance_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex);

	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr);
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
