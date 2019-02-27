#include "Common.fx"

cbuffer MatrixBuffer : register(b0) {
    matrix mViewProj;
    matrix mWorld;
    float3 mCamera;
    float  mLerp;
}

struct DS_OUTPUT {
    float4 ProjPos      : SV_Position;
    float3 WorldPos     : POSITION;
    float3 WorldNormal  : NORMAL;
    float3 WorldTangent : TANGENT;
    float2 Biome        : COLOR0;
    float3 Colour1      : COLOR1;
    float3 Colour2      : COLOR2;
    float2 UV           : TEXCOORD0;
    uint   TexIndex     : TEXCOORD1;
};

struct HS_PATCH_CONSTANT_OUTPUT {
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess  : SV_InsideTessFactor;
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

[domain("tri")]
DS_OUTPUT main(HS_PATCH_CONSTANT_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> TrianglePatch) {
    DS_OUTPUT dOut;

    dOut.WorldPos = BarycentricCoordinates.x * TrianglePatch[0].WorldPos +
        BarycentricCoordinates.y * TrianglePatch[1].WorldPos +
        BarycentricCoordinates.z * TrianglePatch[2].WorldPos;

    dOut.WorldNormal = BarycentricCoordinates.x * TrianglePatch[0].WorldNormal +
        BarycentricCoordinates.y * TrianglePatch[1].WorldNormal +
        BarycentricCoordinates.z * TrianglePatch[2].WorldNormal;

    dOut.WorldNormal = normalize(dOut.WorldNormal);

    dOut.WorldTangent = BarycentricCoordinates.x * TrianglePatch[0].WorldTangent +
        BarycentricCoordinates.y * TrianglePatch[1].WorldTangent +
        BarycentricCoordinates.z * TrianglePatch[2].WorldTangent;

    dOut.WorldTangent = normalize(dOut.WorldTangent);

    dOut.UV = BarycentricCoordinates.x * TrianglePatch[0].UV +
        BarycentricCoordinates.y * TrianglePatch[1].UV +
        BarycentricCoordinates.z * TrianglePatch[2].UV;

    dOut.Biome = BarycentricCoordinates.x * TrianglePatch[0].Biome +
        BarycentricCoordinates.y * TrianglePatch[1].Biome +
        BarycentricCoordinates.z * TrianglePatch[2].Biome;

    dOut.Colour1  = TrianglePatch[0].Colour1;
    dOut.Colour2  = TrianglePatch[0].Colour2;
    dOut.TexIndex = TrianglePatch[0].TexIndex;

    dOut.ProjPos = mul(float4(dOut.WorldPos, 1.0f), mViewProj);
    dOut.ProjPos.z = LogDepthBuffer(dOut.ProjPos.w);

    return dOut;
}