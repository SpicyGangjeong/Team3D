#include "pch.h"
#include "CutSceneInfo.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Monster.h"
#include "Layer.h"
#include "Player.h"
#include "TimeSocket.h"

CCutSceneInfo::CCutSceneInfo()
{
}
void CCutSceneInfo::Update(_float fTimeDelta)
{
	_float fRatio = 0.f;
	for (map<_string, TimeLine*>::iterator iter = m_funcActiveEvents.begin(); iter != m_funcActiveEvents.end();) {
		list<CTimeSocket*>* pSockets = &iter->second->m_Sockets;
		iter->second->m_vTimer.x += fTimeDelta;
		fRatio = CMyTools::Saturate(iter->second->m_vTimer.x / iter->second->m_vTimer.y);
		for (list<CTimeSocket*>::iterator socketIter = pSockets->begin(); socketIter != pSockets->end();) {
			GUI::Begin("CutScene");
			if (GUI::TreeNodeEx((*socketIter)->m_Contents.strEventName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen)) {
				GUI::Text("fRatio : %.2f", fRatio);
				GUI::Text("fSecond : %.2f / %.2f", iter->second->m_vTimer.x, iter->second->m_vTimer.y);
				GUI::TreePop();
			}
			GUI::End();
			if ((*socketIter)->Trigger(fRatio))
			{
				SAFE_RELEASE((*socketIter));
				socketIter = pSockets->erase(socketIter);
			}
			else
			{
				break;
			}
		}
		if (pSockets->empty()) {
			Safe_Delete((*iter).second);
			iter = m_funcActiveEvents.erase(iter);
		}
		else {
			iter++;
		}
	}
}

void CCutSceneInfo::Change_Level()
{
	_uint iLevel = m_pGameInstance->Get_NextLevelID();

	Clear_ActiveEvents();
}

void CCutSceneInfo::Active_Event(_string& strKey)
{
	map<_string, TimeLine*>::iterator iter = m_funcWaitEvents.find(strKey);
	if (iter != m_funcWaitEvents.end()) {
		m_funcActiveEvents.emplace(iter->first, iter->second);
		m_funcWaitEvents.erase(iter);
	}
}

HRESULT CCutSceneInfo::DeActive_ActiveEvent(_string& strKey)
{
	map<_string, TimeLine*>::iterator iter = m_funcActiveEvents.find(strKey);
	if (iter != m_funcActiveEvents.end()) {

		for (list<CTimeSocket*>::iterator socketIter = (*iter).second->m_Sockets.begin(); 
			socketIter != (*iter).second->m_Sockets.end(); ++socketIter) {
			SAFE_RELEASE((*socketIter));
		}
		Safe_Delete((*iter).second);
		m_funcActiveEvents.erase(iter);
	}
	return S_OK;
}

void CCutSceneInfo::Load_Events(pair<_string, TimeLine*>& pairTimeLine)
{
	m_funcWaitEvents.emplace(pairTimeLine);
}

HRESULT CCutSceneInfo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pInfoInstance = CInfoInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContex;

	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pInfoInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);

	return S_OK;
}
HRESULT CCutSceneInfo::Clear_ActiveEvents()
{
	return Clear_Events(m_funcActiveEvents);
}
HRESULT CCutSceneInfo::Clear_Events(map<_string, TimeLine*>& Events)
{
	map<_string, TimeLine*>::iterator iter = Events.begin();
	for (; iter != Events.end();) {

		for (list<CTimeSocket*>::iterator socketIter = (*iter).second->m_Sockets.begin();
			socketIter != (*iter).second->m_Sockets.end(); ++socketIter) {
			SAFE_RELEASE((*socketIter));
		}
		Safe_Delete((*iter).second);
		iter = Events.erase(iter);
	}
	return S_OK;
}
HRESULT CCutSceneInfo::Clear_AllEvents()
{
	Clear_Events(m_funcActiveEvents);
	Clear_Events(m_funcWaitEvents);

	return E_NOTIMPL;
}
CCutSceneInfo* CCutSceneInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	CCutSceneInfo* pInstance = new CCutSceneInfo();

	if (FAILED(pInstance->Initialize(pDevice, pContex))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}
void CCutSceneInfo::Free()
{
	__super::Free();

	Clear_AllEvents();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
