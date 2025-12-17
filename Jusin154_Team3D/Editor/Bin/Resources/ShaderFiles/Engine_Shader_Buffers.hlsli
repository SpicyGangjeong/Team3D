#ifndef ENGINE_SHADER_BUFFERS_HLSLI
#define ENGINE_SHADER_BUFFERS_HLSLI

cbuffer ConstantsMotionMatrices : register(b0)
{
    row_major matrix PrevViewMatrix;
    row_major matrix PrevProjMatrix;
}

#endif // ENGINE_SHADER_BUFFERS_HLSLI
