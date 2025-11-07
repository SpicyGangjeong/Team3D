#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3		vCenter;
		COLLIDER	eType;
	}BOUNDING_DESC;
protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual void Update(_fmatrix WorldMatrix) = 0;
	virtual _bool Intersect(COLLIDER eType, class CBounding* pTarget) = 0;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) = 0;
#endif
protected:
	HRESULT Initialize();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free();
};

NS_END