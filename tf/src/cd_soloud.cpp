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

    qboolean CDAudio_Play(int track, qboolean looping)
    {
        gSoloud.stop(handle);

        std::string path = std::format("{}/music/track{:02}.ogg", FS_Gamedir(), track);
        LOGF(LogLevel::eINFO, "Playing music file %s", path.c_str());

        stream.load(path.c_str());
        handle = gSoloud.playBackground(stream);
        gSoloud.setLooping(handle, looping);

        return false;
    }

    void CDAudio_Stop(void)
    {
        LOGF(LogLevel::eINFO, "Stop music");
        if(handle == -1) return;
        gSoloud.setPause(handle, true);
    }

    void CDAudio_Resume(void)
    {
        LOGF(LogLevel::eINFO, "Resume music");
        gSoloud.setPause(handle, false);
    }

    void CDAudio_Update(void)
    {
    }

    int CDAudio_Init(void)
    {
        LOGF(LogLevel::eINFO, "Init music");
        
        return 0;
    }

    void CDAudio_Shutdown(void)
    {
        LOGF(LogLevel::eINFO, "Shutdown music");
    }
}
