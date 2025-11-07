#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public CBounding::BOUNDING_DESC
	{
		_float3			vSize;
	}BOUNDING_AABB_DESC;

private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	BoundingBox* Get_Desc()const {
		return m_pDesc;
	}

public:
	HRESULT Initialize(const void* pDesc);
	virtual void Update(_fmatrix WorldMatrix)override;
	virtual _bool Intersect(COLLIDER eType, class CBounding* pTarget)override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)override;
#endif

private:
	BoundingBox* m_pOriginalDesc = { nullptr };
	BoundingBox* m_pDesc = { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const void* pDesc);
	virtual void Free() override;

};

NS_END