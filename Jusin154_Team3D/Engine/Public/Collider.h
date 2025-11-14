#pragma once
#include "Component.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;
public:
	virtual void Update(_fmatrix matWorld);

public:
	_bool Intersect(COLLIDER eType, CCollider* pTarget);
	virtual void OnCollision(CGameObject* pOther = nullptr);
	COLLIDER Get_ColliderType() { return m_eType; }
	CGameObject* Get_Caster();

private:
	COLLIDER m_eType = { COLLIDER::END };
	class CBounding* m_pBounding = { nullptr };
	_bool m_isCollide = { false };

#ifdef _DEBUG
public:
	virtual HRESULT Render();
	virtual HRESULT Render(_float3 m_vColor);
private:
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*									m_pEffect = { nullptr };
	ID3D11InputLayout*								m_pInputLayout = { nullptr };
#endif // DEBUG

private:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	void Describe_Entity() override;

};

NS_END