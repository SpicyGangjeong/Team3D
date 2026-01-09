#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CDialogue_Choice final : public CElementObject
{
private:
	CDialogue_Choice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogue_Choice(const CDialogue_Choice& rhs);
	virtual ~CDialogue_Choice() = default;

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
	void Mouse_Hover();
	void Up();
	void Text(_wstring pText);
	void Type(_int Index, _int Type);
	void Reset();

private:
	void Hover(_bool iHover);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float4		m_vImagePosi1{};
	_float4		m_vImagePosi2{};
	_float4		m_vImagePosi3{};

	_int		m_iHover{};
	_int		m_iType{};

	_wstring	m_pText;

	CHOICEINFO		m_ChoiceInfo;

public:
	static CDialogue_Choice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
