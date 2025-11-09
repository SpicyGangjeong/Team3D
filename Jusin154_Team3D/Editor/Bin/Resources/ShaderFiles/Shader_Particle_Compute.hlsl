
struct Particle
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;

    float2 vLifeTime;
};

struct ParticleValue
{
    float  fSpeed;
    float  fRotaionSpeed;

    float4 vOriginRight;
    float4 vOriginUp;
    float4 vOriginLook;
    float4 vOriginTranslation;
    
    float2 fPadding;
};


StructuredBuffer<Particle> g_ParticleBufferInput : register(t0);
StructuredBuffer<ParticleValue> g_ParticleValueBufferInput : register(t1);

RWStructuredBuffer<Particle> g_ParticleBufferOutput : register(u0);

cbuffer g_ConstantBuffer : register(b0) // b0 << 이 숫자와 컨스턴트 쉐이더 바인딩할때 인덱스가 동일해야함
{
    float   fTimeDelta;
  
    float   fPadding; // 반드시 상수버퍼는 16바이트 배수로 만들어져야 한다.
    float   fPadding2;
    float   fPadding3;
}

//내가 몇개의 스레드를 사용할 것인지 지정하는데
//GPU 칩의 개수를 X * Y * Z의 개수 만큼 사용하겠다는 의미이다.
[numthreads(256, 1, 1)]
void CS_MAIN(
  uint3 DispatchThreadID : SV_DispatchThreadID)
{
    int iIndex = DispatchThreadID.x; // 배열 인덱스 
    
    Particle particle = g_ParticleBufferInput[iIndex];
    
    ParticleValue particleValue = g_ParticleValueBufferInput[iIndex];
    
    particle.vTranslation.y -= particleValue.fSpeed * fTimeDelta;
    
    particle.vLifeTime.x += fTimeDelta;
    
    if (particle.vLifeTime.x >= particle.vLifeTime.y)
    {
        particle.vLifeTime.x = 0.f;
        particle.vTranslation.y = 0.f;
    }
    
    g_ParticleBufferOutput[iIndex] = particle;
    
}
