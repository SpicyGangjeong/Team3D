#include "pch.h"
#include "InstancedProp.h"
#include "VIBuffer_Model_Instance.h"

CInstancedProp::CInstancedProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CInstancedProp::CInstancedProp(const CInstancedProp& rhs)
    :CGameObject(rhs)
{
}

void CInstancedProp::Priority_Update(_float fTimeDelta)
{
	
}

void CInstancedProp::Update(_float fTimeDelta)
{
	if(m_bEditMode)
	{
		if (m_pGameInstance->Mouse_Down(DIM_MBUTTON))
		{
			_float4x4 WorldMatrix = {};
			XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_XMWorldMatrix());
			m_WorldMatrices.push_back(WorldMatrix);

			m_pVIBufferInstanceCom->Add_Instance(m_pTransformCom->Get_XMWorldMatrix());
			m_pVIBufferInstanceCom->Update_Instance();

			m_vRotation.y = m_pGameInstance->Random_Float(0.f, 360.f);
			m_vScale.y = m_pGameInstance->Random_Float(0.8f, 1.2f);

			_float3 vPosition = {};
			if (m_pGameInstance->isPicking(&vPosition))
			{
				memcpy(&m_vPosition, &vPosition, sizeof(_float3));
				m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));
			}
		}
#ifdef _DEBUG
		Describe_Entity();
#endif 
	}
}

void CInstancedProp::Late_Update(_float fTimeDelta)
{
	if(true == m_isShake)
	{
		m_pVIBufferInstanceCom->Shake(fTimeDelta);
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CInstancedProp::Render()
{
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMesh = m_pVIBufferInstanceCom->Get_NumMesh();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if(FAILED(m_pVIBufferInstanceCom->Bind_Matrial(m_pShaderCom, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pVIBufferInstanceCom->Render(i);
	}

    return S_OK;
}

HRESULT CInstancedProp::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInstancedProp::Initialize(void* pArg)
{
	INSTANCE_PROP_DESC* pDesc = static_cast<INSTANCE_PROP_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	m_bEditMode = pDesc->bEditMode;
	m_isShake = pDesc->isShake;
	m_strPrototypeTag = pDesc->strPrototypeTag;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if(m_isShake)
		m_pVIBufferInstanceCom->Set_Shake_Value(pDesc->vRadius, pDesc->vSpeed);

	m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	m_vRotation = _float3(0.f, 0.f, 0.f);
	m_vScale = _float3(1.f, 1.f, 1.f);


	XMStoreFloat4x4(&m_WorldMatrixIdentity, XMMatrixIdentity());

	if(!m_bEditMode)
		Load_InstancedProp(pDesc->strInstanceDataPath.c_str());
	else
	{
		m_pVIBufferInstanceCom->Add_Instance(m_pTransformCom->Get_XMWorldMatrix());
		m_pVIBufferInstanceCom->Update_Instance();
	}

    return S_OK;
}

HRESULT CInstancedProp::Ready_Components(void* pArg)
{
    if (FAILED(__super::Ready_Components(pArg)))
        return E_FAIL;

	/* Com_VIBuffer_Instance_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pVIBufferInstanceCom))))
		return E_FAIL;



	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_INSTANCE_PROP_MODEL,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

    return S_OK;
}

HRESULT CInstancedProp::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrixIdentity))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}

    return S_OK;
}

void CInstancedProp::Add_WorldMatrix()
{
	_float4x4 WorldMatrix = {};
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	m_WorldMatrices.push_back(WorldMatrix);
}

void CInstancedProp::Load_InstancedProp(const _char* pFilePath)
{
	m_pVIBufferInstanceCom->Delete_Instance();

	ifstream FileIn(pFilePath, ios::binary);

	if (!FileIn.is_open())
	{
		MSG_BOX("Failed to Load InstancedProp File");
		return;
	}

	m_WorldMatrices.clear();

	_uint iNumWorldMatrix = 0;

	FileIn.read(reinterpret_cast<char*>(&iNumWorldMatrix), sizeof(_uint));
	m_WorldMatrices.reserve(iNumWorldMatrix);

	_float4x4 WorldMatrix = {};
	//FileIn.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));

	for (_uint i = 0; i < iNumWorldMatrix; ++i)
	{
		FileIn.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));

		m_WorldMatrices.push_back(WorldMatrix);
	}
	FileIn.close();

	if (FAILED(m_pVIBufferInstanceCom->Load_WorldData(m_WorldMatrices)))
	{
	}

	return;
}

void CInstancedProp::Patch_Data(const _char* pFilePath)
{
	vector<_float4x4>			TempWorldMatrices;

	ifstream FileIn(pFilePath, ios::binary);

	if (!FileIn.is_open())
	{
		MSG_BOX("Failed to Load InstancedProp File");
		return;
	}

	_uint iNumWorldMatrix = 0;
	_uint iNumAdd = 0;
	FileIn.read(reinterpret_cast<char*>(&iNumWorldMatrix), sizeof(_uint));
	
	_float4x4 WorldMatrix = {};
	for (_uint i = 0; i < iNumWorldMatrix; ++i)
	{
		FileIn.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));
		
		if (0 == WorldMatrix._41 && 0 == WorldMatrix._42 &&  0 == WorldMatrix._43)
			continue;
		else
		{
			++iNumAdd;
			TempWorldMatrices.push_back(WorldMatrix);
		}
		
	}
	FileIn.close();


	/* Re */
	ofstream FileOut(pFilePath, ios::binary);

	if (!FileOut.is_open())
	{
		MSG_BOX("Failed to Save InstancedProp File");
		return;
	}
	else
		MSG_BOX("Successed to Save InstancedProp File");


	FileOut.write(reinterpret_cast<const char*>(&iNumAdd), sizeof(_uint));

	for (_uint i = 0; i < iNumAdd; ++i)
	{
		FileOut.write(reinterpret_cast<const char*>(&TempWorldMatrices[i]), sizeof(_float4x4));
	}

	FileOut.close();
}

#ifdef _DEBUG
void CInstancedProp::Save_InstancedProp(const _char* pFilePath)
{
	_float4x4 WorldMatrix = {};
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_XMWorldMatrix());
	m_WorldMatrices.push_back(WorldMatrix);

	ofstream FileOut(pFilePath, ios::binary);

	if (!FileOut.is_open())
	{
		MSG_BOX("Failed to Save InstancedProp File");
		return;
	}
	else
		MSG_BOX("Successed to Save InstancedProp File");

	_uint iNumWorldMatrix = static_cast<_uint>(m_WorldMatrices.size());

	FileOut.write(reinterpret_cast<const char*>(&iNumWorldMatrix), sizeof(_uint));

	for (_uint i = 0; i < iNumWorldMatrix; ++i)
	{
		FileOut.write(reinterpret_cast<const char*>(&m_WorldMatrices[i]), sizeof(_float4x4));
	}

	FileOut.close();

	return;
}
#endif // _DEBUG

CInstancedProp* CInstancedProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstancedProp* pInstance = new CInstancedProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInstancedProp");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CInstancedProp::Clone(void* pArg, CGameObject* pOwner)
{
	CInstancedProp* pInstance = new CInstancedProp(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInstancedProp");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CInstancedProp::Free()
{
	__super::Free();

	m_WorldMatrices.clear();
	
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferInstanceCom);
}
void CInstancedProp::Describe_Entity()
{
	GUI::Begin("Instanced Prop");

#ifdef _DEBUG
	m_pVIBufferInstanceCom->Describe_Entity();
#endif // _DEBUG
	_float3 vMove = {};

	ImGui::Text("----- Transfrom ----");
	ImGui::InputFloat("Right", &vMove.x, 0.05f, 0.1f);
	ImGui::InputFloat("Up", &vMove.y, 0.05f, 0.1f);
	ImGui::InputFloat("Look", &vMove.z, 0.05f, 0.1f);

	m_pTransformCom->Move_Right(vMove.x);
	m_pTransformCom->Move_Up(vMove.y);
	m_pTransformCom->Move_Look(vMove.z);


	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			memcpy(&m_vPosition, &vPosition, sizeof(_float3));
			m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));
		}
	}
	
	GUI::Text("----- Rotation ----");
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 5.f, 45.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 5.f, 45.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 5.f, 45.f);

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	m_pTransformCom->Set_Scale(m_vScale);
	//m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));
	//m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));

	m_pVIBufferInstanceCom->Fix_Instance(m_pTransformCom->Get_XMWorldMatrix());
	if (GUI::Button("Delete", ImVec2(150.f, 30.f)))
	{
		if(0 != m_WorldMatrices.size())
		{
			m_pVIBufferInstanceCom->Delete_Instance();
			m_WorldMatrices.pop_back();
		}
	}

	GUI::InputText("File Name", m_szFileName, sizeof(m_szFileName));

	if (GUI::Button("Save Instanced Prop", ImVec2(150.f, 30.f)))
	{
		Save_InstancedProp(("../Bin/Resources/Data/Map/Instance/" + string(m_szFileName)+ ".bin").c_str());
	}

	if (GUI::Button("Load Instanced Prop", ImVec2(150.f, 30.f)))
	{
		Load_InstancedProp(("../Bin/Resources/Data/Map/Instance/" + string(m_szFileName) + ".bin").c_str());
	}
	GUI::Spacing();
	GUI::Spacing();
	GUI::Spacing();
	if(GUI::Button(" !!! Patch !!!"))
	{
		Patch_Data(("../Bin/Resources/Data/Map/Instance/" + string(m_szFileName) + ".bin").c_str());
	}
	GUI::End();
}

