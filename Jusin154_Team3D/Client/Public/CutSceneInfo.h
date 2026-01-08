#pragma once

#include "Client_Define.h"
#include "GameInstance.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END


NS_BEGIN(Client)

struct TimeLine {
	_float2						m_vTimer = {};
	_uint						m_eTypeTimeLine = {};
	list<class CTimeSocket*>	m_Sockets = {};
};

class CCutSceneInfo final : public CBase
{
private:
	CCutSceneInfo();
	~CCutSceneInfo() = default;

public:
	void Update(_float fTimeDelta);
	void Change_Level();

public:
	void		Active_Event(_string& strKey);
	HRESULT		DeActive_ActiveEvent(_string& strKey);
	void		Load_Events(LEVEL eLevel);

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CInfoInstance*	m_pInfoInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	map<_string, TimeLine> m_funcActiveEvents = {};
	map<_string, TimeLine> m_funcWaitEvents = {};

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	HRESULT Clear_ActiveEvents();	
	HRESULT Clear_Events(map<_string, TimeLine>&Events);

	HRESULT Clear_AllEvents();

public:
	static CCutSceneInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
};

NS_END
