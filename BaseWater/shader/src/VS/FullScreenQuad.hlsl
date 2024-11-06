ClipPosTex2dTex4d BILLBOARD(uint VertexID : SV_VERTEXID)
{
    ClipPosTex2dTex4d output;

	float2 tmp = float2( (VertexID >> 1) & 1, VertexID & 1 );

	float2 quad = float2( tmp * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f));
    
    float2 tex = float2(quad.y * 0.5 + 0.5f, quad.x * -0.5 + 0.5f);

    float3 pos = float3(quad.y, 0.0, quad.x);

    output.clip_pos = mul( float4( pos, 1.0 ), g_mWorldViewProjection );

    output.tex4d = mul( float4( pos, 1.0 ), g_mObject1WorldViewProjection );

    output.tex2d = tex;

    return output;
} 
