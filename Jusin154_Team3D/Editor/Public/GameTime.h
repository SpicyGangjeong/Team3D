#pragma once

#include "Editor_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CGameTime : public CBase
{
private:
	CGameTime();
	virtual ~CGameTime() = default;

public:
	_float Get_DayTime() const { return m_fDayTime; }

public:
	void		Update_GameTime(_float fTiemDelta);

private:
	CGameInstance*		m_pGameInstance = { nullptr };

	_float				m_PerDayTime = { 24.f };
	_float				m_fGameTime = { 0.f };
	_float				m_fDayTime = { 0.f };

private:
	HRESULT		Initialize();

public:
	static CGameTime* Create();
	virtual void Free() override;
};

NS_END
