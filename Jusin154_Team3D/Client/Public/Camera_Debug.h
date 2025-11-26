#pragma once

#include "Client_Define.h"
#include "Camera.h"

NS_BEGIN(Client)

class CCamera_Debug final : public CCamera
{
public:
	typedef struct tagCamera_Debug : public CCamera::CAMERA_DESC
	{
		_float3		vEye{}, vAt{};
		_float		fMouseSensor{};
	}CAMERA_DEBUG_DESC;
private:
	CCamera_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Debug(const CCamera_Debug& rhs);
	virtual ~CCamera_Debug() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bMovable = true;
	_float m_fMouseSensor = {};
	_float4x4 m_matInitial = {};

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT Bind_ShaderResources() override;
	void Set_InitialPos();

public:
	static CCamera_Debug* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
