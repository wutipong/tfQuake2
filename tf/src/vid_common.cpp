#include <string>

void refreshSettings();

extern "C"
{
#include "../../client/client.h"
    extern cvar_t *vid_gamma;
    extern cvar_t *vid_ref;  // Name of Refresh DLL loaded
    extern cvar_t *vid_xpos; // X coordinate of window position
    extern cvar_t *vid_ypos; // Y coordinate of window position
    extern cvar_t *vid_fullscreen;
    extern cvar_t *vid_refresh;
    extern cvar_t *vid_hudscale;
    extern cvar_t *r_customwidth;
    extern cvar_t *r_customheight;
    extern cvar_t *viewsize;
    extern cvar_t *in_joystick;

    cvar_t *win_noalttab;

    qboolean reflib_active = false;

    static cvar_t *_Cvar_Get (std::string var_name, std::string value, int flags)
    {
        return Cvar_Get (var_name.data(), value.data(), flags);
    }

    void VID_Init(void)
    {
        /* Create the video variables so we know how to start the graphics drivers */
        vid_ref = _Cvar_Get("vid_ref", "vk", CVAR_ARCHIVE);
        vid_xpos = _Cvar_Get("vid_xpos", "3", CVAR_ARCHIVE);
        vid_ypos = _Cvar_Get("vid_ypos", "22", CVAR_ARCHIVE);
        vid_fullscreen = _Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
        vid_refresh = _Cvar_Get("vid_refresh", "0", CVAR_NOSET);
        vid_gamma = _Cvar_Get("vid_gamma", "1", CVAR_ARCHIVE);
        win_noalttab = _Cvar_Get("win_noalttab", "0", CVAR_ARCHIVE);
        r_customwidth = _Cvar_Get("r_customwidth", "1024", CVAR_ARCHIVE);
        r_customheight = _Cvar_Get("r_customheight", "768", CVAR_ARCHIVE);
        viewsize = _Cvar_Get("viewsize", "100", CVAR_ARCHIVE);

        /* Add some console commands that we want to handle */
        // Cmd_AddCommand ("vid_restart", VID_Restart_f);
        // Cmd_AddCommand ("vid_front", VID_Front_f);
    }

    void VID_Shutdown(void)
    {
        //if (reflib_active)
        {
            re.Shutdown();
            // VID_FreeReflib ();
        }
    }

    void VID_CheckChanges(void)
    {
        refreshSettings();

        /** This will be handled in the MainApp class */
        // char name[100];

        // if (win_noalttab->modified)
        // {
        //     if (win_noalttab->value)
        //     {
        //         WIN_DisableAltTab();
        //     }
        //     else
        //     {
        //         WIN_EnableAltTab();
        //     }
        //     win_noalttab->modified = false;
        // }

        // if (vid_ref->modified)
        // {
        //     cl.force_refdef = true; // can't use a paused refdef
        //     S_StopAllSounds();
        // }
        // while (vid_ref->modified)
        // {
        //     /*
        //     ** refresh has changed
        //     */
        //     vid_ref->modified = false;
        //     vid_fullscreen->modified = true;
        //     cl.refresh_prepped = false;
        //     cls.disable_screen = true;

        //     Com_sprintf(name, sizeof(name), "ref_%s.dll", vid_ref->string);
        //     if (!VID_LoadRefresh(name))
        //     {
        //         if (strcmp(vid_ref->string, "soft") == 0)
        //             Com_Error(ERR_FATAL, "Couldn't fall back to software refresh!");
        //         Cvar_Set("vid_ref", "soft");

        //         /*
        //         ** drop the console if we fail to load a refresh
        //         */
        //         if (cls.key_dest != key_console)
        //         {
        //             Con_ToggleConsole_f();
        //         }
        //     }
        //     cls.disable_screen = false;
        // }

        // /*
        // ** update our window position
        // */
        // if (vid_xpos->modified || vid_ypos->modified)
        // {
        //     if (!vid_fullscreen->value)
        //         VID_UpdateWindowPosAndSize(vid_xpos->value, vid_ypos->value);

        //     vid_xpos->modified = false;
        //     vid_ypos->modified = false;
        // }

        // if (vid_refresh->modified)
        // {
        //     vid_refresh->modified = false;
        //     cl.refresh_prepped = false;
        // }
    }
}