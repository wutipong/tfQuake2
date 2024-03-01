#include <ILog.h>
#include <format>
#include <limits>
#include <map>
#include <memory>
#include <soloud.h>
#include <soloud_bus.h>
#include <soloud_wav.h>
#include <string>
#include <vector>

extern SoLoud::Soloud gSoloud;
static SoLoud::Bus sfxBus;
static int busHandle;

static SoLoud::Wav rawWav;
static int rawHandle = -1;

template <class T> static std::vector<float> createWavData(T *src, size_t length)
{
    std::vector<float> output{};
    output.resize(length);

    constexpr auto max = static_cast<float>(std::numeric_limits<T>::max());
    for (int i = 0; i < length; i++)
    {
        auto sample = static_cast<float>(src[i]);
        output[i] = sample / max;
    }

    return output;
}

template <class T> static std::vector<float> createStereoWavData(T *src, size_t length)
{
    std::vector<float> output{};
    output.resize(length * 2);

    const size_t half = length;
    constexpr auto max = static_cast<float>(std::numeric_limits<T>::max());
    for (int i = 0; i < length * 2; i++)
    {
        auto outIndex = i / 2 + ((i % 2 == 0) ? 0 : half);
        auto sample = static_cast<float>(src[i]);

        output[outIndex] = sample / max;
    }

    return output;
}
struct sfx_s;

static std::map<std::string, sfx_s *> sfxMap{};
static std::map<std::string, SoLoud::Wav> wavMap;

extern "C"
{

#include "../../client/client.h"
#include "../../client/snd_loc.h"
#include "../../client/sound.h"

    void S_Init(void)
    {
        sfxBus.setVolume(1);
        busHandle = gSoloud.play(sfxBus);
    }
    void S_Shutdown(void)
    {
        gSoloud.stop(busHandle);
    }

    // if origin is NULL, the sound will be dynamically sourced from the entity
    void S_StartSound(vec3_t origin, int entnum, int entchannel, struct sfx_s *sfx, float fvol, float attenuation,
                      float timeofs)
    {
        vec3_t pos{};
        if (origin == NULL)
        {
            return;
        }

        if (!wavMap.contains(sfx->name))
        {
            return;
        }
        auto &wav = wavMap[sfx->name];

        busHandle = gSoloud.play(sfxBus);
        auto h = sfxBus.play3d(wav, origin[0], origin[1], origin[2]);

        gSoloud.setVolume(h, fvol);
    }

    void S_StartLocalSound(char *s)
    {
        sfx_t *sfx = S_FindName(s, true);
        if (!sfx)
        {
            LOGF(LogLevel::eERROR, "S_StartLocalSound: can't cache %s\n", s);
            return;
        }
        S_StartSound(NULL, cl.playernum + 1, 0, sfx, 1, 1, 0);
    }

    void S_RawSamples(int samples, int rate, int width, int channels, byte *data)
    {
        if (rawHandle != -1)
        {
            gSoloud.stop(rawHandle);
            rawWav = SoLoud::Wav();
        }

        std::vector<float> raw;
        if (width == 1)
        {
            if (channels == 1)
            {
                raw = createWavData(data, samples);
            }
            else if (channels == 2)
            {
                raw = createStereoWavData(data, samples);
            }
        }
        else if (width == 2)
        {
            if (channels == 1)
            {
                raw = createWavData((short *)data, samples);
            }
            else if (channels == 2)
            {
                raw = createStereoWavData((short *)data, samples);
            }
        }
        if (raw.empty())
        {
            return;
        }
        auto res = rawWav.loadRawWave(raw.data(), raw.size(), rate, channels, true);
        rawHandle = sfxBus.play(rawWav);
        busHandle = gSoloud.play(sfxBus);
    }

    void S_StopAllSounds(void)
    {
        sfxBus.stop();
    }

    void S_Update(vec3_t origin, vec3_t v_forward, vec3_t v_right, vec3_t v_up)
    {
        gSoloud.set3dListenerPosition(origin[0], origin[1], origin[2]);
        gSoloud.set3dListenerUp(v_up[0], v_up[1], v_up[2]);
        gSoloud.set3dListenerAt(v_forward[0], v_forward[1], v_forward[2]);

        gSoloud.update3dAudio();
    }

    void S_Activate(qboolean active)
    {
        if (active)
        {
            busHandle = gSoloud.play(sfxBus);
        }
        else
        {
            gSoloud.stop(busHandle);
        }
    }

    void S_BeginRegistration(void)
    {
        for (auto &p : sfxMap)
        {
            delete p.second;
        }

        sfxMap.clear();
    }

    struct sfx_s *S_RegisterSound(char *sample)
    {
        LOGF(LogLevel::eINFO, "Register sound: %s", sample);

        return S_FindName(sample, true);
    }
    void S_EndRegistration(void)
    {
    }

    struct sfx_s *S_FindName(char *name, qboolean create)
    {
        if (sfxMap.contains(name))
        {
            return sfxMap[name];
        }

        if (!create)
        {
            return NULL;
        }

        sfx_t *p = sfxMap[name] = new sfx_t{};
        strcpy_s(p->name, name);

        S_LoadSound(p);

        return p;
    }

    sfxcache_t *S_LoadSound(sfx_t *s)
    {
        auto path = std::format("{}/sound/{}", FS_Gamedir(), s->name);

        auto &wav = wavMap[s->name] = SoLoud::Wav();
        wav.load(path.c_str());

        return NULL;
    }
}