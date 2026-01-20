#pragma once

#include "Editor_Define.h"
#include "Unit.h"

NS_BEGIN(Editor)

class CDummyObject final : public CUnit
{
	enum MESH_ORDER {
		HEAD_EYE_OCC,
		HEAD_FACE,
		HEAD_TEETH,
		HEAD_EYELASH,
		HEAD_EYES,
		BODY_ARMS,
		HAIR_MAIN,
		HAIR_MAIN_CLOTH,
		LOWER,
		SHOES,
		UPPER,
		HAIR_SUB,
		HAIR_SUB_CLOTH,
		ROBE_SKIRT,
		ROBE_FUR,
		ROBE_CLOTH,
		END
	};
private:
	CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummyObject(const CDummyObject& Prototype);
	virtual ~CDummyObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_ShaderParameters(_uint iMeshOrder);

public:
	static CDummyObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;


};

NS_END
