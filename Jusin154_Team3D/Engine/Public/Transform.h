#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final :
	public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = 0.f;
		_float		fRotationPerSec = 0.f;
		_float		fRadius = 20.f;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;
public:
	HRESULT		Bind_ShaderResource(class CShader* pShader, const _char* pConstantName) const;

	void		Set_Scale(_float3& vScale);
	void		Mult_Scale(_float3& vScale);
	void		Set_State(STATE eState, _fvector vState) { XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]), vState); }
	void		Set_WorldMatrix(_float4x4& dstMatrix) { memcpy_s(&m_WorldMatrix, sizeof(_float4x4), &dstMatrix, sizeof(_float4x4)); }
	void		Set_WorldMatrix(_fmatrix& dstMatrix) { XMStoreFloat4x4(&m_WorldMatrix, dstMatrix); }
	void		Set_WorldMatrix(PSX::PxTransform dstMatrix);
	_float		Get_Speed() const { return m_fSpeedPerSec; }
	_float		Get_RotationSpeed() const { return m_fRotationPerSec; }
	_float		Get_Radius() const { return m_fRadius; }
	_float3		Get_Scale() const;
	_vector		Get_QuarternionVector() const; // 현재 회전정보를 담고 있는 쿼터니언 벡터를 out 
	_vector		Get_RollPitchYawVector() const;
	_vector		Get_State(STATE eState) const {
		return XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)])); 
	}
	const _float4* Get_StatePtr(STATE eState) { return reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]); }
	const _float4x4* Get_WorldMatrixPtr() const { return &m_WorldMatrix; }
	const _float4x4* Get_PrevWorldMatrixPtr() const { return &m_PrevMatrix; }
	const _matrix Get_XMWorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	const _matrix Get_WorldMatrixInv() const { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }
public:
	_vector Go_Straight(_float fTimeDelta);
	_vector Go_Backward(_float fTimeDelta);
	_vector Go_Left(_float fTimeDelta);
	_vector Go_Right(_float fTimeDelta);
	_vector Go_Up(_float fTimeDelta);
	_vector Go_Down(_float fTimeDelta);
	void Move_Look(_float fScala);
	void Move_Up(_float fScala);
	void Move_Right(_float fScala);
	///////////////////////////////////
	_vector Go_LerpStraight(_float fSpeed, _float fTimeDelta);
	_vector Go_LerpLeft(_float fSpeed, _float fTimeDelta);
	_vector Go_LerpRight(_float fSpeed, _float fTimeDelta);
	_vector Go_LerpUp(_float fSpeed, _float fTimeDelta);
	_vector Go_LerpDown(_float fSpeed, _float fTimeDelta);

	void	Translation(_float3& vTrans);
	void	Translation(_fvector vTrans);

	void	AccumulateMomentum(_fvector vMomentum);
	_vector Get_CurrentMomentum() const;
	void	Set_CurrentMomentum(_fvector vMomentum);
	void	RewindMomentum();

	_vector Get_EstimatedPositionByMomentum() const;
	void	BookMomentum(_fvector vMomentum);

	void Turn(_fvector vAxis, _float fTimeDelta);
	void TurnAngle(_vector vAxis, _float fAngle);
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation(_float fRadianX, _float fRadianY, _float fRadianZ);
	void Rotation(_float3& vRadian);
	void Rotation(_fvector vRPY);

	// Up, Down 벡터를 바라보게 하면 터짐, w에 1 있어야함
	void LookAt(_fvector vAt);
	void LookAt_Lerp(_fvector vAt, _float fTimeDelta, _float fSpeed);
	void LookAt_Horizontal(_fvector vAt);
	_float TargetDis(_fvector vTarget);

private:
	_float				m_fSpeedPerSec = {};
	_float				m_fRotationPerSec = {};
	_float4x4			m_WorldMatrix = {};
	_float4x4			m_PrevMatrix = {};
	_float				m_fRadius = { 20.f };

	_float3				m_vMomentum = {}; // 현재 프레임에 계산된 이동량
	_float3				m_vBackMomentum = {}; // 다음 프레임에 계산될 이동량

#ifdef _DEBUG
	_float3		m_vRotation = {};
#endif // _DEBUG


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
