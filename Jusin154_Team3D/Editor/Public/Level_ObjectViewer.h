#pragma once

#include "Editor_Define.h"
#include "Level.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CLevel_ObjectViewer final : public CLevel
{
private:
	CLevel_ObjectViewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_ObjectViewer() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	/* 이 레벨에서 쓰기위한 객체들을 생성한다. */
	virtual HRESULT Initialize() override;
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Light();
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Dummy(const _wstring& strLayerTag);

	void Add_Object();

	void Add_Parts();
	void Show_ModelFilePath();
	void Category_ModelList(const _char* Category);
	void Category_PartsModelList(const _char* Category, const _char* Layer);
	void Show_AnimList();
	void Show_ObjectList();
	void Dummy_Object_Setting();
	void Parts_Object_Setting();
	void Save_LayerName(const _char* Layer);
	_wchar* Save_ModelName(const _char* Category);
	void Load_KeyFrame(const _char* Name);
	void Find_Anim();
	void Save_KeyFrame();

private:
	vector<CGameObject*>	m_Objects;
	vector<CGameObject*>	m_PartsObjects;
	class CRootModelPart*	m_HumanRoot = { nullptr };
	_int					m_iObjectIndex = { 0 };
	_int					m_iPartsObjectIndex = { 0 };
	_float					m_fKeyFrame = {};
	map<_string, _float>    m_KeyFrames;
	_char					m_szName[MAX_PATH];
	_wchar					m_wszName[MAX_PATH];
	_wchar					m_wszLayer[MAX_PATH];
	_wchar					m_wszPreLayer[MAX_PATH];
	_char					m_DummyPath[MAX_PATH];
	_char					m_FindAnimName[256] = {};
	string					m_SelectedKey;
	_int					m_iAnimIndex = {};

	class CPlayer* m_Test = { nullptr };
public:
	static CLevel_ObjectViewer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END
