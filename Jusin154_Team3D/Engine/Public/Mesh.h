#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	void SeT_MaterialIndex(_uint iMaterialIndex) { m_iMaterialIndex = iMaterialIndex; }

	const _char* Get_Name() const { return m_strName.c_str(); }
	_int Get_NumBone() { return m_iNumBones; }
	HRESULT Bind_BoneMatrices(const vector<class CBone*>& Bones, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_BoneMatrices(_float4x4* pCombinedMatrices, class CShader* pShader, const _char* pConstantName);
	HRESULT Render_Indexed(_uint IndexCount, _uint StartIndexLocation, _uint BaseVertexLocation);
#ifdef EDITOR_PROJECT
private:
	virtual HRESULT Initialize_Prototype(MODEL eType, vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix& PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(vector<class CBone*>& Bones, const aiMesh* pAIMesh);

	void BuildGlobalToLocal(_uint boneCount);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix& PreTransformMatrix);
#endif // EDITOR_PROJECT
	PSX::PxTriangleMesh* ConvertToPxMesh(const PSX::PxCookingParams* pParam, PSX::PxPhysics* pPhysX, _matrix WorldMatrix);

public:
	//인스턴싱 모델 전용 드로우, 바인딩 함수 
	virtual HRESULT Bind_Resources_Instance(ID3D11Buffer* pVBInstance, _uint iInstanceStride, _uint iBufferCount);
	virtual HRESULT Render_Instance(_uint iNumInstance);
	
	_uint Get_Vertex() { return m_iNumVertices; }
	vector<_float4x4> Get_OffsetMatrix() { return m_offsetMatrices; }
	ID3D11ShaderResourceView* Get_BoneRemapSRV() { return m_pBoneRemapSRV; }
	ID3D11Buffer* Get_BoneRemapBuffer() { return m_pBoneRemapBuffer; }
private:
	virtual HRESULT Initialize(void* pArg) override;
	// 바이너리
	virtual HRESULT Initialize_Prototype(MODEL eType, const class CModel* pModel, SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_NonAnim(SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const class CModel* pModel, SaveMesh* _SaveMesh);
	HRESULT Create_BoneRemapBuffer();
private:
	_string m_strName = { };
	_uint	m_iMaterialIndex = {};
	_uint	m_iNumBones = { };
	_uint	m_iStartIndex = {};

	vector<_int>		m_BoneIndices;
	_float4x4*			m_pBoneMatrices = { nullptr };
	vector<_float4x4>	m_offsetMatrices;
	vector<_uint> m_BoneRemap;
	ID3D11ShaderResourceView* m_pBoneRemapSRV = nullptr;
	ID3D11Buffer* m_pBoneRemapBuffer = nullptr;

	// 바이너리
	vector<_uint>   m_Indices;
	vector<_float3> m_Vertices;
	//

private:
	_uint						m_iNumBuffer = {};
	_uint						m_iNumMeshes = {};
	_int						m_iRootBoneIndex = { -1 };

public:
	// 바이너리
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const class CModel* pModel, SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix);
	//
	virtual CMesh* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;

#endif // _DEBUG

};

NS_END
