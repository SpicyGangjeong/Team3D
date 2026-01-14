#pragma once
#include "Client_Define.h"
#include "CamPosition.h"

NS_BEGIN(Client)

class CCamPosition_WorldLook final : public CCamPosition
{
public:
	typedef struct tagCameraWorldLook_DESC : public CCamPosition::CAMERAPOSITION_DESC
	{
		PSX::PxVec3 vPosition = {};
	}CameraWorldLook_DESC;
private:
	CCamPosition_WorldLook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamPosition_WorldLook(const CCamPosition_WorldLook& rhs);
	virtual ~CCamPosition_WorldLook() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Set_Position(PSX::PxVec3 vPosition);
	virtual void Set_Position(_float3 vPosition);
	virtual void Set_Position(_float4 vPosition);
	virtual void Set_Position(_fvector vPosition);
	virtual _vector Get_WorldPostion()override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	_float	m_fAccDegreeY = { };
#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
	unique_ptr<PrimitiveBatch<VertexPositionColor>> m_Batch = { nullptr };
#endif // _DEBUG
public:
	static CCamPosition_WorldLook* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamPosition_WorldLook* Clone(void* pArg, class CGameObject* pOWner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
