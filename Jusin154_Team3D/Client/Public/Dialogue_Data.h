#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUIObject;
NS_END

NS_BEGIN(Client)

class CDialogue_Data final : public CBase
{
public:

private:
    CDialogue_Data();
    ~CDialogue_Data() = default;

public:
    void Update(_float fTimeDelta);

public:
    HRESULT Load_SpellInfo(const _char* pFilePath);

public:
    const CURRENTDIALOGUEINFO& Get_Info(_wstring NpcName, _int iTextID) const;

private:
    CInfoInstance*              m_pInfoInstance = { nullptr };

    _int                        m_iDialogue_Count{};
    map<_wstring, NPCDIALOGUEINFO>     m_DialogueInfo;
    CURRENTDIALOGUEINFO             Dummy{};
private:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);


public:
    static CDialogue_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END
