#pragma once

#include "Editor_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Loading();
	void Output();

public:
	_bool isFinished() const {
		return m_isFinished;
	}

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL				m_eNextLevelID = { LEVEL::END };

	HANDLE				m_hThread = {};

	_wstring			m_strMessage = {};
	_bool				m_isFinished = { false };

	CRITICAL_SECTION	m_CriticalSection = {};

	CGameInstance* m_pGameInstance = { nullptr };

	//list<SaveModel> m_SaveModel;

private:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_ObjectViewer();
	HRESULT Loading_For_UI();
	HRESULT Loading_For_Effect();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END