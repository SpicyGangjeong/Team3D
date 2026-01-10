#pragma once

#include "Client_Define.h"
#include "GameInstance.h"
NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CTriggerBox final : public CBase
{
private:
	CTriggerBox();
	virtual ~CTriggerBox() = default;



public:
	static CTriggerBox  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg);
	virtual void Free() override;
};

NS_END
