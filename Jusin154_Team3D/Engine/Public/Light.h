#pragma once
#include "Component.h"

NS_BEGIN(Engine)


class ENGINE_DLL CLight final : public CComponent
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLight(const CLight& rhs);
	virtual ~CLight() = default;

public:
	_bool operator==(const CLight& rhs) const;
	_bool operator==(const LIGHT_DESC& rhs) const;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	const LIGHT_DESC* Get_LightDesc() { return &m_LightDesc; }
	void Set_Color(_float4& vDiffuse, _float4& vAmbient, _float4& vSpecular);
public:
	HRESULT			  Render(class CShader* pShader, class CVIBuffer* pVIBuffer) const;

private:
	LIGHT_DESC			m_LightDesc = {};
public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg, CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
