#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
public:
	typedef struct tagMeshDesc
	{
		_float3			vPosition = {};
		_float3			vNormal = {};
		_float3			vTangent = {};
		_float3			vBinormal = {};
		_float2			vTexcoord = {};

		XMUINT4			vBlendIndex = {};
		_float4			vBlendWeight = {};

	}MESH_DESC;

	typedef struct tagSkinngMeshDesc
	{
		_float3			vPosition = {};
		_float3			vNormal = {};
		_float3			vTangent = {};
		_float3			vBinormal = {};
		_float2			vTexcoord = {};

	}SKINNG_MESH_DESC;

	typedef struct tagBoneDesc
	{
		_float4x4 BoneMatrix[512];
	}BONE_DESC;


private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	const _char* Get_Name() const { return m_szName; }
	HRESULT Bind_BoneMatrices(const vector<class CBone*>& Bones, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_BoneMatrices(_float4x4* pCombinedMatrices, class CShader* pShader, const _char* pConstantName);
	HRESULT Render_Indexed(_uint IndexCount, _uint StartIndexLocation, _uint BaseVertexLocation);
#ifdef EDITOR_PROJECT
private:
	virtual HRESULT Initialize_Prototype(MODEL eType, vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix& PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(vector<class CBone*>& Bones, const aiMesh* pAIMesh);
public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix& PreTransformMatrix);
	HRESULT SaveAsBinary(HANDLE hFile, DWORD& dwByte);
	PSX::PxTriangleMesh* ConvertToPxMesh(const PSX::PxCookingParams* pParam, PSX::PxPhysics* pPhysX, _matrix WorldMatrix);
#endif // EDITOR_PROJECT

public:
	//인스턴싱 모델 전용 드로우, 바인딩 함수 
	virtual HRESULT Bind_Resources_Instance(ID3D11Buffer* pVBInstance, _uint iInstanceStride, _uint iBufferCount);
	virtual HRESULT Render_Instance(_uint iNumInstance);
	
	void ComputSkinning(class CComputeShader* ComputeShader, ID3D11Buffer* ConstantBuffer);

	void FillBoneDesc(const vector<_float4x4>& CombinedMatrices, BONE_DESC* pOut);

	_uint Get_Vertex() { return m_iNumVertices; }
private:
	virtual HRESULT Initialize_Prototype(HANDLE hFile, DWORD& dwByte);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_VertexBuffer_For_NonAnim(HANDLE hFile, DWORD& dwByte);
	HRESULT Ready_VertexBuffer_For_Anim(HANDLE hFile, DWORD& dwByte);
	// 바이너리
	virtual HRESULT Initialize_Prototype(MODEL eType, const class CModel* pModel, SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_NonAnim(SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const class CModel* pModel, SaveMesh* _SaveMesh);

	HRESULT Create_RawVB(vector<MESH_DESC>& RawVertices);

	//
private:
	_char	m_szName[MAX_PATH] = {};
	_uint	m_iMaterialIndex = {};
	_uint	m_iNumBones = { };
	_uint	m_iStartIndex = {};

	vector<_int>		m_BoneIndices;
	_float4x4*			m_pBoneMatrices = { nullptr };
	vector<_float4x4>	m_offsetMatrices;
	// 바이너리
	std::vector<_uint>   m_Indices;
	std::vector<_float3> m_Vertices;
	//

private:
	MESH_DESC					m_MeshDesc = {};
	BONE_DESC					m_BoneDesc = {};
	_uint						m_iNumBuffer = {};
	_uint						m_iNumMeshes = {};
	_int						m_iRootBoneIndex = { -1 };
	_int						m_iRawVertexStride = {};

	vector<MESH_DESC> m_RawVertices;

	ID3D11Buffer* m_pVBMesh = { nullptr };

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE hFile, DWORD& dwByte);
	// 바이너리
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const class CModel* pModel, SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix);
	//
	virtual CMesh* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
