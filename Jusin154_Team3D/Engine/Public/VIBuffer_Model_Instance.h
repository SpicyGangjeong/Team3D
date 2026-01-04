#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Model_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagCS_Model_Instacne_Desc
	{
		_float fSpeed;
		_float fMaxRadian;
	}CS_MODEL_INSTANCE_DESC;

	typedef struct tagCS_Model_Instacne_Constant_Desc
	{
		_float fTimeDelta;
		_float fPading1;
		_float fPading2;
		_float fPading3;
	}CS_MODEL_INSTANCE_CONSTANT_DESC;

private:
	CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& rhs);
	virtual ~CVIBuffer_Model_Instance() = default;

public:
	_uint Get_NumMesh() const { return m_iNumMeshes; }
	const _char* Get_MeshName(_uint iMeshIndex);
	HRESULT Ready_PhysXMeshes(_uint iLevel);
	

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, const _char* pMatrialPath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);
	void Shake(_float fTimeDelta);
	HRESULT Set_Shake_Value(_float2 vRadius, _float2 vSpeed);

	HRESULT Bind_OutPut_SRV_VS(_uint iIndex, _uint iBufferIndex);
	HRESULT Bind_Matrial(class CShader* pShader, _uint iMeshIndex);

	HRESULT Load_WorldData(vector<_float4x4>& WorldMatrices);
private:
	_uint										m_iNumBuffer = {};
	_uint										m_iNumMeshes = {};
	
	_uint										m_iCurrentNumInstance = {};
	_uint										m_iMaxNumInstance = {};

	_uint										m_iNumPhysXMeshes = {};

	_float										m_fTimeAcc = {};
	class CComputeShader*						m_pComputeShader = { nullptr };

	ID3D11Buffer*								m_pConstantBuffer = { nullptr };
	ID3D11Buffer*								m_pParticleValueBuffer = { nullptr };

	INSTANCE_DESC								m_InstanceDesc = {};

	vector<class CMesh*>						m_Meshes = {};
	vector<vector<ID3D11ShaderResourceView*>>	m_MeshMaterialsSRV = {};
	vector<aiTextureType>						m_SurfaceMaterialFlag = {};

	vector<vector<string>>						m_MeshMaterialPathes = {};

	vector<PSX::PxTriangleMesh*>				m_TriMeshes = {};
private:
	HRESULT			Reay_Instance_Buffer();
	HRESULT			Load_InstanceData(ifstream& in);
	HRESULT         Load_ModelData(const _char* pModelFilePath);
	HRESULT			Load_Material(const _char* pModelFilePath, const _char* pMatrialPath);
	HRESULT			Ready_Materials();
	HRESULT			Create_ComputeShader();
	HRESULT			Create_ConstantBuffer();
	HRESULT			Create_Instance_Value_Buffer();

#ifdef EDITOR_PROJECT
public:
	void			Update_Instance();
	void			Add_Instance(_fmatrix WorldMatrix);
	void			Delete_Instance();
	void			Fix_Instance(_fmatrix WorldMatrix);

	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc, const _char* pFileName, const _char* pMatrialPath);
	HRESULT			Assimp_Model_Load(const _char* pModelFilePath);
	HRESULT			SaveAssimpModel(const _char* pModelFilePath);
	
public:
	static CVIBuffer_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc, const _char* pModelFilePath, const _char* pMatrialPath);

	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;
	vector<SaveMesh> 		m_SaveMeshDesc = {};
#endif

public:
	static CVIBuffer_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const _char* pMatrialPath);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif

};

NS_END
