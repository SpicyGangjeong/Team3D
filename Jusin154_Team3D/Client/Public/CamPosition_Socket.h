#pragma once
#include "Client_Define.h"
#include "CamPosition.h"

NS_BEGIN(Client)

class CCamPosition_Socket final : public CCamPosition
{
public:
	typedef struct tagCamPosition_Player : public CCamPosition::CAMERAPOSITION_DESC
	{
		 const _float4x4* pSocketMatrices = {  };
	}CAMERAPOSITION_PLAYER_DESC;
private:
	CCamPosition_Socket(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamPosition_Socket(const CCamPosition_Socket& rhs);
	virtual ~CCamPosition_Socket() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources() override;
private:
	const _float4x4* m_pSocketMatrices = {  };

public:
	static CCamPosition_Socket* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamPosition_Socket* Clone(void* pArg, class CGameObject* pOWner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
