#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CLevioso final : public CEffect_Container
{
private:
	CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLevioso(const CLevioso& rhs);
	virtual ~CLevioso() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual	HRESULT	Pre_Setting(CGameObject* pObject) override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
	virtual void	OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr) override;
private:
	_wstring	     m_wstrEffectName = {};
	class CPlayer*	 m_pPlayer = {};

	class CEffectParts*  m_pLeviosoPJ_0   = { nullptr };
	class CEffectParts*  m_pLeviosoPJ_1   = { nullptr };
	class CTrailObject*  m_pLeviosoTrail  = { nullptr };
	class CEffectParts*  m_pTrail_PT_0 = { nullptr };

	_vector				m_vOwnerLook = {};
	_float				m_fAccRotateTime = {};
	_float				m_fTurnSpeed = {};
	_float				m_fRange = {};
public:
	static CLevioso* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
