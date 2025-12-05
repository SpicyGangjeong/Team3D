#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CEessential_Spell_Slot final : public CElementObject
{
private:
	CEessential_Spell_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEessential_Spell_Slot(const CEessential_Spell_Slot& rhs);
	virtual ~CEessential_Spell_Slot() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

	virtual void SizeUpX(_float fSizeX) override;
	virtual void SizeUpY(_float fSizeY) override;
	virtual void SizeUpdate(_float fSizeX, _float fSizeY) override;


private:
	void Hover();

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance* m_pVIBufferCom = { nullptr };

	_float m_fOffSetX{};
	_float m_fOffSetY{};
	_uint  m_iCols{};

	_float m_fHoverTime{};
	_int m_iPerSpell_Slot{};


public:
	static CEessential_Spell_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
