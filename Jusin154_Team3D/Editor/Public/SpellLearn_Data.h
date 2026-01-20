#pragma once

#include "Editor_Define.h"
#include "GameObject.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CSpellLearn_Data final : public CGameObject
{
private:
	//typedef struct tagSpellLearnInfo
	//{
	//	_wstring pSpellName;
	//	_wstring pImageName;
	//	_float	fSpellSize{};
	//	_float2 fStartPosition{};
	//	vector<_float2> Lines;
	//}SPELLLEARNINFO;
private:
	CSpellLearn_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_Data(const CSpellLearn_Data& rhs);
	virtual ~CSpellLearn_Data() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	const CUIObject::SPELLLEARNINFO& Get_SpellLearn(_int Index) const;

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_int m_iSpellLearn_Count{};
	vector<CUIObject::SPELLLEARNINFO> m_SpellLearnInfos;
public:
	static CSpellLearn_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
