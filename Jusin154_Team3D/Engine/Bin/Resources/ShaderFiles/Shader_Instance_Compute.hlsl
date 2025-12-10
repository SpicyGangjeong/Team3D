#include "Engine_Shader_Functions.hlsli"
#include "Engine_Shader_Defines.hlsli"

struct Instance
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
};

struct InstanceValue
{
    float fSpeed;
    float fMaxRadian;
};


StructuredBuffer<Instance> g_InstanceBufferInput : register(t0);
StructuredBuffer<InstanceValue> g_InstanceValueBufferInput : register(t1);

RWStructuredBuffer<Instance> g_InstanceOutput : register(u0);

cbuffer g_ConstantBuffer : register(b0) // b0 << 이 숫자와 컨스턴트 쉐이더 바인딩할때 인덱스가 동일해야함
{
    float fTimeDelta;
    float fPading1;
    float fPading2;
    float fPading3;
}

//내가 몇개의 스레드를 사용할 것인지 지정하는데
//GPU 칩의 개수를 X * Y * Z의 개수 만큼 사용하겠다는 의미이다.
[numthreads(256, 1, 1)]
void CS_MAIN(
  uint3 DispatchThreadID : SV_DispatchThreadID)
{
    int iIndex = DispatchThreadID.x; // 배열 인덱스 
    
    Instance particle = g_InstanceBufferInput[iIndex];
    InstanceValue particleValue = g_InstanceValueBufferInput[iIndex];
    
    float fRadian = sin(particleValue.fSpeed * fTimeDelta) * particleValue.fMaxRadian;
    
    // 3. 임의의 축 회전 행렬 생성
    
    // 4. 최종 월드 행렬 (CurMat) 생성
    
    float4x4 CurMatrix =
    {
        particle.vRight,
        particle.vUp,
        particle.vLook,
        particle.vTranslation
    };
    
    CurMatrix = mul(RotateAxis(particle.vLook, fRadian), CurMatrix);

    particle.vRight = CurMatrix[0].xyzw;
    particle.vUp = CurMatrix[1].xyzw;
    particle.vLook = CurMatrix[2].xyzw;
    particle.vTranslation = CurMatrix[3].xyzw;
    
    //아웃풋 대입
    g_InstanceOutput[iIndex] = particle;
}
