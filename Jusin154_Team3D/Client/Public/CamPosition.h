#pragma once

#include "Client_Define.h"

#include "PartObject.h"

NS_BEGIN(Client)

class CCamPosition abstract : public CPartObject
{
public:
	typedef struct tagCamPosition : public CPartObject::PARTOBJECT_DESC
	{

	}CAMERAPOSITION_DESC;
protected:
	CCamPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamPosition(const CCamPosition& rhs);
	virtual ~CCamPosition() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources() override;


public:
	CGameObject* Clone(void* pArg, CGameObject* pOwner)PURE;

	virtual void Free() override;

	void Describe_Entity() override;

};

NS_END
