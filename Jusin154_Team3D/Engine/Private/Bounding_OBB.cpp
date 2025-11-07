#include "pch.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_OBB::Initialize(const void* pArg)
{
	const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(pArg);

	_float4		vQuaternion = {};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vAngles.x, pDesc->vAngles.y, pDesc->vAngles.z));

	m_pOriginalDesc = new BoundingOrientedBox(pDesc->vCenter, _float3(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f), vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{

	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(COLLIDER eType, CBounding* pTarget)
{
	_bool		isColl = { false };

	switch (eType)
	{
	case COLLIDER::AABB:
		isColl = m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::OBB:
		isColl = m_pDesc->Intersects(*static_cast<CBounding_OBB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::SPHERE:
		isColl = m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pTarget)->Get_Desc());
		break;
	}



	return isColl;
}

#ifdef _DEBUG

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	pBatch->Begin();

	DX::Draw(pBatch, *m_pDesc, vColor);

	pBatch->End();

	return	S_OK;
}

#endif

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const void* pArg)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}
