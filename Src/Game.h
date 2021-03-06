//
// Game.h
//

#pragma once

#include <thread>

#include "StepTimer.h"
#include "GalacticEngine.hpp"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

	Game() noexcept;
	~Game() { Cleanup(); }

    // Initialization and management
    void Initialize(HWND window, int width, int height);

	void Cleanup();

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    std::wstring ValueToString(std::wstring name, DirectX::SimpleMath::Vector3 v);
    std::wstring ValueToString(std::wstring name, float v);

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

    DirectX::Keyboard::KeyboardStateTracker         m_tracker;
    std::unique_ptr<DirectX::Keyboard>              m_keyboard;
    std::unique_ptr<DirectX::Mouse>                 m_mouse;
    std::unique_ptr<DirectX::SpriteFont>            m_font;
    std::unique_ptr<DirectX::SpriteBatch>           m_spriteBatch;
    
    float                                           m_speed;
    bool                                            m_showUI;
    bool                                            m_paused;

    std::unique_ptr<Galactic::Camera>				m_camera;
    std::unique_ptr<Galactic::IStarSystem>          m_system;
    std::unique_ptr<Galactic::FlatEarth>            m_fe;
};