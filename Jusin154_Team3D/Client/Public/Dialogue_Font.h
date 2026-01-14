
#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
class CNPCStat;
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
	void Npc_Dialogue(DIALOGUEINFO Info);

	void NextText();
	void NextLevel(CHOICEINFO Choice);
	void ENDText();
	void NextText(_int Index);
	void NpcDialogue(_int Index);
private:
	void NpcInfo(void* pArg);
	void NpcInteract(_bool bInteract);

	void NpcDialogue();
	void NpcDialogue(CNPCStat* Stat);

	void NpcNextText();

	void CHoice();

	void Shop();
	void SpellLearn();
	void Quest();
	void ReSet();
private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	class CDialogue* m_pDialogue = { nullptr };

	DIALOGUEINFO m_Info;

	vector<class CDialogue*> m_DialoguInfo;
	deque<class CDialogue*> m_pCurrentDialogue;
	vector<_int>  m_NextLevel;

	_bool	m_bNpcInteract = { false };
	_bool	m_bCurrentInteract = { false };
	_bool	m_bEndText = { false };
	_bool	m_bNextText = { false };

	CUnit*		m_pNpc = { nullptr };
	_wstring	m_pNpcName;
	_wstring	m_pName;
	_int		m_iTextID{};
	_int		m_iNextID{};
	_int		m_iQuestID{};
	_int		m_iType{};

	_bool		m_bTag{};
	_bool		m_bChoiceText = { false };
	_bool		m_bCurrentChoiceText = { false };
	_bool		m_bRace = { false };
	_bool		m_bBattle = { false };

	_float		m_fTime{};

public:
	static CDialogue_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END;
