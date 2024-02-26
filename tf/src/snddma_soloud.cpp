#include <IMemory.h>
#include <ILog.h>
#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

extern SoLoud::Soloud gSoloud;
static SoLoud::WavStream stream;
static SoLoud::Bus streamBus;
static uint32_t handle = -1;

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
        LOGF(LogLevel::eINFO, "DMA Audio Init");
        if (s_khz->value == 44)
            dma.speed = 44100;
        if (s_khz->value == 22)
            dma.speed = 22050;
        else
            dma.speed = 11025;

        queue.setParams(dma.speed);

        int rambuffer = BUFFER_SIZE;
        dma.buffer = (byte *)tf_malloc(BUFFER_SIZE);
        dma.channels = 2;
        dma.samplebits = 16;
        dma.samplepos = 0;
        dma.samples = BUFFER_SIZE / (dma.channels * dma.samplebits / 8);
        dma.submission_chunk = 16;

        return true;
    }

    int SNDDMA_GetDMAPos(void)
    {
        LOGF(LogLevel::eINFO, "DMA Audio: Get Position. Queue count: %d", queue.getQueueCount());
        if (queue.getQueueCount() > 2)
        {
            return dma.samples;
        }

        return 0;
    }

    void SNDDMA_Shutdown(void)
    {
        LOGF(LogLevel::eINFO, "DMA Audio: Shutdown");
        queue.stop();
        tf_free(dma.buffer);

        gSoloud.stop(handle);
    }

    void SNDDMA_BeginPainting(void)
    {
        LOGF(LogLevel::eINFO, "DMA Audio: Begin Painting");
    }

    void SNDDMA_Submit(void)
    {
        LOGF(LogLevel::eINFO, "DMA Audio: Submit");
        wav.loadRawWave16((short *)dma.buffer, dma.samples, dma.speed, dma.channels);
        
        if(queue.getQueueCount() == 0) {
            queue.play(wav);
            handle = gSoloud.play(queue);
        } else {
            queue.play(wav);
        }
    }
}