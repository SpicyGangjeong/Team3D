#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;
public:
	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}
	_float4x4* Get_CombinedTransformationMatrixPtr() {
		return &m_CombinedTransformationMatrix;
	}
	void Set_CombinedTransformationMatrixPtr(_matrix& newMatrix) {
		return XMStoreFloat4x4(&m_CombinedTransformationMatrix, newMatrix);
	}
	_bool Compare_Name(const _char* pBoneName) const {
		return !strcmp(pBoneName, m_szName);
	}
	void Get_KeyFrame(KEYFRAME& kf, _bool bIsCurrentFrame);	// 현재 본의 로컬매트릭스를 캡쳐해서 키프레임으로 성분 분해하여 올려보낸다.
	const _char* Get_Name() const { return m_szName; }
	_float3 Get_LocalPosition();
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
	void Set_TransformationMatrix(_fmatrix TransformationMatrix); // 애니메이션에서 현재 로컬 매트릭스를 변경할 때 사용된다

private:
	_char m_szName[MAX_PATH] = {};						// 본의 이름
	_float4x4 m_TransformationMatrix = {};				// 본의 현재 로컬 매트릭스
	_float4x4 m_CombinedTransformationMatrix = {};		// 본의 루트노드로부터 상대적인 복합매트릭스 ( 부모노드는 PreTranformMatrix와 월드좌표로부터 상대적임 )
	_int m_iParentBoneIndex = { -1 };					// 부모본의 인덱스 ( 모델에 있는 본벡터의 인덱스 ) ( 부모는 인덱스 -1 고정 )
	_float4x4 m_matInitial = {};

private:
	HRESULT Initialize(HANDLE hFile, DWORD& dwByte);
	// 바이너리
	HRESULT Initialize(const SaveNode& _SaveNode, _int iParentIndex);
	//

public:
	static CBone* Create(HANDLE hFile, DWORD& dwByte);
	CBone* Clone();
	virtual void Free() override;

	// 바이너리
	static CBone* Create(const SaveNode& _SaveNode, _int iParentIndex);
	//
#ifdef EDITOR_PROJECT
public:
	HRESULT SaveAsBinary(HANDLE hFile, DWORD& dwByte);
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
private:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
#endif // EDITOR_PROJECT

};

NS_END