#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public CBounding::BOUNDING_DESC
	{
		_float			fRadius;
	}BOUNDING_SPHERE_DESC;

private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;
public:
	BoundingSphere* Get_Desc()const {
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
	BoundingSphere* m_pOriginalDesc = { nullptr };
	BoundingSphere* m_pDesc = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const void* pDesc);
	virtual void Free() override;

};

NS_END