
#include "GUI.h"

ImColor Default::clrMinor = ImColor(55, 55, 55);
ImColor Default::clrMajor = ImColor(219, 95, 7);
ImColor Default::textHighlight1 = ImColor(255, 0, 255);
ImColor Default::textHighlight2 = ImColor(0, 194, 6);

MouseDownButton::MouseDownButton(char const * pLabel, ImVec2 size, ImColor onClr, ImColor offClr)
  : m_pLabel(pLabel)
  , m_state(false)
  , m_ready(true)
  , m_wasPressed(false)
  , m_isHovered(false)
  , m_size(size)
  , m_onClr(onClr)
  , m_offClr(offClr)
{

}

void MouseDownButton::Render()
{
  m_wasPressed = false;
  ImColor val = m_state ? m_onClr : m_offClr;

  ImGuiIO & io = ImGui::GetIO();

  ImGui::PushID(1);
  ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)val);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)val);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)val);

  if (ImGui::Button(m_pLabel, m_size))
    m_ready = true;

  m_isHovered = ImGui::IsItemHovered();

  if (m_ready && ImGui::IsItemActive())
  {
    m_state = !m_state;
    m_ready = false;
    m_wasPressed = true;
  }
  else if (!io.MouseDown[0] && !m_ready)
  {
    m_ready = true;
  }

  ImGui::PopStyleColor(3);
  ImGui::PopID();
}

void MouseDownButton::SetState(bool state)
{
  m_state = state;
}

bool MouseDownButton::IsOn() const
{
  return m_state;
}

bool MouseDownButton::WasPressed() const
{
  return m_wasPressed;
}

void MouseDownButton::SetPressed(bool val)
{
  m_wasPressed = val;
}

bool MouseDownButton::IsHovered() const
{
  return m_isHovered;
}

RadioButtons::RadioButtons()
  : m_currentActive(-1)
{

}

void RadioButtons::AddButton(MouseDownButton const & button)
{
  m_buttons.push_back(button);

  if (m_buttons.size() == 1)
  {
    m_currentActive = 0;
    m_buttons[0].SetState(true);
  }
  else
  {
    m_buttons.back().SetState(false);
  }
}

void RadioButtons::Render()
{
  for (int i = 0; i < (int)m_buttons.size(); i++)
  {
    m_buttons[i].Render();

    if (m_buttons[i].IsOn() && i != m_currentActive)
    {
      m_buttons[m_currentActive].SetState(false);
      m_currentActive = i;
    }

    if (i != (int)m_buttons.size() - 1)
      ImGui::SameLine();
  }

  bool oneActive = false;
  for (int i = 0; i < (int)m_buttons.size(); i++)
  {
    if (m_buttons[i].IsOn())
      oneActive = true;
  }

  if (!oneActive)
  {
    m_buttons[m_currentActive].SetState(true);
    m_buttons[m_currentActive].SetPressed(false);
  }
}

size_t RadioButtons::Size() const
{
  return m_buttons.size();
}

int RadioButtons::ActiveButton() const
{
  return m_currentActive;
}

void RadioButtons::SetActiveButton(int index)
{
  if (index < 0 || index >(int)m_buttons.size())
    return;

  for (MouseDownButton & button : m_buttons)
    button.SetState(false);

  m_buttons[index].SetState(true);
}

int RadioButtons::WasPressed() const
{
  for (int i = 0; i < (int)m_buttons.size(); i++)
  {
    if (m_buttons[i].WasPressed())
      return i;;
  }
  return -1;
}