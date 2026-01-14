#pragma once

#include "Client_Define.h"
#include "Camera.h"
#include "CamPosition_WorldLook.h"

NS_BEGIN(Engine)
class CUnit;
NS_END

NS_BEGIN(Client)
class CCamPosition_Target;

class CCamera_Cinematic final : public CCamera
{
public:
	typedef struct tagCamera_Cinematic : public CCamera::CAMERA_DESC
	{
	}Camera_Cinematic_DESC;
private:
	CCamera_Cinematic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Cinematic(const CCamera_Cinematic& rhs);
	virtual ~CCamera_Cinematic() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Trigger(CTimeSocket& Socket)override;

public:
	virtual void Active_Camera(pair<_float4, _float3>& pairTransitionInfo)override;
	void Update_LerpTimer(_float fTimeDelta);
	void Set_Priority(_uint iPriority);
	void Set_LookTarget(CGameObject* pTarget, const _float4x4* pTargetSocketMatrix);
	void Set_FollowTarget(CGameObject* pTarget, const _float4x4* pTargetSocketMatrix);


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Ready_SubPart();
	HRESULT Bind_ShaderResources() override;

	void Lerp_Translation(_float fTimeDelta);
	void Start_Lerp_Translation(_float fTimeMaximum, PSX::PxTransform pxTransform);
	void Start_Lerp_Translation(_float fTimeMaximum, _float3& vTrans);
	void Clear_Lerp_Translation();
	void Lerp_Rotation(_float fTimeDelta);
	void Start_Lerp_Rotation(_float fTimeMaximum, PSX::PxTransform pxTransform, _bool bMainTainDistance);
	void Start_Lerp_Rotation(_float fTimeMaximum, _float4& vRotQ, _bool bMainTainDistance);
	void Clear_Lerp_Rotation();


	void Lerp_FovY(_float fTimeDelta);
	void Start_Lerp_FovY(_float fTimeMaximum, _float2 vRange);
	void Clear_Lerp_FovY();

	virtual void Transition(_float fTimeDelta) override;


	void Rotation(_fvector vRotQ);
private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CCamPosition_Target* m_pLookTargetPart = { nullptr };
	CCamPosition_Target* m_pFollowTargetPart = { nullptr };
	CCamPosition_WorldLook* m_pWORLD_LOCK_TargetPart = { nullptr };

	_bool	m_bLerpTranslation = { false };
	_float2 m_vLerpTranslationTimer = {};

	_bool	m_bLerpRotiation = { false };
	_float2 m_vLerpRotiationTimer = {};

	_float3 m_vLerpTranslationStart = {};
	_float3 m_vLerpTranslationEnd = {};
	_float4 m_vLerpRotQStart = {};
	_float4 m_vLerpRotQEnd = {};

	_bool	m_bLerpFovY = { false };
	_float2 m_vFovYLerp = { 60.f, 60.f };
	_float2 m_vLerpFovYTimer = { };
	_float m_fMaintainingDistance = { };
#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
	unique_ptr<PrimitiveBatch<VertexPositionColor>> m_Batch = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
#endif // _DEBUG

public:
	static CCamera_Cinematic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
	virtual void Free() override;


#ifdef _DEBUG
	void Describe_Entity() override;

#endif // _DEBUG
};

NS_END
