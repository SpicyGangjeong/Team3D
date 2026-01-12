#include "pch.h"
#include "MapElement_Balloon.h"

#include "GameInstance.h"

CMapElement_Balloon::CMapElement_Balloon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Balloon::CMapElement_Balloon(const CMapElement_Balloon& rhs)
	: CMapElement(rhs)
	, m_fUsingSurfaceParams(rhs.m_fUsingSurfaceParams)
{
}

HRESULT CMapElement_Balloon::Initialize_Prototype()
{
	m_fUsingSurfaceParams = MRO_PARAMETER;

	return S_OK;
}

HRESULT CMapElement_Balloon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_isRotate = false;
	m_fTimeAcc = 0.f;

#ifdef _DEBUG
	if (nullptr != pArg)
	{
		BALLOON_DESC* pDesc = static_cast<BALLOON_DESC*>(pArg);
		m_isFloating = pDesc->isFloating;
		m_iDiffuseIndex = pDesc->iDiffuseIndex;
		m_vPosition = pDesc->vPosition;
		m_vScale = pDesc->vScale;
		m_vRotation = pDesc->vRotation;
	}
	else
	{
		m_isFloating = false;
		m_iDiffuseIndex = 0;
		m_vPosition = _float3(0.f, 0.f, 0.f);
		m_vScale = _float3(0.f, 0.f, 0.f);
		m_vRotation = _float3(1.f, 1.f, 1.f);
	}

	memcpy(&m_vOriginPosition, &m_vPosition, sizeof(_float3));

	m_bSelected = false;
	
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	m_pTransformCom->Set_Rotation(m_vRotation);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));

#endif // _DEBUG

	return S_OK;
}

void CMapElement_Balloon::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif 
}

void CMapElement_Balloon::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_isFloating)
		Floating();

	if (m_isRotate)
		Rotation();
}

void CMapElement_Balloon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapElement_Balloon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	if (m_bSelected)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::MAPTOOL)))) {
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
			return E_FAIL;
		}
	}

	if (FAILED(m_pModelComs[0]->Render(0))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapElement_Balloon::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	CModel* pModel = { nullptr };

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_FloatingBalloon"),
		reinterpret_cast<CComponent**>(&pModel))))
		return E_FAIL;

	m_pModelComs.push_back(pModel);

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Balloon_Diffuse"),
		reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Balloon_Normal"),
		reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Balloon_MRO"),
		reinterpret_cast<CComponent**>(&m_pMROTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapElement_Balloon::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pDiffuseTextureCom->Get_SRV(m_iDiffuseIndex))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalTexture", m_pNormalTextureCom->Get_SRV(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_SRV("g_SurfaceParamsTexture", m_pMROTextureCom->Get_SRV(0))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float)))) {
		return E_FAIL;
	}

	return S_OK;
}

void CMapElement_Balloon::Rotation()
{

}

void CMapElement_Balloon::Floating()
{
	_float3 vPosition = m_vOriginPosition;

	vPosition.y = m_vOriginPosition.y + sinf(m_fTimeAcc);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
}

CMapElement_Balloon* CMapElement_Balloon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Balloon* pInstance = new CMapElement_Balloon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Balloon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Balloon::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Balloon* pInstance = new CMapElement_Balloon(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Balloon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Balloon::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);

	SAFE_RELEASE(m_pDiffuseTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pMROTextureCom);
}

void CMapElement_Balloon::Describe_Entity()
{
	if (m_bDead)
		return;
	if (nullptr == m_pGameInstance)
		return;

	GUI::Checkbox("isMove", &m_isFloating);
	GUI::Checkbox("isRotate", &m_isRotate);
	GUI::InputInt("School Index", (_int*)(&m_iDiffuseIndex));

	m_iDiffuseIndex = max(m_iDiffuseIndex, 0);
	m_iDiffuseIndex = min((_int)m_iDiffuseIndex, 3);

	if (GUI::Button("PosCopy"))
	{
		XMStoreFloat3(&m_vOriginPosition, m_pTransformCom->Get_State(STATE::POSITION));
	}

	m_iLodIndex = max(0, m_iLodIndex);
	m_iLodIndex = min(m_iMaxLodLevel, m_iLodIndex);

	_float3 vMove = {};
	GUI::InputFloat("Right", &vMove.x, 0.05f, 0.1f);
	GUI::InputFloat("Up", &vMove.y, 0.05f, 0.1f);
	GUI::InputFloat("Look", &vMove.z, 0.05f, 0.1f);

	m_pTransformCom->Move_Right(vMove.x);
	m_pTransformCom->Move_Up(vMove.y);
	m_pTransformCom->Move_Look(vMove.z);

	XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(STATE::POSITION));

	GUI::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));


	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			memcpy(&m_vPosition, &vPosition, sizeof(_float3));
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		}
	}

	GUI::Text("----- Rotation ----");
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 1.f, 15.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 1.f, 15.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 1.f, 15.f);

	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.05f, 0.1f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.05f, 0.1f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.05f, 0.1f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	m_pTransformCom->Set_Scale(m_vScale);

	if (GUI::Button("Delete"))
		m_bDead = true;

	m_bSelected = true;
}

HRESULT CMapElement_Balloon::Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement* object = doc.NewElement("Object");
	object->SetAttribute("Type", 0);
	root->InsertEndChild(object);


#pragma region TRANSFORM
	_float3 vPosition = {};
	XMStoreFloat3(&vPosition, m_pTransformCom->Get_State(STATE::POSITION));
	_float3 vScale = m_pTransformCom->Get_Scale();
	_float3 vRotation = m_pTransformCom->Get_Rotation();

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

	tinyxml2::XMLElement* Value = doc.NewElement("Value");
	Value->SetAttribute("isFloating", m_isFloating);
	Value->SetAttribute("DiffuseIndex", m_iDiffuseIndex);
	object->InsertEndChild(Value);
#pragma endregion

	return S_OK;
}
