#include "pch.h"
#include "Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_fRadius(rhs.m_fRadius)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName) const
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

_float3 CTransform::Get_Scale() const
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
	);
}
_vector CTransform::Get_QuarternionVector() const
{
	_vector s, r, t;
	XMMatrixDecompose(&s, &r, &t, Get_XMWorldMatrix());
	return r;
}
_vector CTransform::Get_RollPitchYawVector() const
{
	_vector vQuaternion = XMQuaternionNormalize(Get_QuarternionVector());

	_float roll, pitch, yaw;
	{ // XMMatrixRotationRollPitchYawFromVector로 완벽히 복구됨
		XMFLOAT4 q;
		XMStoreFloat4(&q, vQuaternion);

		// Pitch (x-axis)
		float sinp = 2.f * (q.w * q.x - q.z * q.y);
		if (fabsf(sinp) >= 1.f)
			pitch = copysignf(XM_PIDIV2, sinp);
		else
			pitch = asinf(sinp);

		// Yaw (y-axis)
		float siny_cosp = 2.f * (q.w * q.y + q.x * q.z);
		float cosy_cosp = 1.f - 2.f * (q.y * q.y + q.x * q.x);
		yaw = atan2f(siny_cosp, cosy_cosp);

		// Roll (z-axis)
		float sinr_cosp = 2.f * (q.w * q.z - q.x * q.y);
		float cosr_cosp = 1.f - 2.f * (q.z * q.z + q.x * q.x);
		roll = atan2f(sinr_cosp, cosr_cosp);
	}
	return XMVectorSet(pitch, yaw, roll, 0.f);
}

void CTransform::Set_Scale(_float3& vScale)
{
	_vector vRight = XMVector3Normalize(Get_State(STATE::RIGHT));
	_vector vUp = XMVector3Normalize(Get_State(STATE::UP));
	_vector vLook = XMVector3Normalize(Get_State(STATE::LOOK));

	Set_State(STATE::RIGHT, vRight * vScale.x);
	Set_State(STATE::UP, vUp * vScale.y);
	Set_State(STATE::LOOK, vLook * vScale.z);
}

void CTransform::Mult_Scale(_float3& vScale)
{
	_vector vRight = (Get_State(STATE::RIGHT));
	_vector vUp = (Get_State(STATE::UP));
	_vector vLook = (Get_State(STATE::LOOK));

	Set_State(STATE::RIGHT, vRight * vScale.x);
	Set_State(STATE::UP, vUp * vScale.y);
	Set_State(STATE::LOOK, vLook * vScale.z);
}

void CTransform::Set_WorldMatrix(PSX::PxTransform dstMatrix)
{
	_float3 vScale = Get_Scale();

	XMStoreFloat4x4(&m_WorldMatrix,
		XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorZero(), XMLoadFloat4((_float4*)&dstMatrix.q), XMLoadFloat3((_float3*)&dstMatrix.p)));
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;
	m_fRadius = pDesc->fRadius;

	return S_OK;
}

_vector CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vPos = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);
	_vector		vMomentum = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	vPos += vMomentum;
	Set_State(STATE::POSITION, vPos);
	return +vMomentum;
}

_vector CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPos = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);
	_vector		vMomentum = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	vPos -= vMomentum;
	Set_State(STATE::POSITION, vPos);
	return -vMomentum;
}

_vector CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPos = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vMomentum = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	vPos -= vMomentum;
	Set_State(STATE::POSITION, vPos);
	return -vMomentum;
}

_vector CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPos = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vMomentum = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	vPos += vMomentum;
	Set_State(STATE::POSITION, vPos);
	return +vMomentum;
}

_vector CTransform::Go_Up(_float fTimeDelta)
{
	_vector		vPos = Get_State(STATE::POSITION);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vMomentum = XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;
	vPos += vMomentum;
	Set_State(STATE::POSITION, vPos);
	return +vMomentum;
}

_vector CTransform::Go_Down(_float fTimeDelta)
{
	_vector		vPos = Get_State(STATE::POSITION);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vMomentum = XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;
	vPos -= vMomentum;
	Set_State(STATE::POSITION, vPos);
	return -vMomentum;
}

void CTransform::Translation(_float3& vTrans)
{
	Translation(XMLoadFloat3(&vTrans));
}

void CTransform::Translation(_fvector vTrans)
{
	Set_State(STATE::POSITION, Get_State(STATE::POSITION) + XMVectorSetW(vTrans, 0.f));
}

void CTransform::AccumulateMomentum(_fvector vMomentum)
{
	XMStoreFloat3(&m_vMomentum, (XMLoadFloat3(&m_vMomentum) + XMVectorSetW(vMomentum, 0.f)));
}
_vector CTransform::Get_CurrentMomentum() const
{
	return XMLoadFloat3(&m_vMomentum);
}
_vector CTransform::Get_EstimatedPositionByMomentum() const
{
	return XMVectorSetW(Get_State(STATE::POSITION) + XMLoadFloat3(&m_vMomentum), 1.f);
}
void CTransform::RewindMomentum()
{
	m_vMomentum = m_vBackMomentum;
	m_vBackMomentum = { 0.f, 0.f, 0.f };
}

void CTransform::BookMomentum(_fvector vMomentum)
{
	XMStoreFloat3(&m_vBackMomentum, (XMLoadFloat3(&m_vBackMomentum) + XMVectorSetW(vMomentum, 0.f)));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector vRight = Get_State(STATE::RIGHT);
	_vector vUp = Get_State(STATE::UP);
	_vector vLook = Get_State(STATE::LOOK);

	_matrix TurnMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	vRight = XMVector3TransformNormal(vRight, TurnMatrix);
	vUp = XMVector3TransformNormal(vUp, TurnMatrix);
	vLook = XMVector3TransformNormal(vLook, TurnMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::TurnAngle(_vector vAxis, _float fAngle)
{
	_vector vRight = Get_State(STATE::RIGHT);
	_vector vUp = Get_State(STATE::UP);
	_vector vLook = Get_State(STATE::LOOK);
	if (vAxis.m128_f32[0] == 0&&
		vAxis.m128_f32[1] == 0&&
		vAxis.m128_f32[2] == 0)
		return;

	vAxis = XMVector3Normalize(vAxis);

	_matrix TurnMatrix = XMMatrixRotationAxis(vAxis, fAngle);

	vRight = XMVector3TransformNormal(vRight, TurnMatrix);
	vUp = XMVector3TransformNormal(vUp, TurnMatrix);
	vLook = XMVector3TransformNormal(vLook, TurnMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}


void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3 vScale = Get_Scale();
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_float fRadianX, _float fRadianY, _float fRadianZ)
{
	// _fvector 버전이랑 FromVector만 다른거임
	_float3 vScale = Get_Scale();
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_vector vQuternion = XMQuaternionRotationRollPitchYaw(fRadianX, fRadianY, fRadianZ);
	_matrix RotationMatrix = XMMatrixRotationQuaternion(vQuternion);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_float3& vRadian)
{
	Rotation(vRadian.x, vRadian.y, vRadian.z);
}

void CTransform::Rotation(_fvector vRPY)
{
	// _float3 버전이랑 FromVector만 다른거임
	_float3 vScale = Get_Scale();
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_vector vQuternion = XMQuaternionRotationRollPitchYawFromVector(vRPY);
	_matrix RotationMatrix = XMMatrixRotationQuaternion(vQuternion);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::LookAt(_fvector vAt)
{
	_float3 vScale = Get_Scale();

	_vector vLook = vAt - Get_State(STATE::POSITION);
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);
}

_float CTransform::TargetDis(_fvector vTarget)
{
	_vector vTargetDis = vTarget - Get_State(STATE::POSITION);
	_float dist = XMVectorGetX(
		XMVector3Length(vTargetDis));
	return dist;
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg, class CGameObject* pOwner)
{
	CTransform* pInstance = new CTransform(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CTransform::Describe_Entity()
{
	if (GUI::TreeNode("Transform")) {

		GUI::DragFloat4("Right", (_float*)(&m_WorldMatrix._11), 1.f, 0.f, 0.f, "%.3f");
		GUI::DragFloat4("Up", (_float*)(&m_WorldMatrix._21), 1.f, 0.f, 0.f, "%.3f");
		GUI::DragFloat4("Look", (_float*)(&m_WorldMatrix._31), 1.f, 0.f, 0.f, "%.3f");
		GUI::DragFloat4("Pos", (_float*)(&m_WorldMatrix._41), 1.f, 0.f, 0.f, "%.3f");


		static _float3 s_vRotation = {};

		if(GUI::InputFloat3("Rotation", (_float*)&s_vRotation))
		{
			Rotation(s_vRotation.x, s_vRotation.y, s_vRotation.z);
		}

		GUI::TreePop();
		GUI::Spacing();

		
	}
}

#endif // _DEBUG
