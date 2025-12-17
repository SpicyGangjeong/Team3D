#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
class CCharacter_Controller;
NS_END

NS_BEGIN(Client)

class CAccio final : public CEffect_Container
{
private:
	CAccio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAccio(const CAccio& rhs);
	virtual ~CAccio() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual	HRESULT	Pre_Setting(CGameObject* pObject, void* pArg = nullptr) override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
	virtual void	OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr) override;

private:
	class  CInfoInstance* m_pInfoInstance = { nullptr };
private:
	_wstring			 m_wstrEffectName = {};

	class CTrailObject* m_pRope_Trail = { nullptr };
	class CEffectParts* m_pWandLight = { nullptr };
	class CEffectParts* m_pRotate0 = { nullptr };

	_float4				m_vStartPos = { 0.f, 0.f, 0.f, 1.f }; // 현재 발사 된 위치
	_float4				m_vTargetPos = { 0.f, 0.f, 0.f, 1.f }; // 현재 타게팅 된 위치
	_float				m_fLinearSpeed = 0.5f;
	_float3				m_vCameraLook = {};

	_float4x4				 m_TrailWorld = {};
	CCharacter_Controller*	 m_pEnemyCCT = { nullptr };

	_float				m_fAccRotateTime = {};
	_float				m_fTurnSpeed = { 65.f };
	_float				m_fRange = { 1.f };
	_float              m_fAttenuation = { 65.f };
	_bool				m_isDissolve = { true };
	_bool				m_isReverse = { false };
	_int				m_iLoopTime = { 5 };


public:
	static CAccio* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
