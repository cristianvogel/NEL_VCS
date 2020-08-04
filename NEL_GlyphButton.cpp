//
//  NEL_GlyphButton.cpp
//  NEL_VirtualControlSurface-macOS
//
//  Created by Cristian Andres Vogel on 04/08/2020.
//

#include "NEL_GlyphButton.hpp"

NEL_GlyphButton::NEL_GlyphButton(const IRECT& bounds, int paramIdx, const char* offText, const char* onText, const IText& text, const IColor& bgColor)
: ITextControl(bounds, offText, text, bgColor)
, mOnText(onText)
, mOffText(offText)
{
  SetParamIdx(paramIdx);
  //TODO: assert boolean?
  mIgnoreMouse = false;
  mDblAsSingleClick = true;
}

NEL_GlyphButton::NEL_GlyphButton(const IRECT& bounds, IActionFunction aF, const char* offText, const char* onText, const IText& text, const IColor& bgColor)
: ITextControl(bounds, offText, text, bgColor)
, mOnText(onText)
, mOffText(offText)
{
  SetActionFunction(aF);
  mDblAsSingleClick = true;
  mIgnoreMouse = false;
}

void NEL_GlyphButton::OnMouseOver(float x, float y, const IMouseMod& mod)
{
  mText = GLYPH.WithFGColor(COLOR_WHITE);
  ITextControl::OnMouseOver(x, y, mod);
  SetDirty(false);
}

void NEL_GlyphButton::OnMouseOut()
{
  mText = GLYPH;
  SetDirty(false);
}

void NEL_GlyphButton::OnMouseDown(float x, float y, const IMouseMod& mod)
{
  if(GetValue() < 0.5)
    SetValue(1.);
  else
    SetValue(0.);
  
  SetDirty(true);
}

void NEL_GlyphButton::SetDirty(bool push, int valIdx)
{
  if(GetValue() > 0.5)
    SetStr(mOnText.Get());
  else
    SetStr(mOffText.Get());
  
  IControl::SetDirty(push);
}
