#include "Collider.h"
#include "pch.h"

#include "Collider.h"
#include "GameObject.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "GameInstance.h"



CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}
CCollider::CCollider(const CCollider& rhs)
	:CComponent(rhs)
	, m_eType{ rhs.m_eType }
#ifdef _DEBUG
	, m_pBatch{ rhs.m_pBatch }
	, m_pEffect{ rhs.m_pEffect }
	, m_pInputLayout{ rhs.m_pInputLayout }
#endif
{
#ifdef _DEBUG
	SAFE_ADDREF(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype()
{

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout))){
		return E_FAIL;
	}
#endif


	return S_OK;
}
HRESULT CCollider::Initialize(void* pArg)
{
	CBounding::BOUNDING_DESC* pDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);
	m_eType = pDesc->eType;
	switch (m_eType)
	{
	case COLLIDER::AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pArg);
		break;
	case COLLIDER::OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pArg);
		break;
	case COLLIDER::SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pArg);
		break;
	}

	return S_OK;
}
void CCollider::Update(_fmatrix matWorld)
{
	m_pBounding->Update(matWorld);
}
#ifdef _DEBUG

HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding)
		return E_FAIL;

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);

	return m_pBounding->Render(m_pBatch, false == m_isCollide ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));
}

HRESULT CCollider::Render(_float3 m_vColor)
{
	if (nullptr == m_pBounding)
		return E_FAIL;

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);

	return m_pBounding->Render(m_pBatch, XMLoadFloat3(&m_vColor));
}


#endif

_bool CCollider::Intersect(COLLIDER eType, CCollider* pTarget)
{
	m_isCollide = m_pBounding->Intersect(eType, pTarget->m_pBounding);

	return m_isCollide;
}

CGameObject* CCollider::Get_Caster()
{
	return m_pOwner;
}

void CCollider::OnCollision(CGameObject* pOther)
{
	m_pOwner->OnCollision(pOther);
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CComponent* CCollider::Clone(void* pArg, class CGameObject* pOwner)
{
	CCollider* pInstance = new CCollider(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CCollider::Free()
{
	__super::Free();


#ifdef _DEBUG
	SAFE_RELEASE(m_pInputLayout);
	if (false == m_bCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif

	SAFE_RELEASE(m_pBounding);
}

void CCollider::Describe_Entity()
{
}
