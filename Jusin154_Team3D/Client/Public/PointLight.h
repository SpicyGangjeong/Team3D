#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Client)

class CPointLight final : public CGameObject
{
public:
	typedef struct tagPointLightDesc
	{
		_float3		vPosition;
		_float4		vDiffuse;
		_float4		vAmbient;
		_float4		vSpecular;
		_float3		vPosOffset;
		_float		fRange;
		_float		fLightAttRange;
		_float		fLightAttSpeed;
		_float		fPow;

	}POINTLIGHT_DESC;
private:
	CPointLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPointLight(const CPointLight& rhs);
	virtual ~CPointLight() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CLight*			m_pLightCom = { nullptr };

	_float 			m_fAttenuationTime = { 0.f };
	_float 			m_fLightIntensity = { 1.f };
	_float 			m_fLightIntensitySpeed = { 1.f };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT	Bind_ShaderResources()override;

public:
	static CPointLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
