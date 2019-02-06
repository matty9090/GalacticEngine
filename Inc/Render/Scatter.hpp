#pragma once

#include "Body/IPlanet.hpp"
#include "Physics/Constants.hpp"
#include "SimpleMath.h"

namespace Galactic
{
    struct ScatterBuffer
    {
        DirectX::SimpleMath::Vector3 v3CameraPos;
        float fCameraHeight;
        DirectX::SimpleMath::Vector3 v3LightDir;
        float fCameraHeight2;
        DirectX::SimpleMath::Vector3 v3InvWavelength;
        float fOuterRadius;
        float fOuterRadius2;
        float fInnerRadius;
        float fInnerRadius2;
        float fKrESun;
        float fKmESun;
        float fKr4PI;
        float fKm4PI;
        float fScale;
        float fScaleDepth;
        float fScaleOverScaleDepth;
        float g;
        float g2;
    };

    inline ScatterBuffer GetScatterBuffer(IPlanet *planet)
    {
        float radius = (float)(planet->GetRadius() / Constants::Scale);
        float atmheight = (float)(planet->GetAtmosphereHeight() / Constants::Scale);
        float atmradius = (atmheight + radius);
        float camHeight = (planet->GetCameraPos() - planet->GetPosition()).Length();

        float scale = 1 / (atmradius - radius);
        float scaleDepth = 0.25f;

        DirectX::SimpleMath::Vector3 wavelength(0.65f, 0.57f, 0.475f);
        wavelength.x = 1.0f / powf(wavelength.x, 4.0f);
        wavelength.y = 1.0f / powf(wavelength.y, 4.0f);
        wavelength.z = 1.0f / powf(wavelength.z, 4.0f);

        ScatterBuffer buffer = {
            planet->GetCameraPos() - planet->GetPosition(),
            camHeight,
            DirectX::SimpleMath::Vector3::Right,
            camHeight * camHeight,
            wavelength,
            atmradius,
            atmradius * atmradius,
            radius,
            radius * radius,
            Constants::Kr * Constants::ESun,
            Constants::Km * Constants::ESun,
            Constants::Kr * 4.0f * DirectX::XM_PI,
            Constants::Km * 4.0f * DirectX::XM_PI,
            scale,
            scaleDepth,
            scale / scaleDepth,
            Constants::Af,
            Constants::Af * Constants::Af
        };

        return buffer;
    }
}