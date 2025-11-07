#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT On_Light(_uint iLevel, const _wstring& wstrLightKey, const LIGHT_DESC& LightDesc, class CLight** ppOut);
	HRESULT Off_Light(_uint iLevel, const _wstring& wstrLightKey);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer* pVIBuffer);
	HRESULT Change_Level(_uint iLevel);

private:
	map<_wstring, list<class CLight*>*>* m_pLights = { nullptr };;
	_uint m_iNumLevel = {};
private:
	list<class CLight*>* Find_Lights(_uint iLevel, const _wstring& wstrLightKey);
	HRESULT Initialize(_uint iNumLevel);

public:
	static CLight_Manager* Create(_uint iNumLevel);
	virtual void Free() override;
};

NS_END