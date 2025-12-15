#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUIObject;
NS_END

NS_BEGIN(Client)

class CSkill_Data final : public CBase
{
public:

private:
    CSkill_Data();
    ~CSkill_Data() = default;

public:
    void Update(_float fTimeDelta);
    void Change_Level();
    _float Get_CoolTime(_int SpellID);

public:
    HRESULT Load_SpellInfo(const _char* pFilePath);

    void NoCool(_bool bNoCool);

public:
    const SPELL_INFO& Get_Info(_uint SpellID) const;
    _int Update_Spell(_int SpellID);
    _float Get_Spell_Damage(_int SpellDamage);
    void Update_Damage(_float Damage);
    void Spell_UnLock(_int SpellID);

private:
    CInfoInstance* m_pInfoInstance = { nullptr };

    _int            m_iSpell_Count{};
    SPELL_INFO      SpellInfo[34] = {};
    _float          m_fSpell_CoolTime[34] = {};
    _float          m_fSpell_Damage[34] = {};

    _bool        m_bNoCool = { false };

private:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);


public:
    static CSkill_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END
