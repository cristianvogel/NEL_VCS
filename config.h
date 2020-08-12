#pragma once

#define PLUG_NAME "NEL_VirtualControlSurface"
#define PLUG_MFR "NeverEngineLabs"
#define PLUG_VERSION_HEX 0x00010000
#define PLUG_VERSION_STR "1.0.0"
#define PLUG_UNIQUE_ID 'VKob'
#define PLUG_MFR_ID 'Acme'
#define PLUG_URL_STR "https://iplug2.github.io"
#define PLUG_EMAIL_STR "spam@me.com"
#define PLUG_COPYRIGHT_STR "Copyright 2020 Cristian Vogel"
#define PLUG_CLASS_NAME NEL_VirtualControlSurface

#define BUNDLE_NAME "NEL_VirtualControlSurface"
#define BUNDLE_MFR "NeverEngineLabs"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "NEL_VirtualControlSurface"

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_TYPE 0
#define PLUG_DOES_MIDI_IN 0
#define PLUG_DOES_MIDI_OUT 0
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 650
#define PLUG_HEIGHT 400
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY NEL_VirtualControlSurface_Entry
#define AUV2_ENTRY_STR "NEL_VirtualControlSurface_Entry"
#define AUV2_FACTORY NEL_VirtualControlSurface_Factory
#define AUV2_VIEW_CLASS NEL_VirtualControlSurface_View
#define AUV2_VIEW_CLASS_STR "NEL_VirtualControlSurface_View"

#define AAX_TYPE_IDS 'IEF1', 'IEF2'
#define AAX_TYPE_IDS_AUDIOSUITE 'IEA1', 'IEA2'
#define AAX_PLUG_MFR_STR "Acme"
#define AAX_PLUG_NAME_STR "NEL_VirtualControlSurface\nIPEF"
#define AAX_PLUG_CATEGORY_STR "Effect"
#define AAX_DOES_AUDIOSUITE 1

#define VST3_SUBCATEGORY "Fx"

#define APP_NUM_CHANNELS 0
#define APP_N_VECTOR_WAIT 0
#define APP_MULT 1
#define APP_COPY_AUV3 0
#define APP_SIGNAL_VECTOR_SIZE 64

#define ROBOTO_FN "Roboto-Regular.ttf"
#define MENLO_FN "Menlo-Regular.ttf"
#define FORK_AWESOME_FN "forkawesome-webfont.ttf"

#define PLANE_BG "deco_svg/gradient-plane-bg-text.svg"
#define LOGO_TEXT "deco_svg/logo_text.svg"

#define NBR_DUALDIALS 8
#define NBR_WIDGETS 3

#define WIDGET_OFF ICON_FK_SQUARE_O
#define WIDGET_ON ICON_FK_MINUS_SQUARE
#define WIDGET_OFF_NO_NET ICON_FK_CIRCLE_O
#define WIDGET_ON_NO_NET ICON_FK_MINUS_CIRCLE

#define NEL_BUTTON_OFF "buttonPlainBlue.svg"
#define NEL_BUTTON_ON "buttonPlainBlue_ON.svg"
#define DEFAULT_DIAL_ADDRESS "/dualDial/"

#define DEFAULT_TARGET_PORT  9090
#define DEFAULT_LISTENER_PORT  8080




