#pragma once

#include "Base.h"
#include "Editor_Define.h"

NS_BEGIN(Editor)

typedef class CInformation final : public CBase
{
	DECLARE_SINGLETON(CInformation)
public:
	HRESULT Initialize_Info(ID3D11Device* pDevice = nullptr, ID3D11DeviceContext* pContext = nullptr);
	HRESULT Release_Info();
#pragma region WHOLEPART 
	_bool strListBox(const _char* Label, _int& iCurrentIndex, vector<_string>& contents);
	_bool wstrListBox(const _char* Label, _int& iCurrentIndex, vector<_wstring>& contents);

#pragma endregion // Àü¿ª

#pragma region OBJECTVIEWER

#pragma endregion
#pragma region MAPVIEWER

#pragma endregion
#pragma region EFFECTVIEWER

#pragma endregion
#pragma region UIVIEWER

#pragma endregion
#pragma region PHYSXVIEWER

#pragma endregion
#pragma region COMBINEDVIEWER

#pragma endregion

public:
#pragma region LOG
	void Log_Render();
	void Log_Push(const _char* pContents);
	void Log_Clear();
#pragma endregion

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	_string m_strLog = {};


private:
	CInformation();
	virtual ~CInformation() = default;
	virtual void Free() override;
}INFO;

NS_END