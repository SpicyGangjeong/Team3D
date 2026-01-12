#pragma once

#include "Editor_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Editor)

class CRevelio final : public CEffect_Container
{
private:
	CRevelio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRevelio(const CRevelio& rhs);
	virtual ~CRevelio() = default;

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
	_wstring	     m_wstrEffectName = {};
	class CPlayer*	 m_pPlayer = {};

	class CEditEffect* m_pRevelioPT_Y = { nullptr };
	class CEditEffect* m_pRevelioPT_B = { nullptr };
	class CEditEffect* m_pRevelioPT_R = { nullptr };
	class CEditEffect* m_pWand_Light = { nullptr };

public:
	static CRevelio* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
