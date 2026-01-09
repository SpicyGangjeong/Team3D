#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CEffect_Container;
class CTransform;
NS_END

NS_BEGIN(Client)

class CEffectInfo final : public CBase
{
private:
	CEffectInfo();
	~CEffectInfo() = default;
public:
	void Update(_float fTimeDelta);
	void Change_Level();

	HRESULT Regist_ActiveEffect(CEffect_Container* _pEffect);
	HRESULT Deregist_ActiveEffect(CEffect_Container* _pEffect);

	CEffect_Container* Get_LockOnEffect();
private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CInfoInstance* m_pInfoInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CEffect_Container* m_pLockOnEffect = { nullptr };
	list<class CEffect_Container*>	m_ActiveEffects= {};

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	HRESULT	Refresh_LockOnEffects();
	HRESULT Refresh_ActiveEffects();


public:
	static CEffectInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
};

NS_END
