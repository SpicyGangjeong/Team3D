#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CDecendo final : public CEffect_Container
{
private:
	CDecendo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecendo(const CDecendo& rhs);
	virtual ~CDecendo() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual	HRESULT	Pre_Setting(CGameObject* pObject, void* pArg = nullptr) override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
	virtual void	OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr) override;
private:
	_wstring						  m_wstrEffectName = {};
	class  CInfoInstance*			  m_pInfoInstance = { nullptr };

	class CPartObject*				  m_pProjectile = {};
	class CPartObject*				  m_pProjectile_Blur = {};

	_float4							  m_vStartPos = { 0.f, 0.f, 0.f, 1.f }; // 현재 발사 된 위치
	_float4							  m_vTargetPos = { 0.f, 0.f, 10.f, 1.f }; // 현재 타게팅 된 위치
	_float							  m_fLinearSpeed = 22.f;
	_float3							  m_vCameraLook = {};

	_float3							  m_vRotateUp = {};
	_float							  m_fRotateAccTime = {};
	_float							  m_fLerpAmount = {};
	_float							  m_fTurnValue = {};

	_float							  m_fAngularSpeed = {};
	_float							  m_fTimeRate = {};
public:
	static CDecendo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
