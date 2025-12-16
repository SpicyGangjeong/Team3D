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
	void Update_Panel(_int Index);


	class CQuest_Data* Get_Data();
private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pQuest_Panel = { nullptr };
	CGameObject* m_pInProgress_Panel = { nullptr };
	CGameObject* m_pCompleted_Panel = { nullptr };
	CGameObject* m_pQuest_Status_Panel = { nullptr };
	CGameObject* m_pQuest_Data = { nullptr };

	_bool	m_bClick[3] = { false };
	_int m_iIndex{};
public:
	static CQuest_Canvas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
