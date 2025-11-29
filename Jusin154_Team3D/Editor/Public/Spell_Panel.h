#pragma once

#include "Editor_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CSpell_Panel final : public CPanelObject
{
private:
	CSpell_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpell_Panel(const CSpell_Panel& rhs);
	virtual ~CSpell_Panel() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Ready_Element(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr }; 

	CGameObject* m_pCurrent_Spell_Slot = { nullptr };
	CGameObject* m_pSpell_List = { nullptr };
	CGameObject* m_pEessential_Spell_Slot = { nullptr };
	CGameObject* m_pEessential_Spell = { nullptr };
	CGameObject* m_pSpell_List_Image = { nullptr };
	CGameObject* m_pSpell_State = { nullptr };
	CGameObject* m_pSpell_Hover = { nullptr };
	CGameObject* m_pSpell_Hover_Effect = { nullptr };
	CGameObject* m_pSpell_Preview = { nullptr };

public:
	static CSpell_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
