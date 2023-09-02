// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <imgui_freetype.h>
#include <string>

#include <imgui_internal.h>
#include "blur/blur.hpp"
#include "menu.hpp"

ImFont* titlefont;
ImFont* menufont;
ImFont* iconfont;

#define ICON_FA_CROSSHAIRS ("\xef\x81\x9b")
#define ICON_FA_EYE ("\xef\x81\xae")
#define ICON_FA_USER ("\xef\x80\x87")     
#define ICON_FA_GEAR ("\xef\x80\x93")
#define ICON_FA_FOLDER ("\xef\x81\xbb")
#define ICON_FA_CODE ("\xef\x84\xa1")
#define ICON_FA_FONT ("\xef\x80\xb1")
#define ICON_FA_TRASH ("\xef\x87\xb8")
#define ICON_FA_INBOX ("\xef\x80\x9c")
#define ICON_FA_MAP_MARKED ("\xef\x96\x9f")
#define ICON_FA_MONKEY ("\xef\x9b\xbb")
#define ICON_FA_MOON_STARS ("\xef\x9d\x95")
#define ICON_FA_PAINT_BRUSH ("\xef\x87\xbc")
#define ICON_FA_PALLET_ALT ("\xef\x92\x83")

float animation_time = 5.0f;

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1366, 720, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::DefaultStyleColors();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);   

    ImFontConfig titlefontconfig;
    titlefontconfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting;
    titlefontconfig.PixelSnapH = true;
    titlefontconfig.SizePixels = 13.0f;
    titlefontconfig.RasterizerMultiply = 1.0f;

    ImFontConfig menufontconfig;
    menufontconfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting;
    menufontconfig.PixelSnapH = true;
    menufontconfig.SizePixels = 13.0f;
    menufontconfig.RasterizerMultiply = 1.0f;

    char windows_directory[MAX_PATH];
    GetWindowsDirectoryA(windows_directory, MAX_PATH);

    std::string tahoma_bold_font_directory = (std::string)windows_directory + ("\\Fonts\\tahomabd.ttf");
    std::string tahoma_font_directory = (std::string)windows_directory + ("\\Fonts\\tahoma.ttf");
    std::string icons_font_directory = (std::string)windows_directory + ("\\Fonts\\fontawesome-webfont.ttf");
    static const ImWchar ranges[] = { 0xf000, 0xf3ff, 0 };

    titlefont = io.Fonts->AddFontFromFileTTF(tahoma_bold_font_directory.c_str(), 13.0f, &titlefontconfig, io.Fonts->GetGlyphRangesCyrillic());
    menufont = io.Fonts->AddFontFromFileTTF(tahoma_font_directory.c_str(), 13.0f, &menufontconfig, io.Fonts->GetGlyphRangesCyrillic());
    iconfont = io.Fonts->AddFontFromFileTTF(icons_font_directory.c_str(), 15.0f, &menufontconfig, ranges);

    ImGuiFreeType::BuildFontAtlas(io.Fonts);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int tabs = 0;
    bool checkbox = false;

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }


        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
 
            ImGui::SetNextWindowSize(ImVec2(830, 470));
            ImGui::Begin("##menu_for_yougame_free", nullptr, ImGuiWindowFlags_MainMenu);
            {
                const ImVec2 position = ImGui::GetWindowPos();
                const float width = ImGui::GetWindowWidth();
                const float height = ImGui::GetWindowHeight();

                /* backend filled rect's */

                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(position.x, position.y), ImVec2(position.x + width, position.y + 30), ImColor(24, 24, 24, 255), 0.0f);
                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(position.x, position.y + 50), ImVec2(position.x + width, position.y + height), ImColor(24, 24, 24, 255), 0.0f);

                /* backend rect's */

                ImGui::GetBackgroundDrawList()->AddRect(ImVec2(position.x + 5, position.y + 55), ImVec2((position.x + width) - 4, (position.y + height) - 4), ImColor(0, 0, 0, 255), 0.0f, 0, 0.5f);
                ImGui::GetBackgroundDrawList()->AddRect(ImVec2(position.x + 5, position.y + 55), ImVec2((position.x + width) - 5, (position.y + height) - 5), ImColor(255, 255, 255, 255), 0.0f, 0, 0.5f);

                /* fade line */

                ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(ImVec2(position.x, position.y), ImVec2(position.x + (width / 2), position.y + 5), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 0));
                ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(ImVec2(position.x + (width / 2), position.y), ImVec2(position.x + width, position.y + 5), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 255));

                /* title text */

                ImGui::GetBackgroundDrawList()->AddText(titlefont, 13.f, ImVec2(position.x + 6, position.y + 11), ImColor(0, 0, 0, 255), "menu created by bluebird");
                ImGui::GetBackgroundDrawList()->AddText(titlefont, 13.f, ImVec2(position.x + 5, position.y + 10), ImColor(255, 255, 255, 255), "menu created by bluebird");

                /* tabs */

                ImGui::SetCursorPos(ImVec2(width - 305, 8));
                if (elements::tab("rage", tabs == 0))
                {
                    tabs = 0;
                }

                ImGui::SameLine();
                if (elements::tab("legit", tabs == 1))
                {
                    tabs = 1;
                }

                ImGui::SameLine();
                if (elements::tab("visuals", tabs == 2))
                {
                    tabs = 2;
                }

                ImGui::SameLine();
                if (elements::tab("misc", tabs == 3))
                {
                    tabs = 3;
                }

                ImGui::SameLine();
                if (elements::tab("skins", tabs == 4))
                {
                    tabs = 4;
                }

                ImGui::SameLine();
                if (elements::tab("fonts", tabs == 5))
                {
                    tabs = 5;
                }

                ImGui::SameLine();
                if (elements::tab("lua", tabs == 6))
                {
                    tabs = 6;
                }

                ImGui::SameLine();
                if (elements::tab("mr", tabs == 7))
                {
                    tabs = 7;
                }

                switch (tabs)
                {
                    case 0:
                        ImGui::SetCursorPos(ImVec2(10, 60));
                        elements::checkbox("old style", &checkbox);
                        break;
                    case 1: 
                        
                        break;
                    case 2:
                        
                        break;
                }
            }
            ImGui::End();

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
