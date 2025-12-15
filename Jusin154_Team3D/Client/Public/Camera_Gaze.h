#pragma once

#include "Client_Define.h"
#include "Camera.h"

NS_BEGIN(Client)

class CCamera_Gaze final : public CCamera
{
public:
	typedef struct tagCamera_Gaze : public CCamera::CAMERA_DESC
	{
	}CAMERA_GAZE_DESC;
private:
	CCamera_Gaze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Gaze(const CCamera_Gaze& rhs);
	virtual ~CCamera_Gaze() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	void Update_LerpTimer(Engine::_float fTimeDelta);
	void Enable_FollowLerp();
	void Enable_LookLerp();
	void Toggle_Priority();
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT Bind_ShaderResources() override;

private:
	CInfoInstance* m_pInfoInstance = { nullptr };


public:
	static CCamera_Gaze* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
	virtual void Free() override;


#ifdef _DEBUG
	void Describe_Entity() override;

#endif // _DEBUG
};

NS_END
