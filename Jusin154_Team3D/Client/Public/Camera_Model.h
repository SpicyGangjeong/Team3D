#pragma once

#include "Client_Define.h"
#include "Camera.h"

NS_BEGIN(Client)

class CCamera_Model final : public CCamera
{
public:
	typedef struct tagCamera_Model : public CCamera::CAMERA_DESC
	{
		class CUnit* pUnitPtr = { nullptr };
		class CUnit* pFollowTarget = { nullptr };
		class CUnit* pLookTarget = { nullptr };
	}CAMERA_MODEL_DESC;
private:
	CCamera_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Model(const CCamera_Model& rhs);
	virtual ~CCamera_Model() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CUnit*		m_pUnitPtr = { nullptr };
	class CModel*		m_pModelCom = { nullptr };
	class CShader*		m_pShaderCom = { nullptr };
	_bool m_bMovable = true;
	_float4x4 m_matInitial = {};
	const _float4x4* m_Ctrl1 = { nullptr };
	const _float4x4* m_Ctrl2 = { nullptr };
	const _float4x4* m_CtrlShake = { nullptr };
	const _float4x4* m_CtrlCam = { nullptr };
	const _float4x4* m_RootMatrix = { nullptr };
	const _float4x4* m_LookAtMatrix = { nullptr };
	const _float4x4* m_LookAtTargetMatrix = { nullptr };


#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
	unique_ptr<BasicEffect> m_BasicEffect;
	unique_ptr<PrimitiveBatch<VertexPositionColor>> m_Batch;
#endif // _DEBUG
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT Bind_ShaderResources() override;
	void Set_InitialPos();

public:
	static CCamera_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
