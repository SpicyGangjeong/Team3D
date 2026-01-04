#pragma once

#include "Client_Define.h"
#include "Camera.h"

NS_BEGIN(Client)

class CCamera_Model final : public CCamera
{
public:
	typedef struct tagCamera_Model : public CCamera::CAMERA_DESC
	{
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
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	_bool m_bMovable = true;
	_float4x4 m_matInitial = {};

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
