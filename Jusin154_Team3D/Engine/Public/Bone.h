#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationMatrixPtr() const {
		return &m_CombinedTransformationMatrix;
	}
	_bool Compare_Name(const _char* pBoneName) const {
		return !strcmp(pBoneName, m_szName);
	}
public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	void	Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
	void	Set_Transformation(_fmatrix BoneTransformationMatrix);
private:
	_char		m_szName[MAX_PATH] = {};
	_float4x4	m_TransformationMatrix = {};
	_float4x4	m_CombinedTransformationMatrix = {};
	_int		m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	CBone* Clone();
	virtual void Free() override;
};
NS_END
