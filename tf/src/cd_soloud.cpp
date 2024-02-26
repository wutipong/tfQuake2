#include <ILog.h>
#include <format>
#include <soloud.h>
#include <soloud_wavstream.h>
#include <string>

extern SoLoud::Soloud gSoloud;
static SoLoud::WavStream stream;
static uint32_t handle = -1;

extern "C"
{
#include "../../client/client.h"

    static cvar_t *cd_volume;
    static cvar_t *cd_loopcount;
    static cvar_t *cd_looptrack;
    static cvar_t *no_music;
    
    qboolean CDAudio_Play(int track, qboolean looping)
    {
        gSoloud.stop(handle);

        std::string path = std::format("{}/music/track{:02}.ogg", FS_Gamedir(), track);
        LOGF(LogLevel::eINFO, "Playing music file %s", path.c_str());

        stream.load(path.c_str());
        handle = gSoloud.playBackground(stream);
        gSoloud.setLooping(handle, looping);
        gSoloud.setVolume(handle, cd_volume->value);

        return false;
    }

    void CDAudio_Stop(void)
    {
        LOGF(LogLevel::eINFO, "Stop music");
        if (handle == -1)
            return;
        gSoloud.setPause(handle, true);
    }

    void CDAudio_Resume(void)
    {
        LOGF(LogLevel::eINFO, "Resume music");
        gSoloud.setPause(handle, false);
    }

    void CDAudio_Update(void)
    {
        if (cd_volume->modified)
        {
            cd_volume->modified = false;
            gSoloud.setVolume(handle, cd_volume->value);
        }
    }

    int CDAudio_Init(void)
    {
        LOGF(LogLevel::eINFO, "Init music");

        cd_volume = Cvar_Get(std::string("cd_volume").data(), std::string("1").data(), CVAR_ARCHIVE);
        cd_loopcount = Cvar_Get(std::string("cd_loopcount").data(), std::string("4").data(), 0);
        cd_looptrack = Cvar_Get(std::string("cd_looptrack").data(), std::string("11").data(), 0);
        no_music = Cvar_Get(std::string("no_music").data(), std::string("0").data(), 0);

        return 0;
    }

    void CDAudio_Shutdown(void)
    {
        LOGF(LogLevel::eINFO, "Shutdown music");
    }
}
