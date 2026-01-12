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

private:
	CInfoInstance* m_pInfoInstance = { nullptr };
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
