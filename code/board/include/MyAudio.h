#ifndef MY_AUDIO_H
#define MY_AUDIO_H

#include "stm32l475e_iot01_audio.h"
#include "mbed.h"
#include "dest.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>

class MyAudio{
public:
    MyAudio(Mutex &mutex, ConditionVariable &cond, EventQueue &event_queue, char* buffer, char &mode);
    ~MyAudio();

    void target_audio_buffer_full();
    void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance);
    void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance);
    void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance);
    void print_stats();
    void start_recording();
    void record();
    void stop_recording();

    void setup();
    void start();
public:
    bool isRecording = false;

private:
    EventQueue *_event_queue = nullptr;
    Mutex *_mutex = nullptr;
    ConditionVariable *_cond = nullptr;

    char *_buffer;
    char *_mode;

    Thread _thread;

    uint16_t PCM_Buffer[PCM_BUFFER_LEN / 2];
    BSP_AUDIO_Init_t MicParams;
    size_t TARGET_AUDIO_BUFFER_NB_SAMPLES = AUDIO_SAMPLING_FREQUENCY/4;   // AUDIO_SAMPLING_FREQUENCY * m -> Record m seconds
    size_t SIZE_OF_PARTITION = 250;
    int16_t *TARGET_AUDIO_BUFFER = (int16_t*)calloc(TARGET_AUDIO_BUFFER_NB_SAMPLES, sizeof(int16_t));
    size_t TARGET_AUDIO_BUFFER_IX = 0;
    // int16_t *TEMP_TARGET_AUDIO_BUFFER = (int16_t*)calloc(TARGET_AUDIO_BUFFER_NB_SAMPLES, sizeof(int16_t)); //lowpass-filtered output buffer

    // we skip the first 50 events (100 ms.) to not record the button click
    // size_t SKIP_FIRST_EVENTS = 50;
    size_t SKIP_FIRST_EVENTS = 0;
    size_t half_transfer_events = 0;
    size_t transfer_complete_events = 0;
    double cutoff_alpha = 0.01;

    short sample_rate_shift = 2;
    size_t wavFreq = AUDIO_SAMPLING_FREQUENCY/sample_rate_shift;
    size_t sbcHeader = (AUDIO_SAMPLING_FREQUENCY/sample_rate_shift)*2;
    size_t dataSize = 0;    // (TARGET_AUDIO_BUFFER_NB_SAMPLES * 2)
    size_t fileSize = 44;   // 44 + (TARGET_AUDIO_BUFFER_NB_SAMPLES * 2)

    // unsigned wav_header[44] = {
    //     0x52, 0x49, 0x46, 0x46, // RIFF
    //     fileSize & 0xff, (fileSize >> 8) & 0xff, (fileSize >> 16) & 0xff, (fileSize >> 24) & 0xff,
    //     0x57, 0x41, 0x56, 0x45, // WAVE
    //     0x66, 0x6d, 0x74, 0x20, // fmt
    //     0x10, 0x00, 0x00, 0x00, // length of format data
    //     0x01, 0x00, // type of format (1=PCM)
    //     0x01, 0x00, // number of channels
    //     wavFreq & 0xff, (wavFreq >> 8) & 0xff, (wavFreq >> 16) & 0xff, (wavFreq >> 24) & 0xff,
    //     0x00, 0x7d, 0x00, 0x00, // 	(Sample Rate * BitsPerSample * Channels) / 8    (16000)0xd700 -> (4000)0x35c0
    //     0x02, 0x00, 0x10, 0x00,
    //     0x64, 0x61, 0x74, 0x61, // data
    //     dataSize & 0xff, (dataSize >> 8) & 0xff, (dataSize >> 16) & 0xff, (dataSize >> 24) & 0xff,
    // };
};

#endif