//
//  NEL_VCS_Constants.h
//  NEL_VirtualControlSurface-macOS
//
//  Created by Cristian Andres Vogel on 07/07/2020.
//
#pragma once

#define NBR_DUALDIALS 6
#define NEL_BUTTON_OFF "buttonPlainBlue.svg"
#define NEL_BUTTON_ON "buttonPlainBlue_ON.svg"

#pragma mark NEL_Colours
const iplug::igraphics::IColor NEL_TUNGSTEN(255, 27, 27, 30);
const iplug::igraphics::IColor NEL_TUNGSTEN_FGBlend( 255, 230,  87,  74  );
const iplug::igraphics::IColor NEL_MEMARIANI_stop1(255, 78, 141, 153);
const iplug::igraphics::IColor NEL_MEMARIANI_stop2(255, 107, 107, 131);
const iplug::igraphics::IColor NEL_MEMARIANI_stop3(255, 170, 75, 107);
const iplug::igraphics::IColor NEL_LUNADA_stop1(255, 84, 51, 255);
const iplug::igraphics::IColor NEL_LUNADA_stop2(255, 32, 189, 255);
const iplug::igraphics::IColor NEL_LUNADA_stop3(255, 165, 254, 203);

enum Swatches {
  Memariani = 0,
  Lunada,
  Tungsten,
  Swatches_size
};

static iplug::igraphics::IColor getSwatch(int gradient, int stop)
{
  switch (gradient) {
    case Memariani:
    switch (stop) {
      case 0: return NEL_MEMARIANI_stop1;
      case 1: return NEL_MEMARIANI_stop2;
      case 2: return NEL_MEMARIANI_stop3;
      default:
        assert(0);
        return NEL_MEMARIANI_stop1;
      }
      case Lunada:
         switch (stop) {
           case 0: return NEL_LUNADA_stop1;
           case 1: return NEL_LUNADA_stop2;
           case 2: return NEL_LUNADA_stop3;
           default:
             assert(0);
             return NEL_LUNADA_stop1;
           }
  default:
         //assert(0);
         return NEL_MEMARIANI_stop1;
  }
}
