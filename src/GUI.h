#ifndef GUI_H
#define GUI_H

#include <vector>
#include "ImGui/imgui.h"

struct Default
{
  static ImColor clrLightGrey;
  static ImColor clrMinor;
  static ImColor clrMajor;
  static ImColor textHighlight1;
  static ImColor textHighlight2;
};

class MouseDownButton
{
public:

  MouseDownButton(char const * pLabel, ImVec2 size, ImColor onClr, ImColor offClr);

  void Render();

  void SetState(bool state);
  bool IsOn() const;
  bool WasPressed() const;
  void SetPressed(bool);
  bool IsHovered() const;

private:

  const char * m_pLabel;
  bool m_state;
  bool m_ready;
  bool m_wasPressed;
  bool m_isHovered;
  ImVec2 m_size;
  ImColor m_onClr;
  ImColor m_offClr;
};

class RadioButtons
{
public:

  RadioButtons();

  void AddButton(MouseDownButton const & button);

  size_t Size() const;
  void Render();
  int ActiveButton() const;
  void SetActiveButton(int index);
  int WasPressed() const; // Returns > 0 only if a button was pressed and the active button has changed, -1 otherwise

private:
  int m_currentActive;
  std::vector<MouseDownButton> m_buttons;
};


#endif