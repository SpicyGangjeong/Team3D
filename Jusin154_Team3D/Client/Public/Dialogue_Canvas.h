#pragma once

#include "Client_Define.h"
#include "CanvasObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CDialogue_Canvas final : public CCanvasObject
{
private:
	CDialogue_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogue_Canvas(const CDialogue_Canvas& rhs);
	virtual ~CDialogue_Canvas() = default;

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
	CGameObject* m_pDialogue_Panel = { nullptr };

public:
	static CDialogue_Canvas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
