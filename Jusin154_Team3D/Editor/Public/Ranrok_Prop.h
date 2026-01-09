#pragma once

#include "Editor_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Editor)

class CRanrok_Prop final : public CEffect_Container
{
private:
	CRanrok_Prop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRanrok_Prop(const CRanrok_Prop& rhs);
	virtual ~CRanrok_Prop() = default;

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
	_wstring	 m_wstrEffectName = {};

	class CEditEffect* m_pSphere = { nullptr };
	class CEditEffect* m_pSphereLay = { nullptr };
	class CEditEffect* m_pSphereHitLay = { nullptr };
	class CEditEffect* m_pRing = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };

	_float		 m_fSizeAccTime = {};
	_float       m_fAmountSize = {};
	_float       m_fSpeed = {};


	_float       m_fHp = { 3.f };
	_bool		 m_isSizeLerpEnd = {};
private:
	class  CInfoInstance* m_pInfoInstance = { nullptr };
public:
	static CRanrok_Prop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
