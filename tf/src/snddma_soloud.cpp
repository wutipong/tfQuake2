#include <cstdint>
#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

extern SoLoud::Soloud gSoloud;
static SoLoud::WavStream stream;
static SoLoud::Bus streamBus;
static uint32_t handle;

static SoLoud::Queue queue;

static constexpr size_t BUFFER_SIZE = 2048;

static SoLoud::Wav wav;
extern "C"
{
#include "../../client/client.h"
#include "../../client/snd_loc.h"

    extern dma_t dma;

    qboolean SNDDMA_Init(void)
    {
		gSoloud.play(queue);

        return true;
    }

    int SNDDMA_GetDMAPos(void)
    {
        if (queue.getQueueCount() > 2)
        {
            return dma.samples;
        }

        return 0;
    }

    void SNDDMA_Shutdown(void)
    {
        queue.stop();
    }

    void SNDDMA_BeginPainting(void)
    {
		wav.loadMem(dma.buffer, dma.samples * dma.channels * dma.samplebits /8, true, true);
    }

    void SNDDMA_Submit(void)
    {
		queue.play(wav);
    }
}