cbuffer MatrixBuffer : register(b0) {
    matrix mViewProj;
    matrix mWorld;
    float3 mCamera;
    float  mLerp;
}

cbuffer HullShaderBuffer : register(b1) {
    float MinTessDistance;
    float MaxTessDistance;
    float MinTessellation;
    float MaxTessellation;
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
    float  OppEdgeTess  : TEXCOORD2;
};

struct HS_PATCH_CONSTANT_OUTPUT {
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HS_DistanceTess")]
[maxtessfactor(40.0)]
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

    const float distanceRange = MaxTessDistance - MinTessDistance;
    const uint oppID1 = pointID < 2 ? pointID + 1 : 0;
    const uint oppID2 = oppID1 < 2 ? oppID1 + 1 : 0;
    float maxDist = max(distance(mCamera, inputPatch[oppID1].WorldPos), distance(mCamera, inputPatch[oppID2].WorldPos));
    hOut.OppEdgeTess = lerp(MaxTessellation, MinTessellation, sqrt(saturate((maxDist - MinTessDistance) / distanceRange)));

    return hOut;
}

HS_PATCH_CONSTANT_OUTPUT HS_DistanceTess(OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> outputPatch)
{
    HS_PATCH_CONSTANT_OUTPUT hOut;

    hOut.EdgeTess[0] = outputPatch[0].OppEdgeTess;
    hOut.EdgeTess[1] = outputPatch[1].OppEdgeTess;
    hOut.EdgeTess[2] = outputPatch[2].OppEdgeTess;
    hOut.InsideTess = min(min(hOut.EdgeTess[0], hOut.EdgeTess[1]), hOut.EdgeTess[2]);

    return hOut;
}