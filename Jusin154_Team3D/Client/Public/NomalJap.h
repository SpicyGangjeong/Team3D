#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CNomalJap final : public CEffect_Container
{
private:
	CNomalJap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNomalJap(const CNomalJap& rhs);
	virtual ~CNomalJap() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual	HRESULT	Pre_Setting(CGameObject* pObject , void* pArg = nullptr) override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
	virtual void	OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr) override;
private:
	class  CInfoInstance* m_pInfoInstance = { nullptr };
	_wstring						  m_wstrEffectName = {};

	_float4							  m_vJapData[4] = { _float4(105.f , 330.f , 35.f , 20.f)
													 , _float4(95.f , 100.f , 55.f , 200.f)
													 , _float4(95.f , 0.f , 0.f , 0.f)
													 , _float4(105.f , 250.f , 35.f , 60.f) };

	class CPartObject* m_pProjectile = { nullptr };
	class CPartObject* m_pProjectile_Side = { nullptr  };
	class CPartObject* m_pJapFire = { nullptr  };
	
	_float3 						  m_vRotateUp = {};
	_float							  m_fAccMoveTime = {};
	_float							  m_fAccRotateTime = {};
	_float							  m_fRotateAmount = { 0.2f };
	_float							  m_fTurnValue = {};
	_float							  m_fAngle = { XMConvertToRadians(285.f) };
	_float3							  m_vCameraLook = {};

	_float3							  m_vDirection = { 0.f, 0.f, 1.f };
	_float4							  m_vStartPos = { 0.f, 0.f, 0.f, 1.f }; // 현재 발사 된 위치
	_float4							  m_vTargetPos = { 0.f, 0.f, 10.f, 1.f }; // 현재 타게팅 된 위치
	_float							  m_fAngularSpeed = {};
	_float							  m_fLinearSpeed = 35.f;
	_float							  m_fTimeRate = {};

	/*좌우 무빙 관련 변수들*/
	_float3 						  m_vRotateRight = {};
	_float							  m_fZigZagAngle = { XMConvertToRadians(60.f) };
	_float							  m_fAccZigZagTime = {};
	_float							  m_fZigZagSpeed = { 25.f };
	_float							  m_fZigZagAmount = { 0.1f };
public:
	static CNomalJap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;

#endif // _DEBUG

};

NS_END
