struct PixelInputType {
    float4 position : SV_POSITION;  // required
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv       : TEXCOORD;
};

float4 psmain(PixelInputType input) : SV_TARGET
{
    return float4(input.uv, 0.0f, 1.0f); // outputs UVs as color
}
