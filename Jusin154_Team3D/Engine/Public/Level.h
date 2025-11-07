#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLevel abstract 
	: public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelID);
	virtual ~CLevel() = default;

public:
			_uint	Get_LevelID() const { return m_iID; }
	virtual _uint	Get_NextLevelID() { return m_iID; } // 로딩레벨일 때만 의미가 있음
	virtual void	Update(_float fTimeDelta)PURE;
	virtual HRESULT Render()PURE;
protected:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_uint					m_iID = {};

protected:
	virtual HRESULT Initialize()PURE;

public:
	virtual void Free() override;
};

NS_END