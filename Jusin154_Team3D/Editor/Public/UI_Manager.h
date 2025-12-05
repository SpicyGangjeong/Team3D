#pragma once

#include "Editor_Define.h"
#include "UIObject.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Editor)

class CUI_Manager final : public CUIObject
{
public:
	enum class Canvases { GAMEPLAYER_CANVAS, SPELL_CANVAS };

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
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Canvas_Change(Canvases m_eType);
	void Clear_Canvas();
	virtual class CGameObject* Get_Canvas(const wstring& Name);
	virtual _int Canvas_Count();
	virtual const vector<wstring> Canvas_Name();

	void Add_Manager_Event(_wstring Name, function<void(void*)> Event);
	void Event_Callback(_wstring Name, void* pArg = nullptr);
private:
	void Add_Canvas(_wstring Name, class CGameObject* pCanvas);
	CGameObject* Find_Canvas(const _wstring& Name);

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pGamePlay_Canves = { nullptr };
	CGameObject* m_pSpell_Canvas = { nullptr };
	CGameObject* m_pLoding_Canvas= { nullptr };
	CGameObject* m_pMouse_Cursor = { nullptr };
	CGameObject* m_pCamera_LockOn = { nullptr };
	_bool			m_bCanvas_Change = { false };

	Canvases	m_eType{};

	vector<class CGameObject*>			m_Canvases;			// 캔버스가 패널들을 관리 하기 위해서 필요함
	vector<wstring>						m_CanvasNames;		// 패널들의 이름을 전해주기 위해서 
	map<wstring, class CGameObject*>	m_Canvas_map;		// 캔버스에서 내가 원하는 패널을 찾기 위해서
	_int								m_iCanvas_Count{};	// 전체 패널의 갯수

	multimap<wstring, function<void(void*)>>		m_Event_map;


public:
	static CUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;

};

NS_END;
