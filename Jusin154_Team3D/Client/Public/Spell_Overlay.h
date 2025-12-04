#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CSpell_Overlay final : public CElementObject
{
private:
	CSpell_Overlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpell_Overlay(const CSpell_Overlay& rhs);
	virtual ~CSpell_Overlay() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	_float4 Compute_UI(_uint SpellID);
	void Array_Clear(_int Value);
	void Bool_Array_Clear(_bool Bool);

public:
	void Spell_Setting(_int SlotIndex, _int SpellIndex);
	void Set_SpellSlot(_int Index);
	void Use_Spell();

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CTexture* m_pDiffuse_TextureCom3 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_int m_iImageFrameX{};
	_int m_iImageFrameY{};
	_float m_fFrame{};
	_float4 m_vUV[4] = {};

	_int m_iSlotCount{};
	_int m_Spells[4] = { 0 };
	_int m_iSpellType[4] = { 0 };
	_int m_iCurrent_Slot{};

	_bool m_bEquipped[4] = { false };
	_bool m_bSpellUsed[4] = { false };

public:
	static CSpell_Overlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
