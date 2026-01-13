#pragma once

#include "Client_Define.h"
#include "Unit.h"
#include "TimeSocket.h"
NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CThestralCarriage final : public CUnit
{
public:
	enum class CARRIAGE_MESH_ORDER {
		CARRIAGE
		, WINDOWS
	};

	enum class CARRIAGE_SOCKET {
		BACK,
		BACK_LEFT,
		BACK_RIGHT,
		FRONT,
		FRONT_LEFT,
		FRONT_RIGHT,
		END
	};
private:
	CThestralCarriage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThestralCarriage(const CThestralCarriage& Prototype);
	virtual ~CThestralCarriage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Trigger(CTimeSocket& Socket)override;
	virtual _vector Get_WorldPostion()override;
	_matrix Get_SeatMatrix();
	_matrix Get_SocketWorldMatrix(CARRIAGE_SOCKET eSocket);
	void Lerp_Stop();
	void Lerp_Start(PSX::PxTransform pxTransform, _float fLerpTimer);
	void Lerp_Transform(_float fTimeDelta);

private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	_float4x4	m_LerpStartMatrix = { };
	_float4x4	m_LerpEndMatrix = { };
	_float2		m_vLerpTimer = { };
	_bool		m_bLerp = { false };

	_uint m_iAnimationIndex = 0;
#ifdef _DEBUG
	_bool m_bRender_WireFrame = { false };
	ID3D11RasterizerState* m_pRSS = { nullptr };
#endif // _DEBUG

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static CThestralCarriage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
