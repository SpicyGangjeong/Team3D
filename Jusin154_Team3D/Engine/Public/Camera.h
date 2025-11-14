#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CTransform::TRANSFORM_DESC
	{
		_float			fFovy{}, fNear{}, fFar{};
		_uint			iPriority = { UINT_MAX };
		const _wchar*	pCameraKey = { };
		CGameObject*	pFollowTarget = { nullptr };
		CGameObject*	pLookTarget = { nullptr };
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual void Active_Camera(pair<_float4, _float3>& pairTransitionInfo);
	virtual void DeActive_Camera(pair<_float4, _float3>& pairTransition);
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT	Ready_Components(void* pArg)override;
	_bool IsActive() const { return m_bActive; }
	_bool IsImportantThan(CCamera* pOther) const;
	const _float* Get_CurrentFar();
	HRESULT Ready_Shadow();

protected:
	_float			m_fFovy = {};
	_float			m_fAspect = {};
	_float			m_fNear = {};
	_float			m_fFar = {};

	_float			m_fTransitionMaxTime = { 1.f };
	_float			m_fTransitionCurrentTime = { 0.f };

	_uint			m_iPriority = { UINT_MAX };

	_bool			m_bIsCurrentTransition = { false };

	_bool			m_bActive = { false };

	const _wchar*	m_pCameraKey = { nullptr };
	CGameObject*	m_pFollowTarget = { nullptr };
	CGameObject*	m_pLookTarget = { nullptr };

	SHADOW_LIGHT_DESC		ShadowDesc{};
	_float3 vRollPichYaw = { 45.f, 45.f, 0.f };

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_Matrices();
	void Transition(_float fTimeDelta);

public:
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner)PURE;
	virtual void Free() override;
};

NS_END
