#pragma once

#include "Client_Define.h"
#include "Camera.h"
#include "TimeSocket.h"

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
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Trigger(class CTimeSocket& Socket)override;
	virtual HRESULT Render() override;
	void Update_LerpTimer(_float fTimeDelta);
	void Enable_FollowLerp();
	void Enable_LookLerp();
	void Sync_Follow(_bool bSync);
	void Toggle_Priority();
	void Toggle_AIPriority();
	void Set_LookTarget(CGameObject* pTarget);
	void Set_FollowTarget(CGameObject* pTarget);

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT Bind_ShaderResources() override;
	_bool	m_bSyncFollow = { false };
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
