#pragma once

#include "Client_Define.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CLoding_Panel final : public CUIObject
{

private:
    CLoding_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLoding_Panel(const CLoding_Panel& rhs);
    virtual ~CLoding_Panel() = default;

public:
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render() override;
    virtual _vector Get_WorldPostion() override;


private:
    virtual HRESULT    Bind_ShaderResources() override;
    virtual HRESULT    Ready_Components(void* pArg) override;
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    void Set_Image(_int Index);
    void ToolTipSetting();

private:
    CTexture* m_pDiffuse_TextureCom = { nullptr };
    CTexture* m_pDiffuse_TextureCom1 = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    CGameObject* m_pCurrent_Spell_Slot = { nullptr };
    CGameObject* m_pLoadingWidget = { nullptr };


    _int    m_iImageNum{};
    _float4 m_vImageposSi{};

    _wstring ToolTip[5];
    _int m_iToolTip_Count{};

#ifdef _DEBUG
    _bool INHYUK = { false };
    _bool JINHO = { false };
    _bool HYUNBIN = { false };
    _bool NURI = { false };
    _bool JINWOO = { false };
    _int human{};
#endif

public:
    static CLoding_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
    virtual void Free() override;
#ifdef _DEBUG
    void Describe_Entity() override;

#endif // _DEBUG

};

NS_END
