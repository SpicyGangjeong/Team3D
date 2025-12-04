#pragma once

#include "Client_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

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

public:
	const SPELL_INFO Get_SpellInfo(_int SkillID);

private:
	void Slot_Chack(void* pArg);
	void Click_Slot(_bool Click);
	void Current_Slot_Chack(_int Index);


private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Ready_Element(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

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
	CGameObject* m_pSpell_Vidio_Border = { nullptr };
	CGameObject* m_pCurrent_Slot_Number = { nullptr };
	CGameObject* m_pSpell_Data= { nullptr };
	CGameObject* m_pSpell_Header = { nullptr };
	CGameObject* m_pSpell_Header_Line = { nullptr };
	CGameObject* m_pSpell_Anim = { nullptr };
	CGameObject* m_pSpell_Drag = { nullptr };

	_int		m_iHoverSlot[2] = { -1,-1 };

	SPELLINFO   m_Info[34] = {};
	CUIObject::HOVER_INFO	m_Hover;

	_int m_iPendingSpell = -1;
	_float m_fHoverTimer = 0.f;

	_int	m_iCurrent_Slot_Index{};
	_bool  m_bCurrentSlot_Hover = { false };

public:
	static CSpell_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
