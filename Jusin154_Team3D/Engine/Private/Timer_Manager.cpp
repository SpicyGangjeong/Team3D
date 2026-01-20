#include "pch.h"
#include "Timer_Manager.h"

#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{

}

_float CTimer_Manager::Get_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::Compute_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

HRESULT CTimer_Manager::Add_Timer(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	/*m_Timers.insert({ strTimerTag, pTimer });*/

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

HRESULT CTimer_Manager::Initialize()
{
#ifdef _DEBUG
	if (FAILED(Add_Timer(TEXT("Timer_Render_Priority")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_Shadow")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_NonBlend")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_SSAO")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_SSAO_BLUR")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_LightAcc")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_Blur")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_Combined")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_Occlusion")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_EnvironmentPostProcess")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_Fog")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_Effect")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_NonLight")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_Blend")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_WeightBlend")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_PostProcessing")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_LastColor")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_Tone_Mapping")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_UI")))) {
		return E_FAIL;
	}
	if (FAILED(Add_Timer(TEXT("Timer_Render_UI_Overley")))) {
		return E_FAIL;
	}
#endif // _DEBUG
	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& strTimerTag)
{
	/*auto		iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));*/
	auto		iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;
	
	return iter->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	CTimer_Manager* pInstance = new CTimer_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTimer_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTimer_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Timers)
		SAFE_RELEASE(Pair.second);

	m_Timers.clear();

}
