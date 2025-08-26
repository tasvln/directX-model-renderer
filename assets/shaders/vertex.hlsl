struct VertexInputType {
    float4 position: POSITION;
    float4 color: COLOR0;
};

struct PixelInputType {
    float4 position : SV_POSITION; // -> Required by rasterizer
    float4 color    : COLOR0;
};

// Constant buffer for MVP
cbuffer ModelViewProjectionCB : register(b0)
{
    matrix mvp;
}

PixelInputType vsmain(VertexInputType input) {
    PixelInputType output;

    output.position = mul(input.position, mvp);

    output.color = input.color;

    return output;
}