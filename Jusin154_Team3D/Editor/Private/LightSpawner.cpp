#include "pch.h"
#include "LightSpawner.h"

#include "GameInstance.h"
#include "Layer.h"

CLightSpawner::CLightSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLightSpawner::CLightSpawner(const CLightSpawner& rhs)
	: CGameObject(rhs)
{
}

void CLightSpawner::Priority_Update(_float fTimeDelta)
{
}

void CLightSpawner::Update(_float fTimeDelta)
{
	m_fAttenuationTime += fTimeDelta * m_fLightIntensitySpeed;

	
}

void CLightSpawner::Late_Update(_float fTimeDelta)
{
	m_pLightCom->Set_AttPower(sinf(m_fAttenuationTime) * sinf(m_fAttenuationTime) * m_fLightIntensity + m_fLightIntensity + 1.f);
}

HRESULT CLightSpawner::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLightSpawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLightSpawner::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightSpawner::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	LIGHT_DESC			LightDesc{};
	LightDesc.iLevel = NEXT_LEVEL;
	LightDesc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);
	if(nullptr == pArg)
	{
		LightDesc.eType = LIGHT::POINT;

		LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
		LightDesc.vAmbient = _float4(1.f, 0.f, 0.f, 1.f);
		LightDesc.vSpecular = _float4(0.2f, 0.f, 0.f, 1.f);
		LightDesc.vPosOffset = _float4(0.f, 0.f, 0.f, 1.f);
		LightDesc.fRange = 10.f;

		/* Com_Light*/
		if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
		{
			return E_FAIL;
		}
	}
	else
	{
		LIGHTSPAWNER_DESC* pDesc = static_cast<LIGHTSPAWNER_DESC*>(pArg);

		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
		LightDesc.eType = LIGHT::POINT;
		LightDesc.vDiffuse = pDesc->vDiffuse;
		LightDesc.vAmbient = pDesc->vAmbient;
		LightDesc.vSpecular = pDesc->vSpecular;
		LightDesc.vPosOffset = _float4(pDesc->vPosOffset.x, pDesc->vPosOffset.y, pDesc->vPosOffset.z, 1.f);
		LightDesc.fRange = pDesc->fRange;

		m_fLightIntensity = pDesc->fLightAttRange;
		m_fLightIntensitySpeed = pDesc->fLightAttSpeed;

		/* Com_Light*/
		if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
		{
			return E_FAIL;
		}
	}

	m_pGameInstance->Add_Light(NEXT_LEVEL, m_pLightCom);

	return S_OK;
}

HRESULT CLightSpawner::Bind_ShaderResources()
{
	return S_OK;
}


CLightSpawner* CLightSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightSpawner* pInstance = new CLightSpawner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLightSpawner");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLightSpawner::Clone(void* pArg, CGameObject* pOwner)
{
	CLightSpawner* pInstance = new CLightSpawner(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLightSpawner");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CLightSpawner::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLightCom);
}

void CLightSpawner::Describe_Entity()
{
	GUI::DragFloat("Light Intensity", &m_fLightIntensity, 0.1f, 0.f, 20.f);

	m_pTransformCom->Describe_Entity();

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
		}
	}

	m_pLightCom->Describe_Entity();
}

HRESULT CLightSpawner::Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement* object = doc.NewElement("Object");
	root->InsertEndChild(object);

#pragma region TRANSFORM
	_float3 vPosition = {};
	XMStoreFloat3(&vPosition, m_pTransformCom->Get_State(STATE::POSITION));

	_float3 vScale = m_pTransformCom->Get_Scale();

	_float3 vRotation = {};
	XMStoreFloat3(&vRotation, m_pTransformCom->Get_RollPitchYawVector());
	vRotation.x = XMConvertToDegrees(vRotation.x);
	vRotation.y = XMConvertToDegrees(vRotation.y);
	vRotation.z = XMConvertToDegrees(vRotation.z);

	tinyxml2::XMLElement* Position = doc.NewElement("Position");
	Position->SetAttribute("x", vPosition.x);
	Position->SetAttribute("y", vPosition.y);
	Position->SetAttribute("z", vPosition.z);
	object->InsertEndChild(Position);

	tinyxml2::XMLElement* Scale = doc.NewElement("Scale");
	Scale->SetAttribute("x", vScale.x);
	Scale->SetAttribute("y", vScale.y);
	Scale->SetAttribute("z", vScale.z);
	object->InsertEndChild(Scale);

	tinyxml2::XMLElement* Rotation = doc.NewElement("Rotation");
	Rotation->SetAttribute("x", vRotation.x);
	Rotation->SetAttribute("y", vRotation.y);
	Rotation->SetAttribute("z", vRotation.z);
	object->InsertEndChild(Rotation);
#pragma endregion

#pragma region LIGHT_DESC
	const LIGHT_DESC* pLight = m_pLightCom->Get_LightDesc();

	/* Diffuse */
	tinyxml2::XMLElement* Diffuse = doc.NewElement("Diffuse");
	Diffuse->SetAttribute("x", pLight->vDiffuse.x);
	Diffuse->SetAttribute("y", pLight->vDiffuse.y);
	Diffuse->SetAttribute("z", pLight->vDiffuse.z);
	object->InsertEndChild(Diffuse);

	/* Ambient */
	tinyxml2::XMLElement* Ambient = doc.NewElement("Ambient");
	Ambient->SetAttribute("x", pLight->vAmbient.x);
	Ambient->SetAttribute("y", pLight->vAmbient.y);
	Ambient->SetAttribute("z", pLight->vAmbient.z);
	object->InsertEndChild(Ambient);

	/* Specular */
	tinyxml2::XMLElement* Specular = doc.NewElement("Specular");
	Specular->SetAttribute("x", pLight->vSpecular.x);
	Specular->SetAttribute("y", pLight->vSpecular.y);
	Specular->SetAttribute("z", pLight->vSpecular.z);
	object->InsertEndChild(Specular);

	/* PosOffset */
	tinyxml2::XMLElement* PosOffset = doc.NewElement("PosOffset");
	PosOffset->SetAttribute("x", pLight->vPosOffset.x);
	PosOffset->SetAttribute("y", pLight->vPosOffset.y);
	PosOffset->SetAttribute("z", pLight->vPosOffset.z);
	object->InsertEndChild(PosOffset);

	/* Info */
	tinyxml2::XMLElement* Info = doc.NewElement("Info");
	Info->SetAttribute("Range", pLight->fRange);
	Info->SetAttribute("LightAttRange", m_fLightIntensity);
	Info->SetAttribute("LightAttSpeed", m_fLightIntensitySpeed);

	if (false == m_pLightCom->Is_PowerLightAtt())
		Info->SetAttribute("Pow", 0);
	else
		Info->SetAttribute("Pow", 1);

	object->InsertEndChild(Info);
#pragma endregion

	return S_OK;
}
