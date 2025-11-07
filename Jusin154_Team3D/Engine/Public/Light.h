#pragma once
#include "Base.h"

NS_BEGIN(Engine)


class ENGINE_DLL CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	_bool operator==(const CLight& rhs) const;
	_bool operator==(const Engine::LIGHT_DESC& rhs) const;

public:
	HRESULT Replace_LightTo(_fvector vPosition);
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer* pVIBuffer) const;

private:
	LIGHT_DESC m_LightDesc{};
public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

NS_END