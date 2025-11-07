#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;


public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_int Get_BoneIndex(const _char* pBoneName) const;
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	const vector<class CMesh*>& Get_Meshes() const { return m_Meshes; }
public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT			Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT			Bind_Material(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);
	_bool			Play_Animation(_float fTimeDelta);
	void			Set_AnimationIndex(_int iIndex, _bool isLoop = true);
	HRESULT			Render(_uint iMeshIndex);

private:
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;

private:
	MODEL						m_eType = {};
	_float4x4					m_PreTransformMatrix = {};

	vector<class CMesh*>		m_Meshes;
	_uint						m_iNumMeshes = {};

	vector<class CMaterial*>	m_Materials;
	_uint						m_iNumMaterials = {};

	vector<class CBone*>		m_Bones;

	vector<class CAnimation*>	m_Animations;
	_int						m_iCurrentAnimIndex = { -1 };
	_int						m_iPreAnimIndex = { 0 };
	_int						m_iNumAnimations = {};
	_bool						m_isLoop = { false };
	_bool						m_isFinish = { false };

	_float						m_fBlendTime = { 0.f };
	_float						m_fBlendDuration = { 0.3f };

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free();
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif

};

NS_END