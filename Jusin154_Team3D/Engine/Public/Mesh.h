#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_BoneMatrices(const vector<class CBone*>& Bones, class CShader* pShader, const _char* pConstantName);

private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex = {};
	_uint				m_iNumBones = { };
	_float4x4*			m_pBoneMatrices = { nullptr };
	vector<_float4x4>	m_OffsetMatrices;
	vector<_int>		m_BoneIndices;
	vector<_uint>		m_Indices;
	vector<_float3>		m_Vertices;

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const class CModel* pModel, const aiMesh* pAIMesh);


public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END