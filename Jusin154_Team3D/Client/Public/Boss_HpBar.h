#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CBoss_HpBar final : public CElementObject
{
private:
	CBoss_HpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_HpBar(const CBoss_HpBar& rhs);
	virtual ~CBoss_HpBar() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

public:
	void Hit(_float fTimeDelta);
	void Update_Target();

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CInfoInstance* m_pInfoInstance = { nullptr };

	_float				m_fMaxHp{};
	_float				m_fCurrentHp{};
	_float				m_fHpBar{};
	_float				m_fDamage{};
	_float				m_fTargetHp{};

	_bool				m_bAnimation = { false };
public:
	static CBoss_HpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
