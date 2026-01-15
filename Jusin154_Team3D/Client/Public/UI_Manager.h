#pragma once

#include "Client_Define.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUI_Manager final : public CUIObject
{
private:
	CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Manager(const CUI_Manager& rhs);
	virtual ~CUI_Manager() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	//virtual HRESULT	Ready_Panel(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Canvas_Change(UI_STATE eType);
	void Clear_Canvas();
	virtual class CGameObject* Get_Canvas(const wstring& Name);
	virtual _int Canvas_Count();
	virtual const vector<wstring> Canvas_Name();

	void Add_Manager_Event(_wstring Name, function<void(void*)> Event);
	void Event_Callback(_wstring Name, void* pArg = nullptr);
private:
	void Add_Canvas(_wstring Name, class CGameObject* pCanvas);
	CGameObject* Find_Canvas(const _wstring& Name);

	void NpcInteract(void* pArg);
	void Chage();
	void Set_Fade();
	void Change_Map();
	void Set_Race(_bool bRace);
	void Set_Battle(_bool bBattle);
	void FadeIn(_float fAlphaVelue);
private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	CGameObject* m_pGamePlay_Canves = { nullptr };
	CGameObject* m_pSpell_Canvas = { nullptr };
	CGameObject* m_pMouse_Cursor = { nullptr };
	CGameObject* m_pQuest_Canvas = { nullptr };
	CGameObject* m_pSpellLearn_Canvas = { nullptr };
	CGameObject* m_pDialogue_Canvas = { nullptr };
	CGameObject* m_pCamera_LockOn = { nullptr };
	CGameObject* m_pDamage_Font = { nullptr };
	CGameObject* m_pDialogue_Font = { nullptr };
	CGameObject* m_pInteraction_Key = { nullptr };
	CGameObject* m_pNPCInteraction = { nullptr };
	CGameObject* m_pBroom_TargetGate = { nullptr };
	_bool			m_bCanvas_Change = { false };

	UI_STATE	m_eType{};

	_float		m_fAlphaVelue{};

	vector<class CGameObject*>			m_Canvases;			// 캔버스가 패널들을 관리 하기 위해서 필요함
	vector<wstring>						m_CanvasNames;		// 패널들의 이름을 전해주기 위해서 
	map<wstring, class CGameObject*>	m_Canvas_map;		// 캔버스에서 내가 원하는 패널을 찾기 위해서
	_int								m_iCanvas_Count{};	// 전체 패널의 갯수

	multimap<wstring, function<void(void*)>>		m_Event_map;

	_bool m_bCgangede = { false };
	_bool m_bSpellLearn = { false };

	_bool	m_bNPCInteract = { false };
	_bool	m_bCurrentNPCInteract = { false };

	_bool	m_bAlphaZero = { false };

	_bool	m_bRace = { false };
	_bool	m_bBattle = { false };
	_bool	m_bGate = { false };
	_float	m_fGateTime{};
public:
	static CUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END;
