cbuffer MatrixBuffer : register(b0) {
    matrix mViewProj;
    matrix mWorld;
    float3 mCamera;
    float  mLerp;
}

cbuffer HullShaderBuffer : register(b1) {
    float Tessellation;
}

struct HS_INPUT {
    float3 WorldPos : POSITION;
    float3 WorldNormal : NORMAL0;
    float3 WorldTangent : TANGENT;
    float2 Biome : COLOR0;
    float3 Colour1 : COLOR1;
    float3 Colour2 : COLOR2;
    float2 UV : TEXCOORD0;
    uint   TexIndex : TEXCOORD1;
    float  Tess : TESSFACTOR;
};

struct HS_CONTROL_POINT_OUTPUT {
    float3 WorldPos : POSITION;
    float3 WorldNormal : NORMAL0;
    float3 WorldTangent : TANGENT;
    float2 Biome : COLOR0;
    float3 Colour1 : COLOR1;
    float3 Colour2 : COLOR2;
    float2 UV : TEXCOORD0;
    uint   TexIndex : TEXCOORD1;
    float  Tess : TESSFACTOR;
};

struct HS_PATCH_CONSTANT_OUTPUT {
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HS_Fixed")]
[maxtessfactor(10.0)]
HS_CONTROL_POINT_OUTPUT main(InputPatch<HS_INPUT, 3> inputPatch, uint pointID : SV_OutputControlPointID) {
    HS_CONTROL_POINT_OUTPUT hOut;

    hOut.WorldPos = inputPatch[pointID].WorldPos;
    hOut.WorldNormal = inputPatch[pointID].WorldNormal;
    hOut.WorldTangent = inputPatch[pointID].WorldTangent;
    hOut.Biome = inputPatch[pointID].Biome;
    hOut.Colour1 = inputPatch[pointID].Colour1;
    hOut.Colour2 = inputPatch[pointID].Colour2;
    hOut.UV = inputPatch[pointID].UV;
    hOut.TexIndex = inputPatch[pointID].TexIndex;
    hOut.Tess = inputPatch[pointID].Tess;

    return hOut;
}

HS_PATCH_CONSTANT_OUTPUT HS_Fixed(OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> outputPatch)
{
    HS_PATCH_CONSTANT_OUTPUT hOut;

    hOut.EdgeTess[0] = outputPatch[0].Tess;
    hOut.EdgeTess[1] = outputPatch[1].Tess;
    hOut.EdgeTess[2] = outputPatch[2].Tess;
    //hOut.InsideTess  = hOut.InsideTess = min(min(hOut.EdgeTess[0], hOut.EdgeTess[1]), hOut.EdgeTess[2]);
    hOut.InsideTess  = 2;

    return hOut;
}