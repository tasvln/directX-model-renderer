struct VertexInputType {
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv       : TEXCOORD;
};

struct PixelInputType {
    float4 position : SV_POSITION;  // required
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv       : TEXCOORD;
};

// Constant buffer for MVP
cbuffer ModelViewProjectionCB : register(b0)
{
    matrix mvp;
};

PixelInputType vsmain(VertexInputType input)
{
    PixelInputType output;

    // Transform position to clip space
    output.position = mul(input.position, mvp);

    // Pass through data for pixel shader
    output.normal   = input.normal;
    output.tangent  = input.tangent;
    output.uv       = input.uv;

    return output;
}
