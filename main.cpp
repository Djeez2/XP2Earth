
#define WIN32_LEAN_AND_MEAN 
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

#include "XPLMPlugin.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"


#include "kmlclient.h"


const float DEFAULT_SCENIC_HEIGHT = 20.0f;
const float DEFAULT_SCENIC_TILT = 80.0f;
const float DEFAULT_PILOT_HEIGHT = 2.0f;
const float DEFAULT_PILOT_TILT = 85.0f;

KmlClient *gKmlClient = NULL;
static void PluginsMenuHandler(void * mRef, void * iRef);

XPLMMenuID menu_id;
int track_menu_index;
int pilot_view_menu_index;
int look_around_menu_index;

XPLMCommandRef cmdTrackingOn;
XPLMCommandRef cmdTrackingOff;
XPLMCommandRef cmdTrackingToggle;
XPLMCommandRef cmdConnectEarth;
XPLMCommandRef cmdPilotViewOn;
XPLMCommandRef cmdPilotViewOff;
XPLMCommandRef cmdPilotViewToggle;
XPLMCommandRef cmdLookAroundViewOn;
XPLMCommandRef cmdLookAroundViewOff;
XPLMCommandRef cmdLookAroundViewToggle;

XPLMCommandRef cmdScenicTiltInc;
XPLMCommandRef cmdScenicTiltDec;
XPLMCommandRef cmdScenicTiltReset;
XPLMCommandRef cmdPilotHeightInc;
XPLMCommandRef cmdPilotHeightDec;
XPLMCommandRef cmdPilotHeightReset;

//
// Settings
//
float scenic_height = DEFAULT_SCENIC_HEIGHT;
float scenic_tilt   = DEFAULT_SCENIC_TILT;
float pilot_height  = DEFAULT_PILOT_HEIGHT;
float pilot_tilt    = DEFAULT_PILOT_TILT;

int tracking   = 0;
int pilotView  = 0;
int lookAround = 0;

float ReadScenicHeight()
{
    return scenic_height;
}

void WriteScenicHeight(float height)
{
    height = min(100.0f, max(0.0f, height));
    scenic_height = height;
    if (pilotView == 0) {
        gKmlClient->set_view(scenic_height, scenic_tilt);
    }
}

float ReadScenicTilt()
{
    return scenic_tilt;
}

void WriteScenicTilt(float tilt)
{
    tilt = min(90.0f, max(0.0f, tilt));
    scenic_tilt = tilt;
    if (pilotView == 0) {
        gKmlClient->set_view(scenic_height, scenic_tilt);
    }
}

float ReadPilotHeight()
{
    return pilot_height;
}

void WritePilotHeight(float height)
{
    height = min(100.0f, max(0.0f, height));
    pilot_height = height;
    if (pilotView != 0) {
        gKmlClient->set_view(pilot_height, pilot_tilt);
    }
}

float ReadPilotTilt()
{
    return pilot_tilt;
}

void WritePilotTilt(float tilt)
{
    tilt = min(90.0f, max(0.0f, tilt));
    pilot_tilt = tilt;
    if (pilotView != 0) {
        gKmlClient->set_view(pilot_height, pilot_tilt);
    }
}



int GetTracking() {
    return tracking;
}

void SetTracking(int track) {
    tracking = track;
    if (tracking != 0) {
        XPLMCheckMenuItem(menu_id, track_menu_index, xplm_Menu_Checked);
        gKmlClient->start_tracking();
    }
    else {
        XPLMCheckMenuItem(menu_id, track_menu_index, xplm_Menu_Unchecked);
        gKmlClient->stop_tracking();
    }
}


int GetPilotView() {
    return pilotView;
}

void SetPilotView(int view) {
    pilotView = view;
    if (view != 0) {
        XPLMCheckMenuItem(menu_id, pilot_view_menu_index, xplm_Menu_Checked);
        gKmlClient->set_view(pilot_height, pilot_tilt);
    }
    else {
        XPLMCheckMenuItem(menu_id, pilot_view_menu_index, xplm_Menu_Unchecked);
        gKmlClient->set_view(scenic_height, scenic_tilt);
    }
}


int GetLookAroundView() {
    return lookAround;
}

void SetLookAroundView(int look) {
    lookAround = look;
    if (lookAround != 0) {
        XPLMCheckMenuItem(menu_id, look_around_menu_index, xplm_Menu_Checked);
        gKmlClient->set_look_around_on();
    }
    else {
        XPLMCheckMenuItem(menu_id, look_around_menu_index, xplm_Menu_Unchecked);
        gKmlClient->set_look_around_off();
    }
}

//
// Handlers
//
int handleReadTracking(void* inRefCon) {
    return GetTracking();
}

void handleWriteTracking(void* inRefCon, int track) {
    if (track != 0) {
        SetTracking(1);
    } else {
        SetTracking(0);
    }
}

int handleCmdTracking(XPLMCommandRef command, XPLMCommandPhase phase, void* refcon) {
    if (phase == xplm_CommandBegin) {
        if (command == cmdTrackingOn) {
            SetTracking(1);
        }
        else if (command == cmdTrackingOff) {
            SetTracking(0);
        }
        else if (command == cmdTrackingToggle) {
            int prevTracking = GetTracking();
            SetTracking(1 - prevTracking);
        }
    }
    return 1;  // Let X-Plane handle this command
}

int handleCmdConnectEarth(XPLMCommandRef command, XPLMCommandPhase phase, void* refcon) {
    if (phase == xplm_CommandBegin) {
        gKmlClient->connect_earth();
    }
    return 1;  // Let X-Plane handle this command
}

int handleReadPilotView(void* inRefCon) {
    return GetPilotView();
}

void handleWritePilotView(void* inRefCon, int view) {
    if (view != 0) {
        SetPilotView(1);
    } else {
        SetPilotView(0);
    }
}

int handleCmdPilotView(XPLMCommandRef command, XPLMCommandPhase phase, void* refcon) {
    if (phase == xplm_CommandBegin) {
        if (command == cmdPilotViewOn) {
            SetPilotView(1);
        }
        else if (command == cmdPilotViewOff) {
            SetPilotView(0);
        }
        else if (command == cmdPilotViewToggle) {
            int prevPilotView = GetPilotView();
            SetPilotView(1 - prevPilotView);
        }
    }
    return 1;  // Let X-Plane handle this command
}

int handleReadLookAroundView(void* inRefCon) {
    return GetLookAroundView();
}

void handleWriteLookAroundView(void* inRefCon, int view) {
    if (view != 0) {
        SetLookAroundView(1);
    } else {
        SetLookAroundView(0);
    }
}

int handleCmdLookAroundView(XPLMCommandRef command, XPLMCommandPhase phase, void* refcon) {
    if (phase == xplm_CommandBegin) {
        if (command == cmdLookAroundViewOn) {
            SetLookAroundView(1);
        }
        else if (command == cmdLookAroundViewOff) {
            SetLookAroundView(0);
        }
        else if (command == cmdLookAroundViewToggle) {
            int prevLookAroundView = GetLookAroundView();
            SetLookAroundView(1 - prevLookAroundView);
        }
    }
    return 1;  // Let X-Plane handle this command
}

int handleCmdScenicTilt(XPLMCommandRef command, XPLMCommandPhase phase, void* refcon) {
    if (phase == xplm_CommandBegin) {
        if (command == cmdScenicTiltInc) {
            WriteScenicTilt(ReadScenicTilt() + 5.0f);
        }
        else if (command == cmdScenicTiltDec) {
            WriteScenicTilt(ReadScenicTilt() - 5.0f);
        }
        else if (command == cmdScenicTiltReset) {
            WriteScenicTilt(DEFAULT_SCENIC_TILT);
        }
    }

    return 1;
}

float handleReadScenicTilt(void* inRefCon)
{
    return ReadScenicTilt();
}

void handleWriteScenicTilt(void* inRefCon, float tilt)
{
    return WriteScenicTilt(tilt);
}

int handleCmdPilotHeight(XPLMCommandRef command, XPLMCommandPhase phase, void* refcon) {
    if (phase == xplm_CommandBegin) {
        if (command == cmdPilotHeightInc) {
            WritePilotHeight(ReadPilotHeight() + 1.0f);
        }
        else if (command == cmdPilotHeightDec) {
            WritePilotHeight(ReadPilotHeight() - 1.0f);
        }
        else if (command == cmdPilotHeightReset) {
            WritePilotHeight(DEFAULT_PILOT_HEIGHT);
        }
    }

    return 1;
}

float handleReadPilotHeight(void* inRefCon)
{
    return ReadPilotHeight();
}

void handleWritePilotHeight(void* inRefCon, float height)
{
    return WritePilotHeight(height);
}


PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc) {
    strcpy(outName, "XP2Earth");
    strcpy(outSig, "nl.evg.xp2earth");
    strcpy(outDesc, "Google earth view.");

    int menuitem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "XP2Earth", NULL, 1);

    menu_id = XPLMCreateMenu("XP2Earth", XPLMFindPluginsMenu(), menuitem, PluginsMenuHandler, NULL);

    track_menu_index = XPLMAppendMenuItem(menu_id, "Tracking", (void *)"TRACK",1);
    XPLMCheckMenuItem(menu_id, track_menu_index, xplm_Menu_Unchecked);
    XPLMAppendMenuItem(menu_id, "Connect Earth", (void*)"CONNECT_EARTH", 1);
    XPLMAppendMenuSeparator(menu_id);
    pilot_view_menu_index = XPLMAppendMenuItem(menu_id, "Pilot View", (void*)"PILOT_VIEW", 1);
    XPLMCheckMenuItem(menu_id, pilot_view_menu_index, xplm_Menu_Unchecked);
    look_around_menu_index = XPLMAppendMenuItem(menu_id, "Look Around", (void*)"LOOK_AROUND", 1);
    XPLMCheckMenuItem(menu_id, look_around_menu_index, xplm_Menu_Unchecked);

    //
    // Publish commands and datarefs
    //
    // Tracking
    cmdTrackingOn     = XPLMCreateCommand("xp2earth/tracking_on", "Set tracking on");
    cmdTrackingOff    = XPLMCreateCommand("xp2earth/tracking_off", "Set tracking off");
    cmdTrackingToggle = XPLMCreateCommand("xp2earth/tracking_toggle", "Toggle tracking");
    XPLMRegisterCommandHandler(cmdTrackingOn, handleCmdTracking, 0, NULL);
    XPLMRegisterCommandHandler(cmdTrackingOff, handleCmdTracking, 0, NULL);
    XPLMRegisterCommandHandler(cmdTrackingToggle, handleCmdTracking, 0, NULL);

    XPLMDataRef drTracking = XPLMRegisterDataAccessor("xp2earth/tracking", xplmType_Int, 1, handleReadTracking, handleWriteTracking,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    // Connect Earth
    cmdConnectEarth = XPLMCreateCommand("xp2earth/connect_earth", "Connect to Google Earth");
    XPLMRegisterCommandHandler(cmdConnectEarth, handleCmdConnectEarth, 0, NULL);

    // Pilot view
    cmdPilotViewOn     = XPLMCreateCommand("xp2earth/view/pilot", "Set view to pilot perspective");
    cmdPilotViewOff    = XPLMCreateCommand("xp2earth/view/scenic", "Set view to scenic perspective");
    cmdPilotViewToggle = XPLMCreateCommand("xp2earth/view/toggle", "Toggle view between pilot and scenic perspective");
    XPLMRegisterCommandHandler(cmdPilotViewOn, handleCmdPilotView, 0, NULL);
    XPLMRegisterCommandHandler(cmdPilotViewOff, handleCmdPilotView, 0, NULL);
    XPLMRegisterCommandHandler(cmdPilotViewToggle, handleCmdPilotView, 0, NULL);

    XPLMDataRef drPilotView = XPLMRegisterDataAccessor("xp2earth/view", xplmType_Int, 1, handleReadPilotView, handleWritePilotView,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    // Look around
    cmdLookAroundViewOn     = XPLMCreateCommand("xp2earth/lookaround/view_on", "Viewpoint moves with head movement");
    cmdLookAroundViewOff    = XPLMCreateCommand("xp2earth/lookaround/view_off", "Viewpoint remains fixed");
    cmdLookAroundViewToggle = XPLMCreateCommand("xp2earth/lookaround/view_toggle", "Toggle between moving and fixed viewpoint");
    XPLMRegisterCommandHandler(cmdLookAroundViewOn, handleCmdLookAroundView, 0, NULL);
    XPLMRegisterCommandHandler(cmdLookAroundViewOff, handleCmdLookAroundView, 0, NULL);
    XPLMRegisterCommandHandler(cmdLookAroundViewToggle, handleCmdLookAroundView, 0, NULL);

    XPLMDataRef drLookAroundView = XPLMRegisterDataAccessor("xp2earth/lookaround", xplmType_Int, 1, handleReadLookAroundView, handleWriteLookAroundView,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    cmdScenicTiltInc = XPLMCreateCommand("xp2earth/scenic/tilt_inc", "Increase scenic tilt angle by 5 degrees");
    cmdScenicTiltDec = XPLMCreateCommand("xp2earth/scenic/tilt_dec", "Decrease scenic tilt angle by 5 degrees");
    cmdScenicTiltReset = XPLMCreateCommand("xp2earth/scenic/tilt_reset", "Reset scenic tilt angle to default setting");
    XPLMRegisterCommandHandler(cmdScenicTiltInc, handleCmdScenicTilt, 0, NULL);
    XPLMRegisterCommandHandler(cmdScenicTiltDec, handleCmdScenicTilt, 0, NULL);
    XPLMRegisterCommandHandler(cmdScenicTiltReset, handleCmdScenicTilt, 0, NULL);

    XPLMDataRef drScenicTilt = XPLMRegisterDataAccessor("xp2earth/scenic/tilt", xplmType_Float, 1, NULL, NULL, handleReadScenicTilt, handleWriteScenicTilt,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    cmdPilotHeightInc   = XPLMCreateCommand("xp2earth/pilot/height_inc", "Increase pilot height by 1 meter");
    cmdPilotHeightDec   = XPLMCreateCommand("xp2earth/pilot/height_dec", "Decrease pilot height by 1 meter");
    cmdPilotHeightReset = XPLMCreateCommand("xp2earth/pilot/height_reset", "Reset pilot height to default setting");
    XPLMRegisterCommandHandler(cmdPilotHeightInc, handleCmdPilotHeight, 0, NULL);
    XPLMRegisterCommandHandler(cmdPilotHeightDec, handleCmdPilotHeight, 0, NULL);
    XPLMRegisterCommandHandler(cmdPilotHeightReset, handleCmdPilotHeight, 0, NULL);

    XPLMDataRef drPilotHeight = XPLMRegisterDataAccessor("xp2earth/pilot/height", xplmType_Float, 1, NULL, NULL, handleReadPilotHeight, handleWritePilotHeight,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    return 1;
}

PLUGIN_API void	XPluginStop(void) {
}

PLUGIN_API void XPluginDisable(void) {
    if (gKmlClient) {
        delete gKmlClient;
    }
    gKmlClient = NULL;
}

PLUGIN_API int XPluginEnable(void) {

    if (gKmlClient) {
        delete gKmlClient;
    }
    gKmlClient = new KmlClient();

    if (gKmlClient->isConnected()) {
        XPLMEnableMenuItem(menu_id, track_menu_index, 1); // enabled
    } else {
        XPLMEnableMenuItem(menu_id, track_menu_index, 0); // disabled
    }

    if (GetPilotView() == 0) {
        gKmlClient->set_view(scenic_height, scenic_tilt);
    } else {
        gKmlClient->set_view(pilot_height, pilot_tilt);
    }
    if (GetLookAroundView() == 0) {
        gKmlClient->set_look_around_off();
    }
    else {
        gKmlClient->set_look_around_on();
    }

    return 1;
}

int main(int argc, char *argv[]) {
    return 0;
}

void PluginsMenuHandler(void * mRef, void * iRef)
{

    if (!strcmp((char *) iRef, "TRACK")) {
        XPLMMenuCheck checkstate;
        XPLMCheckMenuItemState(menu_id, track_menu_index, &checkstate);
        if (checkstate == xplm_Menu_Checked) {
            SetTracking(0);
        } else {
            SetTracking(1);
        }


    }

    if (!strcmp((char*)iRef, "CONNECT_EARTH")) {
        gKmlClient->connect_earth();
    }

    if (!strcmp((char*)iRef, "PILOT_VIEW")) {
        XPLMMenuCheck checkstate;
        XPLMCheckMenuItemState(menu_id, pilot_view_menu_index, &checkstate);
        if (checkstate == xplm_Menu_Checked) {
            SetPilotView(0);
        }
        else {
            SetPilotView(1);
        }
    }

    if (!strcmp((char*)iRef, "LOOK_AROUND")) {
        XPLMMenuCheck checkstate;
        XPLMCheckMenuItemState(menu_id, look_around_menu_index, &checkstate);
        if (checkstate == xplm_Menu_Checked) {
            SetLookAroundView(0);
        }
        else {
            SetLookAroundView(1);
        }
    }

}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam) {

}
