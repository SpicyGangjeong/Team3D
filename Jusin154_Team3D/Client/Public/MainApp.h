#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();
	void Compute_FrameCount();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CInfoInstance* m_pInfoInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	HRESULT Ready_Default_Setting();
	HRESULT Start_Level(LEVEL eLevelID);
	HRESULT Ready_Prototypes();
	HRESULT Ready_IMGUI();
	HRESULT Release_IMGUI();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

NS_END



