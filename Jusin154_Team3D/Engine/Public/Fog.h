#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CFog final : public CBase
{
private:
	CFog();
	virtual ~CFog() = default;

public:
	void	Set_FogDensity(_float fFogDensity);
	void	Set_FogColor(_float4& vFogColor);

	HRESULT Bind_FogValue(class CShader* pShader);

#ifdef _DEBUG
	void Update_Fog();
#endif // _DEBUG


private:
	_bool		m_bVisible = {};
	_float		m_fFogDensity = { 0.04f };
	_float		m_fFogPow = { 0.1f };
	_float4		m_vFogColor = { 0.906f, 0.812f, 0.745f, 0.5f };

public:
	static CFog* Create();
	virtual void Free() override;
};

NS_END
