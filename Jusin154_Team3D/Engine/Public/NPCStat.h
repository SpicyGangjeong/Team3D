#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNPCStat final : public CComponent
{
	typedef struct tagNpcInfo
	{
		_int			iID{};
		_wstring		pNpc_Name;
		_int			iNpcType{};
		_float			fFontPos{};
	}NPCINFO;

private:
	CNPCStat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCStat(const CNPCStat& rhs);
	virtual ~CNPCStat() = default;

private:
	virtual HRESULT Initialize_Prototype(tinyxml2::XMLNode* pChild);
	virtual HRESULT Initialize(void* pArg);

public:
	static CNPCStat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, tinyxml2::XMLNode* pChild);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

public:
	NPCINFO Get_Stat();

private:
	NPCINFO m_NpcInfo;
};

NS_END
