#pragma once

#include "Client_Define.h"
#include "GameInstance.h"
#include "GameObject.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CTriggerBox final : public CBase
{
public:
	typedef struct tagTriggerDesc {
		_float4 vPosition_Radius = { 0.f, 0.f, 0.f, 1.f };
	}TRIGGERBOX_DESC;
private:
	CTriggerBox();
	virtual ~CTriggerBox() = default;

public:
	HRESULT TryScanArea(_float fTimeDelta);
	

private:
	HRESULT Initialize(TRIGGERBOX_DESC*);
	HRESULT Scan();
private:
	CGameInstance* m_pGameInstance = { nullptr };
	_float4 m_vPosition = { };
	_float	m_fRadius = {};
	_float2 m_vScanTimer = { 0.f, 0.5f };

public:
	static CTriggerBox* Create(TRIGGERBOX_DESC* Desc);
	virtual void Free() override;
};

NS_END
