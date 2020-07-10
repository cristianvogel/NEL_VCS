//
//  NELDoubleDial.hpp
//  APP
//
//  Created by Cristian Andres Vogel on 02/07/2020.
//

#pragma once

#include <stdio.h>
#include "IControl.h"
#include "NEL_VCS_Constants.hpp"
#include "IPlugTimer.h"

using namespace iplug;
using namespace iplug::igraphics;

class NELDoubleDial : public IKnobControlBase
{
public:
  NELDoubleDial(
                const IRECT& bounds
                , const std::initializer_list< int >& params
                , const IColor& stop1 = GetSwatch( Lunada, 0)
                , const IColor& stop2 = GetSwatch( Lunada, 1)
                , const IColor& stop3 = GetSwatch( Lunada, 2)
                , float flashRate = 0.0f
                , float a1 = -135.f, float a2 = 135.f, float aAnchor = -135.f
                );
 
  double mTimerCount = 0.0;
  std::unique_ptr<Timer> mTimer;
  Boolean pulse = false;
  
  void Draw(IGraphics& p) override;
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override;
  void OnMouseWheel(float x, float y, const IMouseMod& mod, float d) override;

  void setFlashRate(float);
  Boolean togglePulse() { pulse = !pulse; return pulse;}
  
private:
  double mMouseDragValue = 0;
  float mTrackToHandleDistance = 4.f;
  float mInnerPointerFrac = 0.1f;
  float mOuterPointerFrac = 1.f;
  float mPointerThickness = 2.5f;
  float mAngle1, mAngle2;
  float mTrackSize = static_cast<float> ( fmax( 4, 14 - NBR_DUALDIALS) );
  float mAnchorAngle; // for bipolar arc
  
  IColor mColourStop1;
  IColor mColourStop2;
  IColor mColourStop3;
  
  float mFlashRate = 0.0f;
  
};

