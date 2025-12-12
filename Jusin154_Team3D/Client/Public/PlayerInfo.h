#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CStat;
NS_END

NS_BEGIN(Client)

class CPlayerInfo final : public CBase
{
private:
	CPlayerInfo();
	~CPlayerInfo() = default;
public:
	CStat* Get_PlayerStatPtr();
	void Update(_float fTimeDelta);
	void Change_Level();

	void Update_CameraCoordinateSystem(_float3& vLook, _float3& vRight);
	pair<_float3, _float3> Get_CameraCoordinateSystem();
private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CInfoInstance*	m_pInfoInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CTransform*		m_pTransform = { nullptr };
	CStat*			m_pStat = { nullptr };
	_float3 m_vCameraLookDir = { 0.f, 0.f, 1.f, };
	_float3 m_vCameraRightDir = { 1.f, 0.f, 0.f };


private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);

public:
	static CPlayerInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
};

NS_END
