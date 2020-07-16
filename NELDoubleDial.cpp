//
//  NELDoubleDial.cpp
//  APP
//
//  Created by Cristian Andres Vogel on 02/07/2020.
//

#include "IPlugTimer.h"
#include "NELDoubleDial.h"
#include <iostream>

NELDoubleDial::NELDoubleDial(
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
{//constructor body

  doubleDialPulseTimer = std::unique_ptr<iplug::Timer>(
                                         iplug::Timer::Create([&](iplug::Timer& t)
                                    {
                                      doubleDialTimerCount = (innerCircleFlashRate > 0.0f) ? doubleDialTimerCount + (innerCircleFlashRate * 0.1f) : 0;
                                      SetDirty(false);
                                    }
                                    , 10.0f)
                                  );
  
  int maxNTracks = static_cast<int>(params.size());
  SetNVals(maxNTracks);
  
  int valIdx = 0;
  for (auto param : params)
  {
    SetParamIdx(param, valIdx++);
  }
  SetValue(0.5, 0);
  SetValue(0.5, 1);
}

void NELDoubleDial::Draw(IGraphics& g) {

  float radius;
  //frameCount = 1 + (frameCount%65534) ;
  if(mRECT.W() > mRECT.H())
    radius = (mRECT.H()/2.f);
  else
    radius = (mRECT.W()/2.f);
  
  const float cx = mRECT.MW(), cy = mRECT.MH();
  
  radius -= (mTrackSize/2.f);
  
  float angle = mAngle1 + (static_cast<float>(GetValue(0)) * (mAngle2 - mAngle1));
 
  IColor interpStage = IColor::LinearInterpolateBetween(colourStop2, colourStop3, static_cast<float>(fmin(1.0f, GetValue(0)*2.0f)));
  
#pragma mark flashing button indicator
  
  g.DrawSVG(buttonStates[ togglePulse() ? 1 : 0 ], mRECT.GetCentredInside(fmin(mRECT.W(), 50.0f + (NBR_DUALDIALS * 1.618f))));
  
  
#pragma mark outer internal LED
  g.DrawCircle(COLOR_WHITE, cx, cy, radius,nullptr, 0.5f);
  g.DrawArc(IColor::LinearInterpolateBetween(colourStop1, interpStage, static_cast<float>(GetValue(0))),
            cx, cy, radius,
            angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle),
            angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, mTrackSize);

  g.DrawArc( (static_cast<float>(GetValue(0)) > 0.5f) ? colourStop2.WithOpacity(0.75f)  : COLOR_GRAY.WithOpacity(0.75f)
            , cx, cy, radius
            , mAnchorAngle // todo: animate with VUMeter value
            , -mAnchorAngle
            , &mBlend
             , mTrackSize * 0.618f
            );

#pragma mark inner internal LED
  radius -= mTrackSize;
  angle = mAngle1 + (static_cast<float>(GetValue(1)) * (mAngle2 - mAngle1));
  
  g.DrawCircle(COLOR_GRAY, cx, cy, radius,nullptr, 0.5f);
  g.DrawArc(IColor::LinearInterpolateBetween(colourStop1, colourStop2, static_cast<float>(GetValue(1))),
             cx, cy, radius,
            angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle),
            angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, mTrackSize);
  
  
#pragma mark Inner status LED
   g.DrawArc( (static_cast<float>(GetValue(1)) > 0.5f) ? colourStop1 : COLOR_GRAY
             , cx, cy, radius + mTrackSize
             , -mAnchorAngle
             , mAnchorAngle
             , &mBlend
             , mTrackSize * 0.618f
             );
}

void NELDoubleDial::OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod)
{
  double gearing = IsFineControl(mod, false) ? mGearing * 10.0 : mGearing;
  
  IRECT dragBounds = GetKnobDragBounds();
  
  if (mDirection == EDirection::Vertical)
    mMouseDragValue += static_cast<double>(dY / static_cast<double>(dragBounds.T - dragBounds.B) / gearing);
  else
    mMouseDragValue += static_cast<double>(dX / static_cast<double>(dragBounds.R - dragBounds.L) / gearing);
  
  mMouseDragValue = iplug::Clip(mMouseDragValue, 0., 1.);
 
  /* QUANTISATION / STEPPED DIAL
      double v = mMouseDragValue;
      const IParam* pParam = GetParam();
  
       if (pParam && pParam->GetStepped() && pParam->GetStep() > 0)
       {
         const double range = pParam->GetRange();
   
         if (range > 0.)
         {
           double l, h;
           pParam->GetBounds(l, h);
   
           v = l + mMouseDragValue * range;
           v = v - std::fmod(v, pParam->GetStep());
           v -= l;
           v /= range;
         }
       }
   */
  setFlashRate(mMouseDragValue);
  SetValue( mMouseDragValue , mod.C ? 1 : 0 /*needs to change depending on arc clicked */);
  SetDirty();
}

void NELDoubleDial::OnMouseWheel(float x, float y, const IMouseMod& mod, float d)
{
  double gearing = IsFineControl(mod, true) ? -0.001 : -0.01;
  SetValue(GetValue(1) + gearing * d, 1);
  SetDirty();
}

const bool& NELDoubleDial::togglePulse()
{
  if (static_cast<int>(doubleDialTimerCount) % 2) pulse = !pulse;
  
  return pulse;
}

void NELDoubleDial::setFlashRate(float  rate)
{
  innerCircleFlashRate = rate;
}

void NELDoubleDial::setButtonStates(const ISVG& on, const ISVG& off){
  buttonStates.push_back(on);
  buttonStates.push_back(off);
}

