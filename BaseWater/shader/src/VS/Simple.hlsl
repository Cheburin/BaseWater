ExpandPosNormalTangetColorTex2d MODEL( in PosNormalTangetColorTex2d i )
{
  ExpandPosNormalTangetColorTex2d output;  
  
  output.pos = float4( i.pos, 1.0 );
  output.normal = i.normal;
  output.tangent = i.tangent;
  output.color = i.color;
  output.tex = i.tex;

  return output;
}   

ClipPosTex2d SIMPLE_MODEL( in PosNormalTex2d i )
{
  ClipPosTex2d output;  
  
  output.clip_pos = mul( float4( i.pos, 1.0 ), g_mWorldViewProjection );
  output.tex = i.tex;

  return output;
} 

ExpandPosNormalTex2d THROWPUT( in PosNormalTex2d i )
{
  ExpandPosNormalTex2d output;  
  
  output.pos =  mul( float4( i.pos, 1.0 ), g_mWorldView );
  output.normal = mul( float4( i.normal, 0.0 ), g_mWorldView ).xyz;
  output.tex = i.tex;

  return output;
}   

ClipPosPosNormalTex2dClipDist0 LIGHTED_MODEL( in PosNormalTex2d i )
{
  ClipPosPosNormalTex2dClipDist0 output;  

  output.pos = mul( float4( i.pos, 1.0 ), g_mWorld  ).xyz;

  output.normal = mul( float4( i.normal, 0.0 ), g_mWorld ).xyz;

  output.tex = i.tex;

  output.clip_pos = mul( float4( i.pos, 1.0 ), g_mWorldViewProjection );

  output.clip = dot( float4(output.pos, 1), waterPlanes[0].plane );

  return output;
} 
