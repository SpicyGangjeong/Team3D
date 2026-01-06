#pragma once

#include "Client_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CAction_Panel final : public CPanelObject
{
	typedef struct Get_Skill_Info
	{
		_int iSlotIndexX{};
		_int iSlotIndexY{};
		_int iSpellIndex{};
	}GETSKILLINFO;
private:
	CAction_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAction_Panel(const CAction_Panel& rhs);
	virtual ~CAction_Panel() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	void Magic_Meter_Visible(_uint iIndex, _bool bVisible);
	void Magic_Meter_Update();
	void Magic_Meter_UV();
	void Magic_Meter_Move();
	void Ancient_Magic_Throw();

	void Use_Spell(_int Index);
	void Spell_Setting(void* pArg);
	void Use_Potion();
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

	_float2 m_vMagic_MeterUV[5] = {};
	_int	m_iMagic_Meter_Count{};

	CGameObject* m_pSpell_Slot		= { nullptr };
	CGameObject* m_pSpell_Overlay1	= { nullptr };
	CGameObject* m_pSpell_Overlay2	= { nullptr };
	CGameObject* m_pSpell_Overlay3	= { nullptr };
	CGameObject* m_pSpell_Overlay4	= { nullptr };
	CGameObject* m_pSlot_Number		= { nullptr };
	CGameObject* m_pHpBarBG			= { nullptr };
	CGameObject* m_pMagic_Meter1	= { nullptr };
	CGameObject* m_pMagic_Meter2	= { nullptr };
	CGameObject* m_pMagic_Meter3	= { nullptr };
	CGameObject* m_pMagic_Meter4	= { nullptr };
	CGameObject* m_pLastMagic_Meter = { nullptr };
	CGameObject* m_pMagic_Icon		= { nullptr };
	CGameObject* m_pSpell_UI		= { nullptr };
	CGameObject* m_pPotion			= { nullptr };
	CGameObject* m_pMagic_Item		= { nullptr };

	vector<CGameObject*> m_Magic_Meters;
	//vector<CGameObject*> m_pSlot;

public:
	static CAction_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
