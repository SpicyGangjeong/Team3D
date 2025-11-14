#include "pch.h"

#include "Information.h"

IMPLEMENT_SINGLETON(CInformation)


HRESULT CInformation::Initialize_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);



	return S_OK;
}

HRESULT CInformation::Release_Info()
{
	if (nullptr == s_pInstance) {
		return S_OK;
	}
	DestroyInstance();

	//SAFE_RELEASE();
	//SAFE_RELEASE();

	return S_OK;
}

_bool CInformation::strListBox(const _char* Label, _int& iCurrentIndex, vector<_string>& contents)
{
	assert(iCurrentIndex < contents.size());
	assert(iCurrentIndex >= 0);
	return GUI::ListBox(Label, &iCurrentIndex, [](void* data, _int iIndex)->const _char*
				{
					auto vec = static_cast<vector<_string>*>(data);
					return (*vec)[iIndex].c_str();
				}, &contents, (_int)(contents.size())
			);
}

_bool CInformation::wstrListBox(const _char* Label, _int& iCurrentIndex, vector<_wstring>& contents)
{
	assert(iCurrentIndex < contents.size());
	assert(iCurrentIndex >= 0);
	return GUI::ListBox(Label, &iCurrentIndex, [](void* data, _int iIndex)->const _char*
		{
			auto vec = static_cast<vector<_wstring>*>(data);
			return CMyTools::ToString((*vec)[iIndex]).c_str();
		}, &contents, (_int)(contents.size())
			);
}

void CInformation::Log_Render()
{
	GUI::Begin("Log");
	GUI::Text(m_strLog.c_str());
	GUI::End();
}

void CInformation::Log_Push(const _char* pContents)
{
	m_strLog.append(_string("\n") + pContents);
}

void CInformation::Log_Clear()
{
	m_strLog.clear();
}

CInformation::CInformation()
{
}

void CInformation::Free()
{
	__super::Free();

	

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
