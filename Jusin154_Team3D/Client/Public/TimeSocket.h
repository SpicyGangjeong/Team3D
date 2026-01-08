#pragma once
#include "Client_Define.h"
#include "GameInstance.h"


NS_BEGIN(Engine)
class CGameObject;
NS_END


NS_BEGIN(Client)

typedef enum class typeTimeSocket_Param {
	TARGET,
	TRANSFORM,
	END
}TIMESOCKET_PARAM;

typedef enum class typeTimeSocket_Func {
	TRIGGER,
	POSITION,
	TAG,
	TARGETTING,
	END
}TIMESOCKET_FUNC;

class CTimeSocket final : public CBase
{
private:
	CTimeSocket();
	virtual ~CTimeSocket() = default;
public:
	_float				m_fRatio = { 0.f };
	_bool				m_bTriggerred = { false };
	CGameObject*		m_pEventTarget = { nullptr };
	_string				m_strEventName = {};
	TIMESOCKET_PARAM					m_eTypeParam = {};
	union {
		CGameObject*		m_pOtherTarget = { nullptr };
		PSX::PxTransform	m_pxTransform;
	};

	virtual _bool Trigger(_float fRatio);

public:
	HRESULT Initialize(CGameObject* pTarget, _string& strTag);
	HRESULT Initialize(CGameObject* pTarget, _string& strTag, PSX::PxTransform& pxTransform);
	HRESULT Initialize(CGameObject* pTarget, _string& strTag, CGameObject* pOther);

public:
	static CTimeSocket* Create();
	virtual void Free() override;

private:
	function<void(CTimeSocket&)>		m_funcEvent = { nullptr };
private:
	void Finalize();
};

NS_END
