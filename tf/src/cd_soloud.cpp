#include <ILog.h>
#include <format>
#include <soloud.h>
#include <soloud_wavstream.h>
#include <string>

extern SoLoud::Soloud gSoloud;
static SoLoud::WavStream stream;
static SoLoud::Bus streamBus;
static uint32_t handle;

extern "C"
{
#include "../../client/client.h"

    qboolean CDAudio_Play(int track, qboolean looping)
    {
        streamBus.stop();

        std::string path = std::format("{}/music/track{:02}.ogg", FS_Gamedir(), track);
        LOGF(LogLevel::eINFO, "Playing music file %s", path.c_str());

        stream.load(path.c_str());
        handle = streamBus.play(stream);

        return false;
    }

    void CDAudio_Stop(void)
    {
        LOGF(LogLevel::eINFO, "Stop music");
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
        gSoloud.play(streamBus);
        return 0;
    }

    void CDAudio_Shutdown(void)
    {
        streamBus.stop();
    }
}
