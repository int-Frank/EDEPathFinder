
#include <vector>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_dx11.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <d3d11.h>

#include <sstream>
#include <fstream>

#include "GameData.h"
#include "GUI.h"
#include "Path.h"

#include <windows.h>
#include <winuser.h>

#pragma comment(lib, "d3d11.lib")

#define DATA_FILE_PATH L"./EDEPaths.json"

SDL_Window * g_pWindow = nullptr;

static ID3D11Device * g_pd3dDevice = NULL;
static ID3D11DeviceContext * g_pd3dDeviceContext = NULL;
static IDXGISwapChain * g_pSwapChain = NULL;
static ID3D11RenderTargetView * g_mainRenderTargetView = NULL;

uint32_t g_iconPixels[64*64] =
{
#include "icon.inl"
};

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

void Alert(char const * format, ...)
{
  char buffer[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, 255, format, args);
  va_end(args);

  MessageBoxA(NULL, buffer, "Message", MB_OK | MB_ICONERROR);
}

struct GUIData
{
  int systemsCount;
  char ** ppSystems;

  int engineerClassCount;
  char ** ppEngineerClasses;

  int moduleClassCount;
  char ** ppModuleClasses;
};

bool InitGuiData(GUIData & data)
{
  size_t i = 0;

  data.ppSystems = new char * [g_GameData.systems.size()];
  data.systemsCount = (int)g_GameData.systems.size();

  i = 0;
  for (auto const & kv : g_GameData.systems)
  {
    data.ppSystems[i] = new char[kv.first.size() + 1]{};
    sprintf(data.ppSystems[i], "%s", kv.first.c_str());
    i++;
  }

  data.ppEngineerClasses = new char * [g_GameData.engineerClasses.size()];
  data.engineerClassCount = (int)g_GameData.engineerClasses.size();

  i = 0;
  for (auto const & str : g_GameData.engineerClasses)
  {
    data.ppEngineerClasses[i] = new char[str.size() + 1]{};
    sprintf(data.ppEngineerClasses[i], "%s", str.c_str());
    i++;
  }

  std::set<std::string> moduleClasses;
  for (auto const & kv : g_GameData.modules)
    moduleClasses.insert(kv.second.Class);

  data.ppModuleClasses = new char * [moduleClasses.size()];
  data.moduleClassCount = (int)moduleClasses.size();

  i = 0;
  for (auto const & str : moduleClasses)
  {
    data.ppModuleClasses[i] = new char[str.size() + 1]{};
    sprintf(data.ppModuleClasses[i], "%s", str.c_str());
    i++;
  }

  return true;
}

void DestroyGUIData(GUIData & data)
{
  for (int i = 0; i < data.systemsCount; i++)
    delete[] data.ppSystems[i];
  delete[] data.ppSystems;
  data.ppSystems = nullptr;
  data.systemsCount = 0;

  for (int i = 0; i < data.engineerClassCount; i++)
    delete[] data.ppEngineerClasses[i];
  delete[] data.ppEngineerClasses;
  data.ppEngineerClasses = nullptr;
  data.engineerClassCount = 0;

  for (int i = 0; i < data.moduleClassCount; i++)
    delete[] data.ppModuleClasses[i];
  delete[] data.ppModuleClasses;
  data.ppModuleClasses = nullptr;
  data.moduleClassCount = 0;
}


bool Init()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
  {
    Alert("ERROR: %s", SDL_GetError());
    return false;
  }

  // Setup window
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_ALLOW_HIGHDPI);
  g_pWindow = SDL_CreateWindow("ED Engineer Path Finder", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1231, 645, window_flags);
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(g_pWindow, &wmInfo);
  HWND hwnd = (HWND)wmInfo.info.win.window;
  
  SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(g_iconPixels, 64, 64, 32, 64*4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  SDL_SetWindowIcon(g_pWindow, surface);
  SDL_FreeSurface(surface);

  // Initialize Direct3D
  if (!CreateDeviceD3D(hwnd))
  {
    Alert("ERROR: Failed to initialise Direct3D");
    CleanupDeviceD3D();
    return false;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO & io = ImGui::GetIO(); (void)io;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplSDL2_InitForD3D(g_pWindow);
  ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

  return g_GameData.Load(DATA_FILE_PATH);
}

void ShutDown()
{
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  SDL_DestroyWindow(g_pWindow);
  SDL_Quit();
}

void DoEngineersWindow(GUIData & guiData)
{
  static int engineerClassIndex = 0;

  ImGui::SetNextWindowPos(ImVec2(330, 5));
  ImGui::SetNextWindowSize(ImVec2(542, 635));
  ImGui::Begin("Engineers", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  ImGui::Text("Select Engineers");
  ImGui::Spacing();

  if (ImGui::Button("Select All##Engineers", ImVec2(120, 25)))
  {
    g_GameData.selectedEngineers.clear();
    for (auto const & ekv : g_GameData.engineers)
    {
      if (ekv.second.Class != g_GameData.engineerClasses[engineerClassIndex])
        continue;

      g_GameData.selectedEngineers.insert(ekv.first);
    }
  }

  ImGui::SameLine();

  if (ImGui::Button("Clear##Engineers", ImVec2(120, 25)))
    g_GameData.selectedEngineers.clear();

  if (ImGui::Combo(g_GameData.engineerClassStr.c_str(), &engineerClassIndex, guiData.ppEngineerClasses, guiData.engineerClassCount))
    g_GameData.selectedEngineers.clear();

  for (auto const & ekv : g_GameData.engineers)
  {
    if (ekv.second.Class != g_GameData.engineerClasses[engineerClassIndex])
      continue;

    bool isSelected = g_GameData.selectedEngineers.find(ekv.first) != g_GameData.selectedEngineers.end();
    if (ImGui::Checkbox(ekv.first.c_str(), &isSelected))
    {
      if (isSelected)
        g_GameData.selectedEngineers.insert(ekv.first);
      else
        g_GameData.selectedEngineers.erase(ekv.first);
    }

    if (ImGui::IsItemHovered())
    {
      ImGui::BeginTooltip();
      ImGui::TextColored(ImVec4(Default::textHighlight2), "System: %s\t", ekv.second.system.c_str());

      float dist = (g_GameData.systems.at(ekv.second.system).position - g_GameData.systems.at(g_GameData.startSystem).position).Length();
      ImGui::TextColored(ImVec4(Default::textHighlight2), "%0.2fly from %s\t", dist, g_GameData.startSystem.c_str());
      
      for (auto const & mkv : ekv.second.moduleGrades)
      {
        if (mkv.second == 5)
          ImGui::TextColored(ImVec4(Default::textHighlight1), "G%d: %s\t", mkv.second, mkv.first.c_str());
        else
          ImGui::Text("G%d: %s\t", mkv.second, mkv.first.c_str());
      }
      ImGui::EndTooltip();
    }
  }
  ImGui::End();
}

void DoModuleWindow(GUIData & guiData)
{
  static int moduleClassIndex = 0;

  ImGui::SetNextWindowPos(ImVec2(5,70));
  ImGui::SetNextWindowSize(ImVec2(320, 570));
  ImGui::Begin("Modules", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  ImGui::Text("Select Modules");
  ImGui::Spacing();
  
  if (ImGui::Button("Clear##Modules", ImVec2(120, 25)))
    g_GameData.selectedModules.clear();

  ImGui::Combo(g_GameData.moduleClassStr.c_str(), &moduleClassIndex, guiData.ppModuleClasses, guiData.moduleClassCount);

  for (auto const & mkv : g_GameData.modules)
  {
    if (mkv.second.Class != guiData.ppModuleClasses[moduleClassIndex])
      continue;

    bool isSelected = g_GameData.selectedModules.find(mkv.first) != g_GameData.selectedModules.end();
    if (ImGui::Checkbox(mkv.first.c_str(), &isSelected))
    {
      if (isSelected)
      {
        g_GameData.selectedModules.insert(std::pair<std::string, ModuleOptions>(mkv.first, {5, 1}));
      }
      else
      {
        g_GameData.selectedModules.erase(mkv.first);
      }
    }

    if (g_GameData.selectedModules.find(mkv.first) == g_GameData.selectedModules.end())
      continue;

    //ImGui::SameLine();
    std::string str = "Grade##" + mkv.first;
    ImGui::SliderInt(str.c_str(), &g_GameData.selectedModules.at(mkv.first).gradeTarget, 1, 5);

    //ImGui::SameLine();
    str = "<##" + mkv.first;
    if (ImGui::Button(str.c_str()) && g_GameData.selectedModules.at(mkv.first).priority > GameData::MinPriority)
      g_GameData.selectedModules.at(mkv.first).priority--;
    ImGui::SameLine();
    ImGui::Text("[%d]", g_GameData.selectedModules.at(mkv.first).priority);
    ImGui::SameLine();
    str = ">##" + mkv.first;
    if (ImGui::Button(str.c_str()) && g_GameData.selectedModules.at(mkv.first).priority < GameData::MaxPriority)
      g_GameData.selectedModules.at(mkv.first).priority++;
    ImGui::SameLine();
    ImGui::Text("Priority");
  }

  ImGui::End();
  
  //moduleClassButtons.Render();
  //ImGui::Separator();

  //int maxGrades[ModuleCOUNT] = {};
  //FindMaxModulesGrades(maxGrades);

  //ModuleClass currentClass = ModuleClass(moduleClassButtons.ActiveButton());

  //char buf[32];
  //for (uint32_t m = 0; m < ModuleCOUNT; m++)
  //{
  //  if (GetModuleClass(ModuleName(m)) != currentClass)
  //    continue;

  //  if (maxGrades[m] < 1)
  //  {
  //    ImGui::PushID(1);
  //    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(Default::clrLightGrey));
  //    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(Default::clrLightGrey));
  //    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(Default::clrLightGrey));
  //    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(Default::clrMinor));

  //    ImGui::Button(ToString(ModuleName(m)), buttonSize);

  //    ImGui::PopStyleColor(4);
  //    ImGui::PopID();
  //    continue;
  //  }

  //  moduleButtons[m].Render();

  //  if (!moduleButtons[m].IsOn())
  //    continue;

  //  ImGui::SameLine();

  //  ImGui::PushID(1);

  //  ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(Default::clrMajor));
  //  ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(Default::clrMajor));
  //  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(Default::clrMinor));
  //  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(Default::clrMinor));
  //  ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(Default::clrMinor));
  //  ImGui::SetNextItemWidth(50.f);
  //  sprintf(buf, "##modLevel%u", m);

  //  if (g_GameData.moduleUpgradeLevel[m] < 1)
  //    g_GameData.moduleUpgradeLevel[m] = 1;
  //  if (g_GameData.moduleUpgradeLevel[m] > maxGrades[m])
  //    g_GameData.moduleUpgradeLevel[m] = maxGrades[m];

  //  ImGui::SliderInt(buf, &g_GameData.moduleUpgradeLevel[m], 1, maxGrades[m]);

  //  // Need this as ImGui::SliderInt is bugged for ranges [x, x]
  //  if (g_GameData.moduleUpgradeLevel[m] < 1)
  //    g_GameData.moduleUpgradeLevel[m] = 1;

  //  ImGui::PopStyleColor(5);
  //  ImGui::PopID();
  //}

  //ImGui::SetCursorPos(ImVec2(60, 515));
  //if (ImGui::Button("Clear##ModuleClass", ImVec2(120, 25)))
  //{
  //  for (uint32_t m = 0; m < ModuleCOUNT; m++)
  //  {
  //    if (GetModuleClass(ModuleName(m)) != currentClass)
  //      continue;

  //    moduleButtons[m].SetState(false);
  //  }
  //}
  //ImGui::SameLine();

  //if (ImGui::Button("Clear All##ModuleClass", ImVec2(120, 25)))
  //{
  //  for (uint32_t i = 0; i < ModuleCOUNT; i++)
  //    moduleButtons[i].SetState(false);
  //}

  //g_GameData.selectedModules = 0;
  //for (uint32_t i = 0; i < ModuleCOUNT; i++)
  //{
  //  if (moduleButtons[i].IsOn())
  //    g_GameData.selectedModules |= (1ull << i);
  //  else
  //    g_GameData.selectedModules &= ~(1ull << i);
  //}

}

void DoSystemWindow(GUIData &guiData)
{
  static int startSystem = 0;
   
  ImGui::SetNextWindowPos(ImVec2(5, 5));
  ImGui::SetNextWindowSize(ImVec2(320, 60));
  ImGui::Begin("Systems", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  ImGui::Text("Choose a start system");
  ImGui::Spacing();
  ImGui::SetNextItemWidth(280);
  ImGui::Combo("##Start System", &startSystem, guiData.ppSystems, guiData.systemsCount);
  ImGui::End();

  g_GameData.startSystem = std::string(guiData.ppSystems[startSystem]);
}

void DoOutputWindow(GUIData & guiData)
{
  ImGui::SetNextWindowPos(ImVec2(876, 5));
  ImGui::SetNextWindowSize(ImVec2(350, 635));
  ImGui::Begin("Output", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  ImGui::PushID(1);
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 200, 0)));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0, 225, 0)));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0, 255, 0)));
  static char buf[2048] = {}; // TODO temp buffer for now, but come up with a better solution
  if (ImGui::Button("Run!", ImVec2(50, 25)))
  {
    std::vector<SystemNode> path;
    if (!FindBestRoute(path)) //TODO this should be in GameData.
      DebugBreak();

    std::stringstream ss;

    int count = 0;
    for (auto const & node : path)
    {
      count++;
      ss << count << ": " << g_GameData.GetEngineer(node.name) << ", " << node.name;
      for (auto const & mg : node.modules)
        ss << "\n    G" << mg.grade << ": " << mg.moduleName;
      ss << "\n";
    }

    ss << "\nTotal distance: " << PathDistance(path) << "ly";
    sprintf(buf, "%s", ss.str().c_str());
  }
  ImGui::Text("%s", buf);
  ImGui::PopStyleColor(3);
  ImGui::PopID();

  ImGui::SetCursorPos(ImVec2(90, 580));
  if (ImGui::Button("Clear##Output", ImVec2(120, 25)))
  {
    buf[0] = 0;
  }
  ImGui::SameLine();

  if (ImGui::Button("Save##Output", ImVec2(120, 25)))
  {
    ImGui::OpenPopup("Save to file");
  }

  if (ImGui::BeginPopupModal("Save to file", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
  {
    static char fileBuf[1024] = "ede_path.txt";
    ImGui::InputText("File name", fileBuf, 1024);

    static bool trySave = false;
    if (ImGui::Button("OK", ImVec2(120, 0)))
    {
      trySave = true;
      std::ifstream ifs(fileBuf);
      
      if (ifs.is_open())
        ImGui::OpenPopup("File already exists!");
      ifs.close();
    }

    if (trySave)
    {
      if (ImGui::BeginPopupModal("File already exists!", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
      {
        ImGui::Text("Overwrite?");
        if (ImGui::Button("Yes", ImVec2(80, 25)))
          ImGui::CloseCurrentPopup();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(80, 25)))
        {
          trySave = false;
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }
      else
      {
        trySave = false;
        ImGui::CloseCurrentPopup();
        std::ofstream ofs(fileBuf);
        ofs << buf;
        ImGui::CloseCurrentPopup();
      }
    }
    
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0)))
    {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  ImGui::End();
}

void Run()
{
  bool show_demo_window = false;
  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

  GUIData guiData = {};
  if (!InitGuiData(guiData))
  {
    return;
  }

  bool done = false;
  while (!done)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(g_pWindow))
        done = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame(g_pWindow);
    ImGui::NewFrame();

    // 1. Show the big demo g_pWindow (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    DoEngineersWindow(guiData);
    DoModuleWindow(guiData);
    DoSystemWindow(guiData);
    DoOutputWindow(guiData);

    // Rendering
    ImGui::Render();
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float *)&clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0); // Present with vsync
  }

  DestroyGUIData(guiData);
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] ={D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0,};
  if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
    return false;

  CreateRenderTarget();
  return true;
}

void CleanupDeviceD3D()
{
  CleanupRenderTarget();
  if (g_pSwapChain)
  {
    g_pSwapChain->Release(); g_pSwapChain = NULL;
  }
  if (g_pd3dDeviceContext)
  {
    g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL;
  }
  if (g_pd3dDevice)
  {
    g_pd3dDevice->Release(); g_pd3dDevice = NULL;
  }
}

void CreateRenderTarget()
{
  ID3D11Texture2D * pBackBuffer;
  g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
  pBackBuffer->Release();
}

void CleanupRenderTarget()
{
  if (g_mainRenderTargetView)
  {
    g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL;
  }
}

int main(int, char **)
{
  if (!Init())
    return 0;

  Run();

  ShutDown();

  return 0;
}
