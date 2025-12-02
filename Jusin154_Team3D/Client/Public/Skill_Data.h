#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CSkill_Data final : public CGameObject
{
public:
	typedef struct tagSpellInfo
	{
		_int		iSpell_ID{};
		_string		pSpell_Name;
		_string		pImage_Name;
		_int		iSpell_Type{};
		_int		iSkill_Type{};
		_float		fSpell_CoolTime{};
		_float		fDuration{};
		_int		iAnimNum{};
		_string		pSpellInfo;
		_bool		bSpell_Lock = false;
		_bool		bEquip_Spell = false;
	}SPELLINFO;
private:
	CSkill_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Data(const CSkill_Data& rhs);
	virtual ~CSkill_Data() = default;

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

public:
	//const SPELLINFO& Get_Info(_uint SpellID) const;

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CGameObject* m_pSpell_Panel = { nullptr };

	SPELLINFO SpellInfo = {};
public:
	static CSkill_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
