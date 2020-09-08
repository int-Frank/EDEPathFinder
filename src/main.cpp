
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>

#include <sstream>
#include <fstream>

#include <glad/glad.h>

#include "GameData.h"
#include "GUI.h"
#include "Path.h"

SDL_GLContext g_GLContext = nullptr;
SDL_Window * g_pWindow = nullptr;

bool Init()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    return false;
  }

  // GL 3.0 + GLSL 130
  const char * glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // Create g_pWindow with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  g_pWindow = SDL_CreateWindow("ED Engineer Path Finder", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1231, 645, window_flags);
  
  {
    Uint32 pixels[64*64] =
    {
#include "icon.inl"
    };

    SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(pixels, 64, 64, 32, 64*4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_SetWindowIcon(g_pWindow, surface);
    SDL_FreeSurface(surface);
  }

  g_GLContext = SDL_GL_CreateContext(g_pWindow);
  SDL_GL_MakeCurrent(g_pWindow, g_GLContext);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  // Initialize OpenGL loader
  bool err = gladLoadGL() == 0;
  if (err)
  {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return  false;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(g_pWindow, g_GLContext);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return true;
}

void ShutDown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(g_GLContext);
  SDL_DestroyWindow(g_pWindow);
  SDL_Quit();
}

void DoEngineersWindow()
{
  static MouseDownButton engineers[EngineerCOUNT] = 
  {
#undef ITEM2
#undef ITEM3
#define ITEM2(a, c) MouseDownButton(ToString(e ## a), ImVec2(200, 40), Default::clrMajor, Default::clrMinor),
#define ITEM3(a, c, str) MouseDownButton(ToString(e ## a), ImVec2(200, 40), Default::clrMajor, Default::clrMinor),
    UNROLL_ENGINEERS
  };

  static bool isInit = false;
  static RadioButtons regions;
  static char const * engineerPriorities[] ={"1", "2", "3", "4", "5", "6", "7", "8", "9"}; //Ensure this aligns with GameData::[Min][Max]ModuleRank

  if (!isInit)
  {
    for (uint32_t i = 0; i < RegionCOUNT; i++)
      regions.AddButton(MouseDownButton(ToString(Region(i)), ImVec2(120, 25), Default::clrMajor, Default::clrMinor));
    isInit = true;
  }

  ImGui::SetNextWindowPos(ImVec2(330, 5));
  ImGui::SetNextWindowSize(ImVec2(542, 635));
  ImGui::Begin("Engineers", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  ImGui::Text("Select Engineers");
  ImGui::Spacing();
  regions.Render();

  Region reg = Region(regions.ActiveButton());

  if (regions.WasPressed())
  {
    for (uint32_t i = 0; i < EngineerCOUNT; i++)
      engineers[i].SetState(false);
  }

  uint32_t const elePerRow = 2;
  ImGui::Separator();

  uint32_t placedEngineers = 0;
  for (uint32_t e = 0; e < EngineerCOUNT; e++)
  {
    if (GetRegion(GetSystem(EngineerName(e))) != reg)
      continue;

    if ((placedEngineers) % elePerRow != 0)
      ImGui::SameLine();

    placedEngineers++;

    ImGui::BeginGroup();
    engineers[e].Render();

    if (ImGui::IsItemHovered())
    {
      ImGui::BeginTooltip();
      ImGui::TextColored(ImVec4(Default::textHighlight2), "System: %s\t", ToString(GetSystem(EngineerName(e))));
      for (int g = 5; g > 0; g--)
      {
        for (uint32_t t = 0; t < ModuleCOUNT; t++)
        {
          if (g_GameData.engineer[e].moduleGrade[t] != g)
            continue;

          if (g == 5)
            ImGui::TextColored(ImVec4(Default::textHighlight1), "G%d: %s\t", g, ToString(ModuleName(t)));
          else
            ImGui::Text("G%d: %s\t", g, ToString(ModuleName(t)));
        }
      }
      ImGui::EndTooltip();
    }

    ImGui::SameLine();
    ImGui::BeginGroup();

    ImGui::PushID(1);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(Default::clrMinor));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(Default::clrMinor));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(Default::clrMinor));
    ImGui::Button(engineerPriorities[g_GameData.engineerPriorities[e] - 1], ImVec2(25, 40));

    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("Priority value. Engineers with higher\npriority values will be visited first.");
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();
    ImGui::EndGroup();
    ImGui::SameLine();

    static MouseDownButton bUp("^##EngPriUp", ImVec2(17, 17), Default::clrMinor, Default::clrMinor);
    static MouseDownButton bDown("v##EngPriDown", ImVec2(17, 17), Default::clrMinor, Default::clrMinor);

    ImGui::BeginGroup();

    bUp.Render();
    bDown.Render();

    if (bUp.WasPressed() && g_GameData.engineerPriorities[e] < GameData::MaxPriority)
      g_GameData.engineerPriorities[e]++;
    if (bDown.WasPressed() && g_GameData.engineerPriorities[e] > GameData::MinPriority)
      g_GameData.engineerPriorities[e]--;

    ImGui::EndGroup();
    ImGui::EndGroup();
  }

  ImGui::SetCursorPos(ImVec2(154, 579));
  if (ImGui::Button("Select All", ImVec2(120, 25)))
  {
    for (uint32_t e = 0; e < EngineerCOUNT; e++)
    {
      if (GetRegion(GetSystem(EngineerName(e))) == reg)
        engineers[e].SetState(true);
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear", ImVec2(120, 25)))
  {
    for (uint32_t e = 0; e < EngineerCOUNT; e++)
      engineers[e].SetState(false);
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset Priorities", ImVec2(120, 25)))
  {
    for (uint32_t e = 0; e < EngineerCOUNT; e++)
    {
      if (GetRegion(GetSystem(EngineerName(e))) == reg)
        g_GameData.engineerPriorities[e] = GameData::MinPriority;
    }
  }

  g_GameData.selectedEngineers = 0;
  for (uint32_t e = 0; e < EngineerCOUNT; e++)
  {
    if (engineers[e].IsOn())
      g_GameData.selectedEngineers |= (1ull << e);
    else
      g_GameData.selectedEngineers &= ~(1ull << e);
  }

  ImGui::End();
}

void DoModuleWindow()
{
  static bool isInit = false;
  static RadioButtons moduleClasses;

  static MouseDownButton modules[ModuleCOUNT] =
  {
#undef ITEM2
#undef ITEM3
#define ITEM2(a, c) MouseDownButton(ToString(m ## a), ImVec2(304, 30), Default::clrMajor, Default::clrMinor),
#define ITEM3(a, c, str) MouseDownButton(ToString(m ## a), ImVec2(304, 30), Default::clrMajor, Default::clrMinor),
     UNROLL_MODULES
  };

  if (!isInit)
  {
    for (uint32_t i = 0; i < ModuleClassCOUNT; i++)
      moduleClasses.AddButton(MouseDownButton(ToString(ModuleClass(i)), ImVec2(70, 25), Default::clrMajor, Default::clrMinor));
    isInit = true;
  }
  ImGui::SetNextWindowPos(ImVec2(5,70));
  ImGui::SetNextWindowSize(ImVec2(320, 570));
  ImGui::Begin("Modules", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  ImGui::Text("Select Modules");
  ImGui::Spacing();
  moduleClasses.Render();
  ImGui::Separator();

  static ModuleClass lastClass = mcCore;
  ModuleClass currentClass = ModuleClass(moduleClasses.ActiveButton()); // TODO use WasPressed

  if (lastClass != currentClass)
  {
    lastClass = currentClass;
  }

  for (uint32_t i = 0; i < ModuleCOUNT; i++)
  {
    if (GetModuleClass(ModuleName(i)) != currentClass)
      continue;

    modules[i].Render();
  }

  ImGui::SetCursorPos(ImVec2(60, 515));
  if (ImGui::Button("Clear##ModuleClass", ImVec2(120, 25)))
  {
    for (uint32_t i = 0; i < ModuleCOUNT; i++)
    {
      if (GetModuleClass(ModuleName(i)) != currentClass)
        continue;

      modules[i].SetState(false);
    }
  }
  ImGui::SameLine();

  if (ImGui::Button("Clear All##ModuleClass", ImVec2(120, 25)))
  {
    for (uint32_t i = 0; i < ModuleCOUNT; i++)
      modules[i].SetState(false);
  }

  g_GameData.selectedModules = 0;
  for (uint32_t i = 0; i < ModuleCOUNT; i++)
  {
    if (modules[i].IsOn())
      g_GameData.selectedModules |= (1ull << i);
    else
      g_GameData.selectedModules &= ~(1ull << i);
  }

  ImGui::End();
}

void DoSystemWindow()
{
  static const char * systems[] =
  {
#undef ITEM2
#undef ITEM3
#define ITEM2(a, r) #a,
#define ITEM3(a, r, str) str,
    UNROLL_SYSTEMS
  };

  static int startSystem = int(sShinrartaDezhra);
   
  ImGui::SetNextWindowPos(ImVec2(5, 5));
  ImGui::SetNextWindowSize(ImVec2(320, 60));
  ImGui::Begin("Systems", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  ImGui::Text("Choose a start system");
  ImGui::Spacing();
  ImGui::SetNextItemWidth(280);
  ImGui::Combo("##Start System", &startSystem, systems, IM_ARRAYSIZE(systems));
  ImGui::End();

  g_GameData.startSystem = SystemName(startSystem);
}

void DoOutputWindow()
{
  ImGui::SetNextWindowPos(ImVec2(876, 5));
  ImGui::SetNextWindowSize(ImVec2(350, 635));
  ImGui::Begin("Output", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
  ImGui::PushID(1);
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 200, 0)));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0, 225, 0)));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0, 255, 0)));
  static char buf[2048] = {};
  if (ImGui::Button("Run!", ImVec2(50, 25)))
  {
    std::vector<SystemNode> path = FindShortestPath(); //TODO this should be in GameData.
    std::stringstream ss;

    int count = 0;
    for (auto const & node : path)
    {
      count++;
      ss << count << ": " << ToString(node.name);
      for (auto const & mg : node.modules)
        ss << "\n    G" << (int)mg.Grade() << ": " << ToString(mg.Name());
      ss << "\n";
    }
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

// TODO Add save/load settings, or even connect with the game data to auto load unlocked engineers.
//      Possible even connect with https: //coriolis.io/ for module list
// TODO Separate the path finder engine from the UI. Maybe build into a separate lib.
// TODO Use brute force to find a set of shortest paths to test against (up to about 10 systems per path should be fine)
// TODO Develop a testing framework, that maybe loads in test cases from a file
// TODO Allow users to add extra start systems
// TODO Add ability to include an end system, in the case the user wants to go somewhere after upgrading
// TODO Add window that displays path in 3D
void Run()
{
  // Our state
  bool show_demo_window = false;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

  // Main loop
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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(g_pWindow);
    ImGui::NewFrame();

    // 1. Show the big demo g_pWindow (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    DoEngineersWindow();
    DoModuleWindow();
    DoSystemWindow();
    DoOutputWindow();

    // Rendering
    ImGui::Render();
    ImGuiIO & io = ImGui::GetIO(); (void)io;
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(g_pWindow);
  }
}

float Distance(std::initializer_list<SystemName> systems)
{
  float result = 0.f;
  auto it0 = systems.begin();
  auto it1 = it0;
  it1++;
  while (it1 != systems.end())
  {
    SystemName ni = *it0;
    SystemName nj = *it1;

    Float3 pi = g_GameData.system[*it0].position;
    Float3 pj = g_GameData.system[*it1].position;

    result += (pi - pj).Length();

    it0 = it1;
    it1++;
  }
  return result;
}

void Tests()
{
  {
    //Start system = Shinrarta Dezhra, Modules: FSD, Armour
    float f = Distance({sShinrartaDezhra, sDeciat, sKuk});
  }
  {
    //Start system = Shinrarta Dezhra, Modules: Power distributor, Shileds, Shiled booster
    float f = Distance({sShinrartaDezhra, sLeesti, sWyrd, sLaksak});
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
