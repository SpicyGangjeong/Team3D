#pragma once

#include "Editor_Define.h"
#include "CanvasObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CQuest_Canvas final : public CCanvasObject
{
private:
	CQuest_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Canvas(const CQuest_Canvas& rhs);
	virtual ~CQuest_Canvas() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT	Ready_Panel(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Clear_Penel();

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CGameObject* m_pQuest_Panel = { nullptr };
public:
	static CQuest_Canvas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
