#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUnit;
class CTransform;
NS_END

NS_BEGIN(Client)

class CInteractiveInfo final : public CBase
{
private:
	CInteractiveInfo();
	~CInteractiveInfo() = default;
public:
	void Update(_float fTimeDelta);
	void Change_Level();

	HRESULT Regist_ActiveInteractive(class CMapElement_Interactable* pUnit);
	HRESULT Deregist_ActiveInteractive(class CMapElement_Interactable* pUnit);

	HRESULT Ready_PoolingInteractive(); // Ready Pooling
	HRESULT ActiveAt_Interactive(_fvector vPosition); // Spawn Pooling Object

	class CMapElement_Interactable* Get_LockOnUnit();
private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CInfoInstance*	m_pInfoInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CMapElement_Interactable*			m_pLockOnInteractive = { nullptr };
	list<class CMapElement_Interactable*>	m_ActiveInteractive = {};

	list<class CMapElement_Interactable*>	m_PoolingInteractive = {};
	list<class CMapElement_Interactable*>	m_PoolingActiveInteractive = {};
private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	HRESULT	Refresh_LockOnTarget();
	

public:
	static CInteractiveInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
};

NS_END
