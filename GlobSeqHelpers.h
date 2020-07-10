//
//  GlobSeqHelpers.hpp
//  All
//
//  Created by Cristian Andres Vogel on 01/07/2020.
//
#pragma once

class GlobSeqHelpers
{
public:
  
  GlobSeqHelpers() //: iplug::OSCReciever(8080)
  {}
  
  ~GlobSeqHelpers()
  {}

  std::string chomp(std::string &str);
};

/*
class ChangeColorEverySecondControl : public IControl
{
public:
  ChangeColorEverySecondControl(const IRECT& bounds)
  : IControl(bounds)
  {
    mTimer = std::unique_ptrTimer(Timer::Create([&](Timer& t) { mColor = IColor::GetRandomColor(); SetDirty(false);  }, 1000));
  }
  void Draw(IGraphics& g)
  {
    g.FillRect(mColor, mRECT);
  }
  IColor mColor;
  std::unique_ptrTimer mTimer;
};

*/
