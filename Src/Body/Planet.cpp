#include "pch.h"
#include "Body/Planet.hpp"
#include "Render/PlanetRendererFactory.hpp"
#include "Physics/Constants.hpp"
#include "StepTimer.h"

#include <fstream>

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Planet::Planet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name)
    : m_name(name),
      m_deviceContext(deviceContext),
      m_renderer(nullptr),
      m_atmosphere(nullptr),
      m_isGenerated(false),
      m_vertexCount(0),
      m_cloudsEnabled(true)
{
    m_settings.GridSize = 33;
    m_settings.Radius = 6700.0;
    m_settings.AtmColour = { 0.0f, 0.7f, 1.0f };
    m_settings.AtmHeight = 200.0;
    m_settings.Biomes.AddBiomeRow(BiomeConfig::Row().AddBiome(1.0f, "Ocean"), 1.0f);
    m_settings.Mass = 5e24;
    m_settings.MinValue = 0.0f;
    m_settings.NoiseMaps.push_back(PlanetSettings::Map { 0.05f, 0.02f, 0.02, 2.0f, 1.0f });
    m_settings.Seed = rand() % RAND_MAX;
}

Planet::~Planet()
{
    
}

void Planet::Generate(EDetail detail)
{
    double PCFreq = 0.0;
    __int64 CounterStart = 0;

    LARGE_INTEGER li;
    
    if (!QueryPerformanceFrequency(&li))
        std::cout << "QueryPerformanceFrequency failed!\n";

    PCFreq = double(li.QuadPart) / 1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
    
    m_vertexCount = 0;

    Matrix matrix;

    if (m_isGenerated)
    {
        matrix = m_renderer->GetMatrix();

        m_renderer->Reset();
        m_renderer.reset();
        
        m_atmosphere->Reset();
        m_atmosphere.reset();

        m_clouds->Reset();
        m_clouds.reset();
    }

    m_renderer = CreatePlanetRenderer(m_deviceContext, this, detail);
    m_atmosphere = CreateAtmosphereRenderer(m_deviceContext, this, detail);
    m_clouds = std::make_unique<NoiseCloudRenderer>(m_deviceContext.Get(), this);

    if (m_isGenerated)
        m_renderer->GetMatrix() = matrix;

    m_isGenerated = true;

    QueryPerformanceCounter(&li);

    std::cout << "Generated planet in " << (double(li.QuadPart - CounterStart) / PCFreq) << "ms\n";
}

void Planet::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if (m_renderer)
        m_renderer->Render(view, proj);

    if (m_atmosphere)
        m_atmosphere->Render(view, proj);

    if (m_cloudsEnabled && m_clouds)
        m_clouds->Render(view, proj);
}

void Planet::Update(float dt)
{
    //m_position += m_velocity * dt;

    Matrix rotX = Matrix::CreateRotationX(m_rotation.x);
    Matrix rotY = Matrix::CreateRotationY(m_rotation.y);
    Matrix rotZ = Matrix::CreateRotationZ(m_rotation.z);

    Matrix scale = Matrix::CreateScale((float)(m_settings.Radius / Constants::Scale));
    Matrix translation = Matrix::CreateTranslation(m_position);

    m_world = scale * rotX * rotY * rotZ * translation;

    if (m_renderer) m_renderer->Update(dt);
    if (m_atmosphere) m_atmosphere->Update(dt);
    if (m_cloudsEnabled && m_clouds) m_clouds->Update(dt);
}

void Planet::Reset()
{
    if (m_renderer) m_renderer->Reset();
    if (m_atmosphere) m_atmosphere->Reset();
    if (m_clouds) m_clouds->Reset();

    m_renderer.reset();
    m_atmosphere.reset();
    m_clouds.reset();
}

void Planet::ReadSettings(std::string file)
{
    /*std::ifstream f(file.c_str());

    while (!f.eof())
    {
        int param;
        float value;

        f >> param >> value;

        SetParam((EParams)param, value);
    }

    f.close();*/
}

Vector3 Galactic::Planet::GetPoint(Vector3 normal) {
    return m_renderer->GetPoint(normal);
}
