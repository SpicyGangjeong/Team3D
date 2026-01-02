#pragma once

#include "Client_Define.h"
#include "PartObject.h"
#include "Player_ENUM.h"

NS_BEGIN(Client)

class CPlayerRobe final : public CPartObject
{
	typedef struct tagRouteCreater {
		_uint iTargetRouteIndex = UINT_MAX; // 만들려는 루트
		_uint iStartBoneIndex = UINT_MAX;	// 시작 앵커
		_uint iEndBoneIndex = UINT_MAX;		// 끝 앵커
		_uint iStartJointIndex = UINT_MAX;	// 시작 앵커가 할당받을 인덱스
		_uint iEndJointIndex = UINT_MAX;	// 끝 앵커가 할당받을 인덱스
	}ROUTE_DESC;

public:
	typedef struct tagPlayerRobe : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = {  };
		CModel* pModel = { nullptr };
	}PlayerRobe_DESC;
private:
	CPlayerRobe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerRobe(const CPlayerRobe& Prototype);
	virtual ~CPlayerRobe() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Update_RobeSocketPosition();
	vector<_float4x4> Get_RobeJointAnchorMatrix() const { return m_ReconstructedJointAnchorMatirces; }
	_float4x4 Get_RobeJointAnchorMatrix(_uint iIndex) const { return m_ReconstructedJointAnchorMatirces[iIndex]; }
private:
	_float4x4  m_PreTransformMatrix = { };
	const _float4x4* m_pSocketMatrix			= { nullptr };
	const _float4x4* m_pLeftFootLocalMatrix		= { nullptr };
	const _float4x4* m_pLeftLegLocalMatrix		= { nullptr };
	const _float4x4* m_pRightFootLocalMatrix	= { nullptr };
	const _float4x4* m_pRightLegLocalMatrix		= { nullptr };
	const _float4x4* m_RobeJointAnchorMatrices[ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END)] = { nullptr };
	vector<_float4x4> m_ReconstructedJointAnchorMatirces = { };
	//_float4x4 m_ReconstructedJointAnchorMatirces[ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END)] = { };

	CModel*				m_pModelCom = { nullptr };
	_int				m_iBoneNum = -1;
	CRigidBody_Dynamic* m_pRightLeg = { nullptr };
	CRigidBody_Dynamic* m_pLeftLeg = { nullptr };
	CRigidBody_Dynamic* m_pLower0Bound = { nullptr };
	CRigidBody_Dynamic* m_pLower1Bound = { nullptr };
	CRigidBody_Dynamic* m_pRobeMainAnchor = { nullptr };
	CRigidBody_Dynamic* m_pRobeJointRoute[ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::END)] = { nullptr };
	CRigidBody_Dynamic* m_pRobeJointAnchor[ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END)] = { nullptr };
	PSX::PxD6Joint*		m_pDynamicJoints[ENUM_CLASS(PLAYER_JOINT_ORDER::END)] = { nullptr };
	D6JOINTDESC			m_JointDescriptions[ENUM_CLASS(PLAYER_JOINT_ORDER::END)] = { };
	
	vector<_float4x4>	m_PrevRobeBoneMatrices = { };
#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pMainShape = { nullptr };
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
	ID3D11DepthStencilState* m_pDepthStencilStateNone = { nullptr };
	_float4x4 m_LeftLegMatrix = {};
	_float4x4 m_RightLegMatrix = {};
#endif // _DEBUG
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources() { return S_OK; }

	HRESULT Update_LegsPosition();
	HRESULT Helper_RouteJointGenerater(CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC& Desc_Body, ROUTE_DESC& Desc_Route, _matrix* xmAnchorMatricesWorld);
#ifdef _DEBUG
	HRESULT Render_BonePhysX();
	HRESULT Render_Legs();
	HRESULT Update_RobeJoints();
#endif // _DEBUG
public:
	static CPlayerRobe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
