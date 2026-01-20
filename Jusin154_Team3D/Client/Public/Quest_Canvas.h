#pragma once

#include "Client_Define.h"
#include "CanvasObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

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
	virtual void Visible(_bool bVisible) override;
		
private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CGameObject* m_pQuest_Panel = { nullptr };
	_bool	m_bClick[3] = { false };
	_int m_iIndex{};
public:
	static CQuest_Canvas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
