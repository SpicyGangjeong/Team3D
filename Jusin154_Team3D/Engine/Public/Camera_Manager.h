#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCamera;

class CCamera_Manager final : public CBase
{
private:
	CCamera_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Clear_Cameras(_uint iLevel);
	HRESULT Release_Camera(_uint iLevel, const _wstring& wstrCameraKey);
	HRESULT Add_Camera(_uint iLevel, CCamera* pCamera, const _wstring& wstrCameraKey);
	HRESULT Bind_Camera(_uint iLevel, const _wstring& wstrCameraKey, _bool bIgnorePriority);
	HRESULT IsBinded_Camera(const _wstring& wstrCameraKey);


	void Force_CamPosition(_fvector vPos);
	const _float* Get_CurrentCameraFar();

private:
	ID3D11Device*								m_pDevice = { nullptr };
	ID3D11DeviceContext*						m_pContext = { nullptr };

	unordered_map<_wstring, CCamera*>*			m_Cameras = { nullptr };

	_uint										m_iLevelNumber = { };

	CCamera*									m_pCurrentMainCamera = { nullptr };
	_wstring									m_wstrCurrentCameraKey = {};

	_float										m_fFar = 500.f;

private:
	HRESULT Initialize(_uint iLevelNumber);
	CCamera* Find_Camera(_uint iLevel, const _wstring& strCameraKey);

public:
	static CCamera_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelNumber);
	virtual void Free()override;
};

NS_END
