
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



KmlClient *gKmlClient = NULL;
static void PluginsMenuHandler(void * mRef, void * iRef);

XPLMMenuID menu_id;
int track_menu_index;
int pilot_view_menu_index;
int look_around_menu_index;

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
            XPLMCheckMenuItem(menu_id, track_menu_index, xplm_Menu_Unchecked);
            gKmlClient->stop_tracking();
        } else {
            XPLMCheckMenuItem(menu_id, track_menu_index, xplm_Menu_Checked);
            gKmlClient->start_tracking();
        }


    }

    if (!strcmp((char*)iRef, "CONNECT_EARTH")) {
        gKmlClient->connect_earth();
    }

    if (!strcmp((char*)iRef, "PILOT_VIEW")) {
        XPLMMenuCheck checkstate;
        XPLMCheckMenuItemState(menu_id, pilot_view_menu_index, &checkstate);
        if (checkstate == xplm_Menu_Checked) {
            XPLMCheckMenuItem(menu_id, pilot_view_menu_index, xplm_Menu_Unchecked);
            gKmlClient->set_pilot_view_off();
        }
        else {
            XPLMCheckMenuItem(menu_id, pilot_view_menu_index, xplm_Menu_Checked);
            gKmlClient->set_pilot_view_on();
        }
    }

    if (!strcmp((char*)iRef, "LOOK_AROUND")) {
        XPLMMenuCheck checkstate;
        XPLMCheckMenuItemState(menu_id, look_around_menu_index, &checkstate);
        if (checkstate == xplm_Menu_Checked) {
            XPLMCheckMenuItem(menu_id, look_around_menu_index, xplm_Menu_Unchecked);
            gKmlClient->set_look_around_off();
        }
        else {
            XPLMCheckMenuItem(menu_id, look_around_menu_index, xplm_Menu_Checked);
            gKmlClient->set_look_around_on();
        }
    }

}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam) {

}
