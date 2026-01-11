#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CLevioso final : public CEffect_Container
{
private:
	CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLevioso(const CLevioso& rhs);
	virtual ~CLevioso() = default;

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
	_wstring	     m_wstrEffectName = {};
	class CPlayer*	 m_pPlayer = {};

	class CEffectParts* m_pLeviosoPJ = { nullptr };

	_float4				m_vStartPos = { 0.f, 0.f, 0.f, 1.f }; // 현재 발사 된 위치
	_float4				m_vTargetPos = { 0.f, 0.f, 10.f, 1.f }; // 현재 타게팅 된 위치
	_float				m_fLinearSpeed = 0.75f;
	_float3				m_vCameraLook = {};
public:
	static CLevioso* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
