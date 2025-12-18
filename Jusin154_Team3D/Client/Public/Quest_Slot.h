#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CQuest_Slot final : public CElementObject
{
	struct FontInfo
	{
		_wstring	pFontSizeName;
		_wstring	pQuestName;
		_float2		m_fFontSize{};
	};
private:
	CQuest_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Slot(const CQuest_Slot& rhs);
	virtual ~CQuest_Slot() = default;

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
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	void Hover();
	void Font_Size();
	void Font_Setting(_int Index);

public:
	void Set_QuestType(_int Index);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance* m_pVIBufferCom = { nullptr };
	CInfoInstance* m_pInfoInstance = { nullptr };
	CURRENTQUESTSECETINFO m_Info;
	_float	m_fOffSetX{};
	_float	m_fOffSetY{};
	_uint	m_iCols{};
	_int	m_iColor{};

	_int	m_iCurrentQeustClick{};
	_int	m_iQuestCount{};
	vector<FontInfo> m_Fonts;
public:
	static CQuest_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
