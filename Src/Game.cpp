//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "Utils/PlanetGenerator.hpp"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

bool Galactic::IBody::Wireframe = false;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(1280),
    m_outputHeight(960),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0),
    m_showUI(false),
    m_paused(false)
{
//#ifdef _DEBUG
    AllocConsole();

    FILE *stdOut, *stdErr, *stdIn;

    freopen_s(&stdOut, "conin$", "r", stdin);
    freopen_s(&stdErr, "conout$", "w", stdout);
    freopen_s(&stdIn, "conout$", "w", stderr);
//#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    Galactic::InitEngine();

    CreateDevice();
    CreateResources();

    m_keyboard = std::make_unique<Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    //m_camera->Attach(m_system->FindBody("Moon"));

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

// Executes the basic game loop.
void Game::Tick()
{
    m_tracker.Update(m_keyboard->GetState());

    if (m_tracker.IsKeyReleased(Keyboard::P)) { m_paused = !m_paused; }

    if (!m_paused)
    {
        m_timer.Tick([&]()
        {
            Update(m_timer);
        });

        Render();
    }
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float dt = float(timer.GetElapsedSeconds());

    std::ostringstream caption;
    caption << "Galactic Engine [FPS: ";
    caption << timer.GetFramesPerSecond();
    caption << "]";

    SetWindowTextA(m_window, caption.str().c_str());

    auto kb = m_keyboard->GetState();
    auto mouse = m_mouse->GetState();

    if (kb.Escape)
        PostQuitMessage(0);

    /*if (m_tracker.IsKeyReleased(Keyboard::R))
    {
        Galactic::PlanetGenerator gen(m_d3dContext.Get());
        auto planet = gen.CreateRocky("Planet", 962e24, 6371.0);

        planet->SetInfluence(m_system->FindBody("Star"));
        planet->SetVelocity(Vector3(0.0f, 0.0f, 1000.0f));
        planet->Generate(Galactic::EDetail::High);

        m_system->RemoveBody("Planet");
        m_system->AddBody(std::move(planet));
    }*/

    auto planet = static_cast<Galactic::IPlanet*>(m_system->FindBody("Moon"));

    if (m_tracker.IsKeyReleased(Keyboard::F1)) planet->Generate(Galactic::EDetail::Low);
    if (m_tracker.IsKeyReleased(Keyboard::F2)) planet->Generate(Galactic::EDetail::Medium);
    if (m_tracker.IsKeyReleased(Keyboard::F3)) planet->Generate(Galactic::EDetail::High);
    if (m_tracker.IsKeyReleased(Keyboard::D1)) planet->EnableClouds(!planet->IsCloudsEnabled());
    if (m_tracker.IsKeyReleased(Keyboard::D2)) planet->EnableAtmosphere(!planet->IsAtmosphereEnabled());
    
    /*float detailHeightMod = planet->GetParam(Galactic::EParams::DetailHeightMod);
    float detailFrequency = planet->GetParam(Galactic::EParams::DetailFrequency);

    if (m_tracker.IsKeyReleased(Keyboard::NumPad7)) { planet->SetParam(Galactic::EParams::DetailHeightMod, detailHeightMod + 0.02f); planet->Generate(Galactic::EDetail::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad8)) { planet->SetParam(Galactic::EParams::DetailHeightMod, detailHeightMod - 0.02f); planet->Generate(Galactic::EDetail::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad4)) { planet->SetParam(Galactic::EParams::DetailFrequency, detailFrequency + 0.02f); planet->Generate(Galactic::EDetail::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad5)) { planet->SetParam(Galactic::EParams::DetailFrequency, detailFrequency - 0.02f); planet->Generate(Galactic::EDetail::High); }
    */

    m_camera->Events(m_mouse.get(), mouse, dt);

    Vector3 move = Vector3::Zero;

    if (kb.W) move.z += 1.f;
    if (kb.S) move.z -= 1.f;
    if (kb.A) move.x += 1.f;
    if (kb.D) move.x -= 1.f;
    if (kb.E) move.y += 1.f;
    if (kb.X) move.y -= 1.f;

    move = Vector3::Transform(move, m_camera->GetQuaternion());
    move *= 0.05f;

    auto closestBody = static_cast<Galactic::IPlanet*>(m_system->GetClosestBody(m_camera->GetPosition()));
    auto dir = m_camera->GetPosition() - closestBody->GetPosition();

    float factor = ((Vector3::Distance(m_camera->GetPosition(), closestBody->GetPoint(dir)))) * 30.0f;
    factor = std::fminf(std::fmaxf(factor, 0.02f), 100000.0f);

    move = move * factor * dt;
    m_speed = factor;
    m_camera->Move(move);

    m_camera->Update(dt);
    m_system->SetCameraPos(m_camera->GetPosition());
    m_system->Update(dt * 0.01f);

    m_fe->Update(dt * 0.01f);
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    auto view = m_camera->GetViewMatrix();
    auto proj = m_camera->GetProjectionMatrix();

    m_system->Render(view, proj);
    m_fe->Render(view, proj);

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Color(0.0f, 0.0f, 0.0f));
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(0, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr, "Device reset");
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();
    
    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 1280;
    height = 960;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
        ), "Creating device");

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // TODO: Initialize device dependent objects here (independent of window size).
    m_font = std::make_unique<SpriteFont>(m_d3dDevice.Get(), L"Resources/Fonts/Courier.font");
    m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());
    
    m_system = Galactic::CreateStarSystem("System", Galactic::EStarSystem::Simple);

    auto star = Galactic::CreateStar(m_d3dContext.Get(), "Star");
    star->SetRadius(695508.0);
    star->SetTemperature(5777);
    star->SetPosition(Vector3(60000.0f, 0.0f, 0.0f));
    star->SetMass(1.989e30);
    star->Generate();

    /*auto planet = Galactic::CreatePlanet(m_d3dContext.Get(), "Planet", 5.962e24, 6371.0);
    planet->SetPosition(Vector3::Zero);
    planet->SetInfluence(star.get());
    planet->SetVelocity(Vector3(0.0f, 0.0f, 1000.0f));
    planet->Generate(Galactic::EDetail::High);*/
    
    Galactic::PlanetGenerator gen(m_d3dContext.Get());

    auto planet = Galactic::CreatePlanet(m_d3dContext.Get(), "Planet"/*, 5.683e26, 58232.0*/);
    planet->SetInfluence(star.get());
    planet->SetPosition(Vector3(500.0f, 0.0f, -160.0f));
    planet->SetVelocity(Vector3(0.0f, 0.0f, 1e6));
    //planet->SetAtmosphereHeight(800.0f);
    planet->Generate(Galactic::EDetail::High);

    auto moon = gen.CreateRocky("Moon", 5.971e24, 6371.0);
    moon->SetInfluence(planet.get());
    moon->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    moon->SetVelocity(Vector3(-1.0e5, 0.0f, 1.3e6));
    //moon->SetAtmosphereHeight(200.0f);
    moon->ReadSettings("settings.txt");
    moon->Generate(Galactic::EDetail::High);

    m_fe = Galactic::CreateFlatEarth(m_d3dContext.Get(), "Flat Earth", 5e26, 60000.0);
    m_fe->SetInfluence(planet.get());
    m_fe->SetPosition(Vector3(0.0f, 0.0f, 100.0f));
    m_fe->SetVelocity(Vector3(-0.8e5, 0.0f, 0.3e6));
    m_fe->Generate();

    m_system->AddLightSource(dynamic_cast<Galactic::ILightSource*>(star.get()));
    m_system->AddBody(std::move(star));
    m_system->AddBody(std::move(planet));
    m_system->AddBody(std::move(moon));
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr, "Device error");
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 4;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
            ), "Creating swapchain");

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, 4, 0);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
    m_camera = std::make_unique<Galactic::Camera>(backBufferWidth, backBufferHeight);
}

void Game::Cleanup()
{
    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();
    m_font.reset();
    m_spriteBatch.reset();

    m_system->Reset();
}

void Game::OnDeviceLost()
{
    Cleanup();
    
    CreateDevice();
    CreateResources();
}

std::wstring Game::ValueToString(std::wstring name, DirectX::SimpleMath::Vector3 v)
{
    std::wstringstream ss;
    ss.precision(3);
    ss << name << ": ";
    ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";

    return ss.str();
}

std::wstring Game::ValueToString(std::wstring name, float v)
{
    std::wstringstream ss;
    ss.precision(3);
    ss << name << ": " << v;

    return ss.str();
}
