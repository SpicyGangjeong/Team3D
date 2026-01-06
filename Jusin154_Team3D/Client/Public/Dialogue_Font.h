
#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CDialogue_Font final : public CGameObject
{
	typedef struct tagDialoueFontInfo
	{
		_wstring	pName;
		_wstring	pText;
		_float		fTime = 1.f;
	}DIALOGUFONTINFO;
private:
	CDialogue_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogue_Font(const CDialogue_Font& rhs);
	virtual ~CDialogue_Font() = default;

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
	void Add_Text(void* pArg);

private:
	void NpcInfo(void* pArg);
	void NpcInteract(_bool bInteract);

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	class CDialogue* m_pDialogue = { nullptr };

	vector<class CDialogue*> m_DialoguInfo;
	deque<class CDialogue*> m_pCurrentDialogue;

	_bool	m_bNpcInteract = { false };
	_bool	m_bCurrentInteract = { false };
	_wstring	m_pNpcName;
	_int		m_iTextID{};

public:
	static CDialogue_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END;
