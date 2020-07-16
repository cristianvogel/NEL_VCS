//
//  NELDoubleDial.hpp
//  APP
//
//  Created by Cristian Andres Vogel on 02/07/2020.
//

#pragma once

#include <stdio.h>
#include "IControl.h"
#include "IPlugTimer.h"
#include "NEL_VCS_Constants.h"

//using namespace iplug;
using namespace iplug::igraphics;

class NELDoubleDial : public IKnobControlBase
{
public:
  NELDoubleDial(
                const IRECT& bounds
                , const std::initializer_list< int >& params
                , const iplug::igraphics::IColor& stop1 = getSwatch( Lunada, 0)
                , const iplug::igraphics::IColor& stop2 = getSwatch( Lunada, 1)
                , const iplug::igraphics::IColor& stop3 = getSwatch( Lunada, 2)
                , float flashRate = 0.0f
                , float a1 = -135.f, float a2 = 135.f, float aAnchor = -135.f
                );
 
  double doubleDialTimerCount = 0;
  std::unique_ptr<iplug::Timer> doubleDialPulseTimer;
  bool pulse { false };
  
  
  
  void Draw(IGraphics& p) override;
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override;
  void OnMouseWheel(float x, float y, const IMouseMod& mod, float d) override;

  void setFlashRate(float);
  const bool& togglePulse();
  
  void setButtonStates( const ISVG& on , const ISVG& off);
  
private:
  double mMouseDragValue = 0;
  float mTrackToHandleDistance = 4.f;
  float mInnerPointerFrac = 0.1f;
  float mOuterPointerFrac = 1.f;
  float mPointerThickness = 2.5f;
  float mAngle1, mAngle2;
  float mTrackSize = static_cast<float> ( fmax( 4, 14 - NBR_DUALDIALS) );
  float mAnchorAngle; // for bipolar arc
  
  iplug::igraphics::IColor colourStop1;
  iplug::igraphics::IColor colourStop2;
  iplug::igraphics::IColor colourStop3;
  float innerCircleFlashRate = 0.0f;
  std::vector<ISVG> buttonStates;
  int16_t frameCount {0};
};

