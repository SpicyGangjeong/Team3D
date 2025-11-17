#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CDummy_PhysXFreeDoor final : public CGameObject
{
public:
	typedef struct tagPhysXDummyDesc
	{
		_float3 vPos = { };
		_float3 vRotRPY = { };
		_uint iSubKind = { };
		_float fAngleLimit = {};
	}PHYSXDUMMY_DESC;
private:
	CDummy_PhysXFreeDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_PhysXFreeDoor(const CDummy_PhysXFreeDoor& rhs);
	virtual ~CDummy_PhysXFreeDoor() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	void Lerp_Matrix(Engine::_float fTime);
	void Lerp_NonMatrix(Engine::_float fTime);
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	PSX::PxRigidDynamic* m_pActor = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	
	_float3		m_vRadianYAngle = {};
	_float3		m_vKinematicTimer = { 0.f, 2.f, 2.3f };
	PSX::PxQuat m_pxStartQuat = {};
	_float4x4	m_InitialMatrix = {};


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;
	_float ClampRadian(_float fNewRadian);


public:
	static CDummy_PhysXFreeDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
