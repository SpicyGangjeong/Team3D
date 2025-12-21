#pragma once

#include "Client_Define.h"
#include "Base.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUIObject;
NS_END

NS_BEGIN(Client)

class CSpellLearn_Data final : public CBase
{
private:
	CSpellLearn_Data();
	~CSpellLearn_Data() = default;

public:
	void Update(_float fTimeDelta);
public:
	HRESULT Load_SpellInfo(const _char* pFilePath);\


public:
	const SPELLLEARNINFO& Get_SpellLearn(_int Index) const;
	_int Get_Index();

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	_int m_iSpellLearn_Count{};
	vector<SPELLLEARNINFO> m_SpellLearnInfos;

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);

public:
	static CSpellLearn_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
