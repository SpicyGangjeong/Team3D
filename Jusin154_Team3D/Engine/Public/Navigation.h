#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{ 
	// 맵툴 형님께서 다 갈아 엎으셔도 됩니다 ^^7, CCell, CVIBuffer_Cell, 이친구들이랑 영향력이 묶여있어요
	static _float4x4				s_WorldMatrix;
public:
	typedef struct tagNavigaionDesc
	{
		_int					iCurrentCellIndex = { -1 };
		vector<class CCell*>*	pCells = { nullptr };
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	void Update(_fmatrix WorldMatrix);
	HRESULT isMove(_fvector vCurrentPos, _vector vCurrentDir, _vector& vCorrectOut);
	HRESULT Get_NearCell(_fvector vCurrentPos, _int& iDestCell);
	_ubyte Get_CellAttribute();
	_int Find_Cell_ByPosition(_fvector vLocalPos, _uint iStartCell, _uint iMaxDepth);
	_bool Compute_Height(class CTransform* pTransform, _float fTimeDelta, _bool bIsHover = false);
	_int Get_CurrentCellIndex() { return m_iCurrentCellIndex; }
	void Set_CurrentCellIndex(_int iIndex) { m_iCurrentCellIndex = iIndex; }
	const vector<class CCell*>* Get_CellPtr();
	HRESULT isIn(_fvector vLocalPos);

#ifdef _DEBUG
public:
	void Change_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY eType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	HRESULT Render();
#endif

private:
	_int							m_iCurrentCellIndex = { -1 };
	vector<class CCell*>*			m_pCells = { nullptr };


#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

private:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
