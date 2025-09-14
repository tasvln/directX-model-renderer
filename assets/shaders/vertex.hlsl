struct VertexInputType {
    float4 position : POSITION;
    float3 normal   : NORMAL0;
    float3 tangent  : TANGENT;
    float2 uv       : TEXCOORD;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 worldPos   : WORLDPOS;
    float3 worldNormal: NORMAL0;
    float3 tangent  : TANGENT;
    float2 uv       : TEXCOORD;
};

// Constant buffer for MVP
cbuffer ModelViewProjectionCB : register(b0)
{
    matrix model;
    matrix viewProj;
};

float3x3 inverse3x3(float3x3 m)
{
    float3 a = m[0];
    float3 b = m[1];
    float3 c = m[2];

    float3 r0 = cross(b, c);
    float3 r1 = cross(c, a);
    float3 r2 = cross(a, b);

    float invDet = 1.0 / dot(r2, c);
    return float3x3(r0 * invDet, r1 * invDet, r2 * invDet);
}


PixelInputType vsmain(VertexInputType input)
{
    PixelInputType output;

    // Transform position to clip space
    float4 worldPosition = mul(input.position, model);
    output.position = mul(worldPosition, viewProj); // clip space
    output.worldPos = worldPosition.xyz;

    // output.worldNormal = normalize(
    //     mul(float4(input.normal, 0.0f), model).xyz
    // );

    float3x3 normalMatrix = transpose(inverse3x3((float3x3)model));
    output.worldNormal = normalize(mul(input.normal, normalMatrix));

    // Pass through data for pixel shader
    output.tangent = input.tangent;
    output.uv = input.uv;

    return output;
}
