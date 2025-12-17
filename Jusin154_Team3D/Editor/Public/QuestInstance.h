#pragma once

#include "Editor_Define.h"
#include "Quest.h"

NS_BEGIN(Editor)

class CQuestInstance final : public CQuest
{
private:
	CQuestInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuestInstance(const CQuestInstance& rhs);
	virtual ~CQuestInstance() = default;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CQuestInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
