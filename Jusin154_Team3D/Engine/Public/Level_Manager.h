#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLevel_Manager final
	: public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Change_Level(class CLevel* pNewLevel);
	_bool	Check_LevelShouldChange() const;
	void	Set_LevelToChange();
	_uint	Get_Current_LevelID();
	void	ResetLevel_Environment();
	_uint	Get_NextLevelID();
	void	Update(_float fTimeDelta);
	HRESULT Render();

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CLevel*			m_pCurrentLevel = { nullptr };
	_bool					m_bIsChangeLevel = { false };

public:
	static CLevel_Manager*	Create();
	virtual void			Free() override;
};

NS_END
