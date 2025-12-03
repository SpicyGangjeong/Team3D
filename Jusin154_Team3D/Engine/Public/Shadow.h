#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{
private:
	CShadow();
	virtual ~CShadow() = default;

public:
	HRESULT Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc);
	HRESULT Bind_Shadow_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType) const;
	const _float4x4* Get_ShadowMatricesPtr() { return m_TransformationMatrices; }
	const SHADOW_LIGHT_DESC* Get_ShadowDesc() { return &m_Desc; }
private:
	_float4x4 m_TransformationMatrices[ENUM_CLASS(D3DTS::END)] = {};
	SHADOW_LIGHT_DESC m_Desc = {};

public:
	static CShadow* Create();
	virtual void Free() override;
};

NS_END
