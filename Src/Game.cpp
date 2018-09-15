//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

bool Galactic::IBody::Wireframe = false;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(1280),
    m_outputHeight(960),
    m_featureLevel(D3D_FEATURE_LEVEL_11_1),
    m_yaw(0),
    m_pitch(0),
    m_cameraPos(0.0f, 0.0f, -50.0f)
{
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateDevice();
    CreateResources();

    m_keyboard = std::make_unique<Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

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
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
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

    m_tracker.Update(kb);

    if (kb.Escape)
        PostQuitMessage(0);

    if (kb.Home) m_cameraPos = Vector3(0.0f, 5.0f, -10.0f);

    if (m_tracker.IsKeyReleased(Keyboard::F1)) m_bodies[0]->Generate(Galactic::EDetail::Low);
    if (m_tracker.IsKeyReleased(Keyboard::F2)) m_bodies[0]->Generate(Galactic::EDetail::Medium);
    if (m_tracker.IsKeyReleased(Keyboard::F3)) m_bodies[0]->Generate(Galactic::EDetail::High);

    if (m_tracker.IsKeyReleased(Keyboard::Q)) Galactic::IBody::Wireframe = !Galactic::IBody::Wireframe;

    if (mouse.positionMode == Mouse::MODE_RELATIVE)
    {
        Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f) * 0.002f;

        m_pitch -= delta.y;
        m_yaw -= delta.x;

        float limit = XM_PI / 2.0f - 0.01f;
        m_pitch = std::max(-limit, m_pitch);
        m_pitch = std::min(+limit, m_pitch);

        if (m_yaw > XM_PI)
        {
            m_yaw -= XM_PI * 2.0f;
        }
        else if (m_yaw < -XM_PI)
        {
            m_yaw += XM_PI * 2.0f;
        }
    }

    m_mouse->SetMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

    Vector3 move = Vector3::Zero;

    if (kb.W) move.z += 1.f;
    if (kb.S) move.z -= 1.f;
    if (kb.A) move.x += 1.f;
    if (kb.D) move.x -= 1.f;

    auto planet = std::dynamic_pointer_cast<Galactic::IPlanet>(m_bodies[0]);

    int octaves = planet->GetOctaves();
    float gain = planet->GetGain();
    float height = planet->GetHeight();
    float lacunarity = planet->GetLacunarity();
    float frequency = planet->GetFrequency();

    if (m_tracker.IsKeyReleased(Keyboard::NumPad7)) { planet->SetParameters(frequency, lacunarity, gain - 0.1f, height, octaves); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad8)) { planet->SetParameters(frequency, lacunarity, gain + 0.1f, height, octaves); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad4)) { planet->SetParameters(frequency - 0.01f, lacunarity, gain, height, octaves); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad5)) { planet->SetParameters(frequency + 0.01f, lacunarity, gain, height, octaves); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad1)) { planet->SetParameters(frequency, lacunarity - 0.1f, gain, height, octaves); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad2)) { planet->SetParameters(frequency, lacunarity + 0.1f, gain, height, octaves); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad9)) { planet->SetParameters(frequency, lacunarity, gain, height, octaves + 1); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::NumPad6)) { planet->SetParameters(frequency, lacunarity, gain, height, octaves - 1); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::D1)) { planet->SetParameters(frequency, lacunarity, gain, height - 0.25f, octaves); planet->Generate(Galactic::High); }
    if (m_tracker.IsKeyReleased(Keyboard::D2)) { planet->SetParameters(frequency, lacunarity, gain, height + 0.25f, octaves); planet->Generate(Galactic::High); }

    Quaternion q = Quaternion::CreateFromYawPitchRoll(m_yaw, -m_pitch, 0.f);

    move = Vector3::Transform(move, q);
    move *= 0.05f;

    // TODO: Find actual closest body
    std::shared_ptr<Galactic::IPlanet> closestBody = std::dynamic_pointer_cast<Galactic::IPlanet>(m_bodies[0]);
    
    float radius = (float)(closestBody->GetRadius() / Galactic::Constants::Scale) - 0.005f;
    float factor = ((Vector3::Distance(m_cameraPos, closestBody->GetPosition()) - radius)) * 30.0f;

    factor = std::fmaxf(factor, 5.0f);

    move = move * factor * dt;
    m_speed = factor;
    m_cameraPos += move;

    // TODO: Add your game logic here.
    for (auto &body : m_bodies)
    {
        body->SetCameraPos(m_cameraPos);
        body->Update(dt);
    }
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

    float y = sinf(m_pitch);
    float r = cosf(m_pitch);
    float z = r * cosf(m_yaw);
    float x = r * sinf(m_yaw);

    XMVECTOR lookAt = m_cameraPos + Vector3(x, y, z);
    XMMATRIX view = XMMatrixLookAtRH(m_cameraPos, lookAt, Vector3::Up);

    auto planet = std::dynamic_pointer_cast<Galactic::IPlanet>(m_bodies[0]);

    float height = (m_cameraPos - m_bodies[0]->GetPosition()).Length() - (float)(planet->GetRadius() / Galactic::Constants::Scale);

    // TODO: Add your rendering code here.
    for(auto &body : m_bodies)
        body->Render(view, m_proj);

    m_spriteBatch->Begin();

    m_font->DrawString(m_spriteBatch.get(), ValueToString(L"Camera", m_cameraPos).c_str(), Vector2(10, 10), Colors::White, 0.f, Vector2(0, 0));
    m_font->DrawString(m_spriteBatch.get(), ValueToString(L"Height", height).c_str(), Vector2(10, 40), Colors::White, 0.f, Vector2(0, 0));
    m_font->DrawString(m_spriteBatch.get(), ValueToString(L"Speed", m_speed).c_str(), Vector2(10, 70), Colors::White, 0.f, Vector2(0, 0));
    m_font->DrawString(m_spriteBatch.get(), ValueToString(L"Gain", planet->GetGain()).c_str(), Vector2(10, 120), Colors::White, 0.f, Vector2(0, 0));
    m_font->DrawString(m_spriteBatch.get(), ValueToString(L"Frequency", planet->GetFrequency()).c_str(), Vector2(10, 150), Colors::White, 0.f, Vector2(0, 0));
    m_font->DrawString(m_spriteBatch.get(), ValueToString(L"Lacunarity", planet->GetLacunarity()).c_str(), Vector2(10, 180), Colors::White, 0.f, Vector2(0, 0));
    m_font->DrawString(m_spriteBatch.get(), ValueToString(L"Octaves", (float)planet->GetOctaves()).c_str(), Vector2(10, 210), Colors::White, 0.f, Vector2(0, 0));
    m_font->DrawString(m_spriteBatch.get(), ValueToString(L"Height", planet->GetHeight()).c_str(), Vector2(10, 240), Colors::White, 0.f, Vector2(0, 0));

    m_spriteBatch->End();

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Color(0.012f, 0.012f, 0.012f));
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
        DX::ThrowIfFailed(hr);
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
        ));

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
    
    auto planet = Galactic::CreatePlanet(m_d3dContext, "Planet", 5.962e24, 6371.0);
    planet->SetPosition(Vector3::Zero);
    planet->Generate(Galactic::EDetail::High);

    m_bodies.push_back(planet);
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
            DX::ThrowIfFailed(hr);
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
            ));

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
    m_view = Matrix::CreateLookAt(Vector3(0.0f, 5.0f, -10.f), Vector3::Zero, Vector3::UnitY);
    m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f, float(backBufferWidth) / float(backBufferHeight), 0.0001f, 1000.f);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();
    m_font.reset();
    m_spriteBatch.reset();

    for (auto &body : m_bodies)
        body->Reset();
    
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
