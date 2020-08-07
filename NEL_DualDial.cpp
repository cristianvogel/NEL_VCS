//
//  NELDoubleDial.cpp
//  APP
//
//  Created by Cristian Andres Vogel on 02/07/2020.
//

#include "IPlugTimer.h"
#include "NEL_DualDial.h"
#include "GlobSeqHelpers.h"
#include <iostream>
#include <atomic>

NEL_DualDial::NEL_DualDial(
              const IRECT& bounds
              , const std::initializer_list<int>& params
              , const IColor& stop1
              , const IColor& stop2
              , const IColor& stop3
              , float flashRate
              , float a1 , float a2 , float aAnchor
              )
: IKnobControlBase(bounds)
, mAngle1(a1)
, mAngle2(a2)
, mAnchorAngle(aAnchor)
, innerCircleFlashRate(flashRate)
, colourStop1( stop1 )
, colourStop2( stop2 )
, colourStop3( stop3 )

#pragma mark constructor body
{
  // Pulse timer function
  pulseTimerFunc = [&]( iplug::Timer & t )
  {
    timerMillis += innerCircleFlashRate;
    if (timerMillis>1.0f)
    {
      timerMillis = 0;
      togglePulse(true);
      SetDirty(false);
    }
  };
  
  doubleDialPulseTimer->Create(pulseTimerFunc, 150.0f);
  
  int allValues = static_cast<int>(params.size());
  SetNVals(allValues);
  
  int valIdx = 0;
  for (auto param : params)
  {
    SetParamIdx(param, valIdx++);
  }
  SetValue(0.5, 0);
  SetValue(0.5, 1);
  
}

#pragma mark Draw()

void NEL_DualDial::Draw(IGraphics& g) {

  float radius;
  int tickFan = std::abs(mAnchorAngle)*2;
  int tickStep = tickFan / tickMarks;
  
  if(mRECT.W() > mRECT.H())
    radius = (mRECT.H()/2.f);
  else
    radius = (mRECT.W()/2.f);
  
  const float cx = mRECT.MW(), cy = mRECT.MH();
  
  radius -= (mTrackSize/2.f);
  
  radius = radius * 0.75f;
  
  float angle = mAngle1 + (static_cast<float>(GetValue(0)) * (mAngle2 - mAngle1));
 
  IColor interpStage = IColor::LinearInterpolateBetween(colourStop2, colourStop3, static_cast<float>(fmin(1.0f, GetValue(0)*2.0f)));
  

#pragma mark flashing
  
  togglePulse(false);
  //g.DrawSVG( buttonStates [ togglePulse(false) ], mRECT.GetCentredInside(fmin(mRECT.W(), 50.0f + (NBR_DUALDIALS * 1.618f))));

#pragma mark outer arc and ticks
  g.DrawCircle(COLOR_WHITE, cx, cy, radius,nullptr, 0.5f);
  
  float tickIndex = 0;
  float opacity = 0;
  float outerRingVal = static_cast<float>(GetValue(0));
  float innerRingVal = static_cast<float>(GetValue(1));
  IColor outerDialShade = IColor::LinearInterpolateBetween(colourStop1, interpStage, static_cast<float>(GetValue(0)));
  IColor innerDialShade = IColor::LinearInterpolateBetween(colourStop1, colourStop2, static_cast<float>(GetValue(1)));

  
  for (int i= mAnchorAngle + (tickStep/2); i<= angle; i+= tickStep) {
     tickIndex+= 1.0f / tickMarks;
    opacity = (i >= (angle - tickStep) + (tickStep/2)) ? pulse[0] :  GlobSeqHelpers::lerp(0.2f, 0.9f , tickIndex ) ;
    
    g.DrawRadialLine( innerDialShade.WithOpacity( opacity ).WithContrast(tickIndex) , cx, cy, i , (radius + 10), (radius + 12.5) + (5 * innerRingVal) , &mBlend , 1.f);
  }
  
  g.DrawArc( outerDialShade,
            cx, cy, radius,
            angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle),
            angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, mTrackSize);

  g.DrawArc( (static_cast<float>(GetValue(0)) > 0.5f) ? colourStop2.WithOpacity(0.75f)  : COLOR_GRAY.WithOpacity(0.75f)
            , cx, cy, radius
            , mAnchorAngle
            , -mAnchorAngle
            , &mBlend
            , mTrackSize * 0.75f
            );

#pragma mark inner arc and ticks
  
  angle = mAngle1 + (static_cast<float>(GetValue(1)) * (mAngle2 - mAngle1));
  tickIndex = 0 ;
  for (int i= mAnchorAngle; i<= angle; i+= tickStep) {
    tickIndex+= 1 / tickMarks;
    opacity = ( i >= (angle - tickStep) ) ? pulse[1] : GlobSeqHelpers::lerp( outerRingVal, 1.0f , tickIndex ) ;
    g.DrawRadialLine( innerDialShade.WithOpacity( opacity ).WithContrast(tickIndex) , cx, cy, i , (radius + 10), (radius + 12.5) + (5 * outerRingVal) , &mBlend , 1.f);
  }
  
  radius -= mTrackSize;
  g.DrawCircle(COLOR_GRAY, cx, cy, radius,nullptr, 0.5f);
  g.DrawArc(innerDialShade,
             cx, cy, radius,
            angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle),
            angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, mTrackSize);

  
#pragma mark Inner status LED affirmation segment
   g.DrawArc( (static_cast<float>(GetValue(1)) > 0.5f) ? colourStop1 : COLOR_GRAY
             , cx, cy, radius + mTrackSize
             , -mAnchorAngle
             , mAnchorAngle
             , &mBlend
             , mTrackSize * 0.618f
             );
}

void NEL_DualDial::OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod)
{
  double gearing = IsFineControl(mod, false) ? mGearing * 10.0 : mGearing;
  
  IRECT dragBounds = GetKnobDragBounds();
  
  if (mDirection == EDirection::Vertical)
    mMouseDragValue += static_cast<double>(dY / static_cast<double>(dragBounds.T - dragBounds.B) / gearing);
  else
    mMouseDragValue += static_cast<double>(dX / static_cast<double>(dragBounds.R - dragBounds.L) / gearing);
  
  mMouseDragValue = iplug::Clip(mMouseDragValue, 0., 1.);
 
  SetValue( mMouseDragValue , mod.C ? 1 : 0 /*needs to change depending on arc clicked */);
  SetDirty();
}

void NEL_DualDial::OnMouseWheel(float x, float y, const IMouseMod& mod, float d)
{
  double gearing = IsFineControl(mod, true) ? -0.001 : -0.01;
  SetValue(GetValue(1) + gearing * d, 1);
  SetDirty();
}


const bool NEL_DualDial::togglePulse( const bool update )
{
  
  pulse[0] = !pulse[0] xor pulse[1];
  
  if (update) {
      pulse[1] = !pulse[1];
  }
  return pulse[1] ;
  //two pulses generated
}

void NEL_DualDial::setFlashRate(float  rate)
{
  innerCircleFlashRate = rate ;
}


NEL_DualDial* NEL_DualDial::setupButtonStateSVG(const ISVG& on, const ISVG& off){
  buttonStates.push_back(on);
  buttonStates.push_back(off);
  return this;
}


IRECT* NEL_DualDial::getDialBounds() {
  return &mRECT;
}



