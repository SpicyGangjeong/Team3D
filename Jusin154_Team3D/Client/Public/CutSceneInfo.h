#pragma once

#include "Client_Define.h"
#include "GameInstance.h"

NS_BEGIN(Engine)
class CGameObject;
class CCamera;
NS_END


NS_BEGIN(Client)

class CCutSceneInfo final : public CBase
{
private:
	CCutSceneInfo();
	~CCutSceneInfo() = default;

public:
	void Update(_float fTimeDelta);
	void Load_CutScenes();

public:
	void		Active_Event(_string& strKey);
	HRESULT		DeActive_ActiveEvent(_string& strKey);
	void		Load_Events(pair<_string, TimeLine*>& pairTimeLine);
	HRESULT		Clear_AllEvents();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CInfoInstance*	m_pInfoInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	map<_string, TimeLine*> m_funcActiveEvents = {};
	map<_string, TimeLine*> m_funcWaitEvents = {};

private:
	void Update_ActiveEvents(_float fTimeDelta);
	void Update_WaitEvents(_float fTimeDelta);
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	void Load_CutSceneXML(const string& path);

#ifdef _DEBUG
public:
#endif // _DEBUG
	HRESULT Clear_ActiveEvents();
	HRESULT Clear_Events(map<_string, TimeLine*>&Events);


	HRESULT Ready_GameplayCutScenes();
	HRESULT Ready_FieldCutScenes();

public:
	static CCutSceneInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity();
#endif // _DEBUG

};

NS_END
