#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CMage_Nomal_Attack final : public CEffect_Container
{
private:
	CMage_Nomal_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMage_Nomal_Attack(const CMage_Nomal_Attack& rhs);
	virtual ~CMage_Nomal_Attack() = default;

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

	class CEffectParts*				  m_pCircle_Proj = { nullptr };
	class CEffectParts*				  m_pProj_PT1 = { nullptr };
	class CEffectParts*				  m_pProj_PT2 = { nullptr };

	_float3							  m_vLook = {};
	_float							  m_fLinearSpeed = 0.3f;

	_float4							  m_vStartPos = { 0.f, 0.f, 0.f, 1.f }; 
	_float4							  m_vTargetPos = { 0.f, 0.f, 10.f, 1.f };
private:
	class  CInfoInstance*			  m_pInfoInstance = { nullptr };
public:
	static CMage_Nomal_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
	void Describe_Entity() override;
};

NS_END
