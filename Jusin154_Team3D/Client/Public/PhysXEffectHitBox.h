#pragma once

#include "Client_Define.h"
#include "GameObject.h"
#include "CallBack_EffectHitBox_Behavior.h"
#include "CallBack_EffectHitBox_HitReport.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CRigidBody;
NS_END

NS_BEGIN(Client)

class CPhysXEffectHitBox final : public CGameObject
{
public:
	typedef struct tagPhysXDummyDesc
	{
		_float3 vPos = {};
		_float3 vRotRPY = {};
		_uint   iSubKind = {};
		_float3 vDeltaPos = {};
		_float2 vLifeTime = {};
		_bool   bGravity = {};
	}PHYSXDUMMY_DESC;
private:
	CPhysXEffectHitBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXEffectHitBox(const CPhysXEffectHitBox& rhs);
	virtual ~CPhysXEffectHitBox() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Move(_float fTimeDelta);

private:
	CCharacter_Controller*				m_pCharacter_Controller = { nullptr };
	CModel*								m_pModelCom = { nullptr };
	CShader*							m_pShaderCom = { nullptr };
	CCallBack_EffectHitBox_Behavior*	m_pCallBack_Behavior = { nullptr };
	CCallBack_EffectHitBox_HitReport*	m_pCallBack_HitReport = { nullptr };
	_float3 m_vDeltaPos = {};
	_float2 m_vLifeTime = { };


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CPhysXEffectHitBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;

#endif // _DEBUG

};

NS_END
