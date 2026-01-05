#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CRanrok_GroundPulse final : public CEffect_Container
{
private:
	CRanrok_GroundPulse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRanrok_GroundPulse(const CRanrok_GroundPulse& rhs);
	virtual ~CRanrok_GroundPulse() = default;

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


	_float3							  m_vLook = {};
	_float							  m_fLinearSpeed = 0.8f;

	_float4							  m_vStartPos = { 0.f, 0.f, 0.f, 1.f }; 
	_float4							  m_vTargetPos = { 0.f, 0.f, 10.f, 1.f };
private:
	class  CInfoInstance*			  m_pInfoInstance = { nullptr };
public:
	static CRanrok_GroundPulse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // DEBUG


};

NS_END
