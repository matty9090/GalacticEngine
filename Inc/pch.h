//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0601
#include <SDKDDKVer.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl/client.h>

// DirectX
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// DirectXTK
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

// STD
#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>
#include <atomic>
#include <sstream>
#include <iostream>
#include <queue>

#include <comdef.h>

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr, std::string reason = "")
    {
        if (FAILED(hr))
        {
            /*_com_error err(hr);
            LPCTSTR errMsg = err.ErrorMessage();

            MessageBoxW(NULL, errMsg, L"Error", MB_OK);
            throw std::exception();*/

            /*TCHAR szBuffer[200];

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, hr,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR)szBuffer,
                200,
                NULL);*/

            MessageBoxA(NULL, reason.c_str(), "Error", MB_OK);
            throw std::exception();
        }
    }
}