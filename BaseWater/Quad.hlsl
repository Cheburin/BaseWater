#include "shader\\inc\\shader_include.hlsl"

#include "shader\\src\\vs\\FullScreenQuad.hlsl"

Texture2D textureMap : register( t2 );

SamplerState linearSampler : register( s0 );

Texture2DMS<float4> reflectionMap : register( t3 );

Texture2DMS<float4> refractionMap : register( t4 );

StructuredBuffer<Light> lights : register(t0);

StructuredBuffer<WaterPlane> waterPlanes : register(t1);

#include "shader\\src\\ps\\Simple.hlsl"