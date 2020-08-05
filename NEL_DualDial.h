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

class NEL_DualDial : public IKnobControlBase
{
public:
  NEL_DualDial(
                const IRECT& bounds
                , const std::initializer_list< int >& params
                , const IColor& stop1 = getSwatch( Lunada, 0)
                , const IColor& stop2 = getSwatch( Lunada, 1)
                , const IColor& stop3 = getSwatch( Lunada, 2)
                , float flashRate = 0.0f
                , float a1 = -135.f, float a2 = 135.f, float aAnchor = -135.f
                );
   
  float timerMillis { 0 };
  void setFlashRate(float);
  iplug::Timer* doubleDialPulseTimer;
  std::function<void( iplug::Timer & )> timerFunc = nullptr;
  const bool& togglePulse();
  bool pulse { false };
  
  void Draw(IGraphics& p) override;
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override;
  void OnMouseWheel(float x, float y, const IMouseMod& mod, float d) override;
  
  NEL_DualDial* setupButtonStateSVG( const ISVG& on , const ISVG& off);
  NEL_DualDial* setTickMarkSVG( const ISVG& tickMark  );
  int tickMarks {10};
  std::vector<ISVG> buttonStates;
  
  
  IRECT* getDialBounds();
  
private:
  const float rateMillisFactor = 1000.f;
  double mMouseDragValue = 0;
  float mTrackToHandleDistance = 4.f;
  float mInnerPointerFrac = 0.1f;
  float mOuterPointerFrac = 1.f;
  float mPointerThickness = 2.5f;
  float mAngle1, mAngle2;
  float mTrackSize = static_cast<float> ( fmax( 4, 20 - NBR_DUALDIALS) );
  float mAnchorAngle; // for bipolar arc
  
  IColor colourStop1;
  IColor colourStop2;
  IColor colourStop3;
  float innerCircleFlashRate = 0.0f;
  
};

