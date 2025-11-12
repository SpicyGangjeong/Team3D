
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
    
    float2 vMaskingUVMoveTime;
    float2 vDiffuseUVMoveTime;
    float2 vAniTime;
    
    float2 vAniIndex;
};

struct ParticleOut
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    
    float2 vMaskingUVMoveTime;
    float2 vDiffuseUVMoveTime;
    float2 vAniTime;
    
    float2 vAniIndex;
   
};


StructuredBuffer<Particle> g_ParticleBufferInput : register(t0);
StructuredBuffer<ParticleValue> g_ParticleValueBufferInput : register(t1);

RWStructuredBuffer<ParticleOut> g_ParticleBufferOutput : register(u0);

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
    
    ParticleOut ParticleOut;
    
    // 라이프타임 움직임
    particle.vTranslation.y -= particleValue.fSpeed * fTimeDelta;
    
    particle.vLifeTime.x += fTimeDelta;
    
    if (particle.vLifeTime.x >= particle.vLifeTime.y)
    {
        particle.vLifeTime.x = 0.f;
        particle.vTranslation.y = 0.f;
    }
    
        
    //애니메이션 속도 , 인덱스
    
    particleValue.vAniTime.x += fTimeDelta;
    
    if (particleValue.vAniTime.x >= particleValue.vAniTime.y)
    {
        particleValue.vAniTime.x = 0.f;
        
        particleValue.vAniIndex.x++;
        
        if (particleValue.vAniIndex.x > particleValue.vAniIndex.y) // 애니메이션에 끝에 다다랐다면
        {
            particleValue.vAniIndex.x = 0.f; // 인덱스 초기화

        }

    }
    
    //마스킹 UV 움직임 시간 

    particleValue.vMaskingUVMoveTime.x += fTimeDelta;
    
    if (particleValue.vMaskingUVMoveTime.x >= particleValue.vMaskingUVMoveTime.x)
    {
        particleValue.vMaskingUVMoveTime.x = 0.f;
    }

    
    //디퓨즈 UV 움직임 
    
    particleValue.vDiffuseUVMoveTime.x += fTimeDelta;
    
    if (particleValue.vDiffuseUVMoveTime.x >= particleValue.vDiffuseUVMoveTime.x)
    {
        particleValue.vDiffuseUVMoveTime.x = 0.f;
    }
        
    
    //아웃풋 대입
    
    ParticleOut.vRight = particle.vRight;
    ParticleOut.vUp = particle.vUp;
    ParticleOut.vLook = particle.vLook;
    ParticleOut.vTranslation = particle.vTranslation;
    ParticleOut.vLifeTime = particle.vLifeTime;
    
    ParticleOut.vMaskingUVMoveTime = particleValue.vMaskingUVMoveTime;
    ParticleOut.vDiffuseUVMoveTime = particleValue.vDiffuseUVMoveTime;
    ParticleOut.vAniTime = particleValue.vAniTime;
    ParticleOut.vAniIndex = particleValue.vAniIndex;
    
    
    g_ParticleBufferOutput[iIndex] = ParticleOut;
}
