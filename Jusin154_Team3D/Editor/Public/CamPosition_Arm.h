#pragma once
#include "Editor_Define.h"
#include "CamPosition.h"

NS_BEGIN(Editor)

class CCamPosition_Arm final : public CCamPosition
{
public:
	typedef struct tagCameraArm_Desc : public CCamPosition::CAMERAPOSITION_DESC
	{
		_float3		fEye{}, fAt{};
		_float		fMouseSensor{};
		_float		fDistance{};
	}CameraArm_DESC;
private:
	CCamPosition_Arm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamPosition_Arm(const CCamPosition_Arm& rhs);
	virtual ~CCamPosition_Arm() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	_bool	m_bMovable = true;
	_float	m_fMouseSensor = {};
	_float	m_fDistance = {};
	_float	m_fAccDegreeY = { };

public:
	static CCamPosition_Arm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamPosition_Arm* Clone(void* pArg, class CGameObject* pOWner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
