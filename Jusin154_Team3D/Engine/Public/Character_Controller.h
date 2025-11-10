#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CCharacter_Controller final : public CBase
{

private:
	CCharacter_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCharacter_Controller() = default;

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	PSX::PxScene* m_pScene = { nullptr };
private:
	HRESULT Initialize(PSX::PxScene* pScene);


public:
	static CCharacter_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PSX::PxScene* pScene);
	virtual void Free() override;
};

NS_END