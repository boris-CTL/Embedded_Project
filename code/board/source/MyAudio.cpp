#include "MyAudio.h"
#include "dest.h"
#include "stm32l475e_iot01_audio.h"
#include <cstddef>
#include <cstdio>

MyAudio::MyAudio(Mutex &mutex, ConditionVariable &cond, EventQueue &event_queue, char* buffer, char &mode):
    _mutex(&mutex), _cond(&cond), _event_queue(&event_queue), _buffer(buffer), _mode(&mode){

}

MyAudio::~MyAudio(){

}

void MyAudio::target_audio_buffer_full(){
    // printf("target_audio_buffer_full\n");
    // pause audio stream
    // int32_t ret = BSP_AUDIO_IN_Pause(AUDIO_INSTANCE);
    // dataSize += (TARGET_AUDIO_BUFFER_NB_SAMPLES * 2);
    // fileSize += (TARGET_AUDIO_BUFFER_NB_SAMPLES * 2);
    dataSize += (TARGET_AUDIO_BUFFER_NB_SAMPLES * 4)/sample_rate_shift;
    fileSize += (TARGET_AUDIO_BUFFER_NB_SAMPLES * 4)/sample_rate_shift;
    int32_t ret = BSP_AUDIO_IN_Stop(AUDIO_INSTANCE);
    if (ret != BSP_ERROR_NONE) {
        printf("Error Audio Pause (%d)\n", ret);
    }
    else {
        printf("OK Audio Pause\n");
    }
    
    // printf("Total complete events: %u, index is %u\n", transfer_complete_events, TARGET_AUDIO_BUFFER_IX);

    // print both the WAV header and the audio buffer in HEX format to serial
    // you can use the script in `hex-to-buffer.js` to make a proper WAV file again
    // printf("WAV file before low-pass:\n");
    // _mutex->lock();
    // int len = sprintf(_buffer, "{\"type\":\"STM_HEADER\", ");
    // len += sprintf(_buffer+len, "\"data\":\"");
    // for (size_t ix = 0; ix < 44; ix++) {
    //     // printf("%02x", wav_header[ix]);
    //     len += sprintf(_buffer+len, "%02x", wav_header[ix]);
    // }
    // len += sprintf(_buffer+len, "\", \"dest\":\"%s\"}", DEST);
    // _cond->notify_all();
    // _mutex->unlock();


    uint8_t *buf = (uint8_t*)TARGET_AUDIO_BUFFER;

    int len;
    size_t ix = 0;
    size_t shift = 2*sample_rate_shift;
    size_t NB_PARTITION = (TARGET_AUDIO_BUFFER_NB_SAMPLES*2)/(SIZE_OF_PARTITION*2);
    for (size_t i=0; i<NB_PARTITION; i++){
        _mutex->lock();
        len = sprintf(_buffer, "{\"type\":\"STM_AUDIO\", ");
        len += sprintf(_buffer+len, "\"data\":\"");
        for (size_t j=0; j<SIZE_OF_PARTITION/2; j++) {
            len += sprintf(_buffer+len, "%02x", buf[ix]);
            len += sprintf(_buffer+len, "%02x", buf[ix+1]);
            ix+=shift;
        }
        len += sprintf(_buffer+len, "\", \"dest\":\"%s\"}", DEST);
        _cond->notify_all();
        _mutex->unlock();
        // ThisThread::sleep_for(2ms);
    }

    // _mutex->lock();
    // int len = sprintf(_buffer, "{\"type\":\"STM_AUDIO\", ");
    // len += sprintf(_buffer+len, "\"data\":\"");
    // for (size_t ix = 0; ix < TARGET_AUDIO_BUFFER_NB_SAMPLES * 2; ix+=4){
    //         len += sprintf(_buffer+len, "%02x%02x", buf[ix], buf[ix+1]);
    //         ix+=4;
    // }
    // len += sprintf(_buffer+len, "\", \"dest\":\"%s\"}", DEST);
    // _cond->notify_all();
    // _mutex->unlock();


    if(!isRecording){
        unsigned wav_header[44] = {
            0x52, 0x49, 0x46, 0x46, // RIFF
            fileSize & 0xff, (fileSize >> 8) & 0xff, (fileSize >> 16) & 0xff, (fileSize >> 24) & 0xff,
            0x57, 0x41, 0x56, 0x45, // WAVE
            0x66, 0x6d, 0x74, 0x20, // fmt
            0x10, 0x00, 0x00, 0x00, // length of format data
            0x01, 0x00, // type of format (1=PCM)
            0x01, 0x00, // number of channels
            wavFreq & 0xff, (wavFreq >> 8) & 0xff, (wavFreq >> 16) & 0xff, (wavFreq >> 24) & 0xff,
            sbcHeader & 0xff, (sbcHeader >> 8) & 0xff, (sbcHeader >> 16) & 0xff, (sbcHeader >> 24) & 0xff, // (Sample Rate * BitsPerSample * Channels) / 8
            0x02, 0x00, 0x10, 0x00,
            0x64, 0x61, 0x74, 0x61, // data
            dataSize & 0xff, (dataSize >> 8) & 0xff, (dataSize >> 16) & 0xff, (dataSize >> 24) & 0xff,
        };
        _mutex->lock();
        len = sprintf(_buffer, "{\"type\":\"STM_HEADER\", ");
        len += sprintf(_buffer+len, "\"data\":\"");
        for (size_t ix = 0; ix < 44; ix++) {
            len += sprintf(_buffer+len, "%02x", wav_header[ix]);
        }
        len += sprintf(_buffer+len, "\", \"dest\":\"%s\"}", DEST);
        _cond->notify_all();
        _mutex->unlock();
    }

    // END signal
    // _mutex->lock();
    // len = sprintf(_buffer, "{\"type\":\"STM_END\", \"dest\":\"%s\"}", DEST);
    // _cond->notify_all();
    // _mutex->unlock();
    
    // for (size_t ix = 0; ix < TARGET_AUDIO_BUFFER_NB_SAMPLES * 2; ix++) {
    //     printf("%02x", buf[ix]);
    // }

    // test for low-pass filter
    // printf("WAV file after low-pass:\n");
    // for (size_t ix = 0; ix < 44; ix++) {
    //     printf("%02x", wav_header[ix]);
    // }

    // int16_t *buf_16t = TARGET_AUDIO_BUFFER;
    // int16_t *buff_temp = TEMP_TARGET_AUDIO_BUFFER;
    // double A = 1/(1+cutoff_alpha);
    // double B = (cutoff_alpha)/(1+cutoff_alpha); 

    // //compute the low-pass filtered output
    // for (int16_t ix = 0; ix < TARGET_AUDIO_BUFFER_NB_SAMPLES; ix++) {
    //     if (ix > 0) {
    //         double tmp_output = buff_temp[ix-1]*A+buf_16t[ix]*B;
    //         buff_temp[ix] = (int16_t)tmp_output;
    //     }
    //     else {
    //         buff_temp[ix] = 0;//y[0] = 0
    //     }
    // }

    // uint8_t *buf8t = (uint8_t*)TEMP_TARGET_AUDIO_BUFFER;
    // for (size_t ix = 0; ix < TARGET_AUDIO_BUFFER_NB_SAMPLES * 2; ix++) {
    //     printf("%02x", buf8t[ix]);
    // }
    // printf("Done filtering\n");

    // if(!isRecording){
    //     _mutex->lock();
    //     len = sprintf(_buffer, "{\"type\":\"STM_END\", \"dest\":\"%s\"}", DEST);
    //     _cond->notify_all();
    //     _mutex->unlock();
    //     isRecording = false;
    // }
    // else{
    // if(isRecording){
    //     // start_recording();
    //     // _event_queue->call(callback(this, &MyAudio::start_recording));
    //     int32_t ret = BSP_ERROR_NO_INIT;
    //     uint32_t state;

    //     while (ret!=BSP_ERROR_NONE){
    //         ret = BSP_AUDIO_IN_GetState(AUDIO_INSTANCE, &state);
    //     }
    //     // reset audio buffer location
    //     TARGET_AUDIO_BUFFER_IX = 0;
    //     transfer_complete_events = 0;
    //     half_transfer_events = 0;

    //     ret = BSP_ERROR_NO_INIT;
    //     while (ret!=BSP_ERROR_NONE){
    //         ret = BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, PCM_BUFFER_LEN);
    //         if (ret != BSP_ERROR_NONE) {
    //             printf("Error Audio Record (%d)\n", ret);
    //         }
    //         else {
    //             printf("OK Audio Record\n");
    //         }
    //     }
        

        // TARGET_AUDIO_BUFFER_IX = 0;
        // transfer_complete_events = 0;
        // half_transfer_events = 0;

        // BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, PCM_BUFFER_LEN);
    // }
    
}

void MyAudio::BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance){
    // _event_queue->call(callback(this, &MyAudio::print_stats));
    half_transfer_events++;
    if (half_transfer_events < SKIP_FIRST_EVENTS) return;

    uint32_t buffer_size = PCM_BUFFER_LEN / 2; /* Half Transfer */
    uint32_t nb_samples = buffer_size / sizeof(int16_t); /* Bytes to Length */

    if ((TARGET_AUDIO_BUFFER_IX + nb_samples) > TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        return;
    }

    /* Copy first half of PCM_Buffer from Microphones onto Fill_Buffer */
    memcpy(((uint8_t*)TARGET_AUDIO_BUFFER) + (TARGET_AUDIO_BUFFER_IX * 2), PCM_Buffer, buffer_size);
    TARGET_AUDIO_BUFFER_IX += nb_samples;

    if (TARGET_AUDIO_BUFFER_IX >= TARGET_AUDIO_BUFFER_NB_SAMPLES && isRecording) {
        _event_queue->call(callback(this, &MyAudio::print_stats));
        _event_queue->call(callback(this, &MyAudio::target_audio_buffer_full));
        return;
    }
}

void MyAudio::BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance){
    // _event_queue->call(callback(this, &MyAudio::print_stats));
    transfer_complete_events++;
    if (transfer_complete_events < SKIP_FIRST_EVENTS) return;

    uint32_t buffer_size = PCM_BUFFER_LEN / 2; /* Half Transfer */
    uint32_t nb_samples = buffer_size / sizeof(int16_t); /* Bytes to Length */

    if ((TARGET_AUDIO_BUFFER_IX + nb_samples) > TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        return;
    }

    /* Copy second half of PCM_Buffer from Microphones onto Fill_Buffer */
    memcpy(((uint8_t*)TARGET_AUDIO_BUFFER) + (TARGET_AUDIO_BUFFER_IX * 2),
        ((uint8_t*)PCM_Buffer) + (nb_samples * 2), buffer_size);
    TARGET_AUDIO_BUFFER_IX += nb_samples;

    if (TARGET_AUDIO_BUFFER_IX >= TARGET_AUDIO_BUFFER_NB_SAMPLES && isRecording) {
        _event_queue->call(callback(this, &MyAudio::print_stats));
        _event_queue->call(callback(this, &MyAudio::target_audio_buffer_full));
        return;
    }
}

void MyAudio::BSP_AUDIO_IN_Error_CallBack(uint32_t Instance){
    printf("BSP_AUDIO_IN_Error_CallBack\n");
}

void MyAudio::print_stats(){
    // printf("Half %u, Complete %u, IX %u\n", half_transfer_events, transfer_complete_events, TARGET_AUDIO_BUFFER_IX);
}

void MyAudio::start_recording(){
    dataSize = 0;
    fileSize = 44;
    // _mutex->lock();
    // int len = sprintf(_buffer, "{\"type\":\"STM_HEADER\", ");
    // len += sprintf(_buffer+len, "\"data\":\"");
    // for (size_t ix = 0; ix < 44; ix++) {
    //     // printf("%02x", wav_header[ix]);
    //     len += sprintf(_buffer+len, "%02x", wav_header[ix]);
    // }
    // len += sprintf(_buffer+len, "\", \"dest\":\"%s\"}", DEST);
    // _cond->notify_all();
    // _mutex->unlock();

    isRecording = true;
}
void MyAudio::record(){
    int32_t ret;
    uint32_t state;

    ret = BSP_AUDIO_IN_GetState(AUDIO_INSTANCE, &state);
    if (ret != BSP_ERROR_NONE) {
        // printf("Cannot start recording: Error getting audio state (%d)\n", ret);
        return;
    }
    if (state == AUDIO_IN_STATE_RECORDING) {
        // printf("Cannot start recording: Already recording\n");
        return;
    }

    // reset audio buffer location
    TARGET_AUDIO_BUFFER_IX = 0;
    transfer_complete_events = 0;
    half_transfer_events = 0;

    ret = BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, PCM_BUFFER_LEN);
    if (ret != BSP_ERROR_NONE) {
        // printf("Error Audio Record (%d)\n", ret);
        return;
    }
    else {
        // printf("OK Audio Record\n");
    }
}

void MyAudio::stop_recording(){

    isRecording = false;
    // ThisThread::sleep_for(300ms);
    // int len;
    // unsigned wav_header[44] = {
    //     0x52, 0x49, 0x46, 0x46, // RIFF
    //     fileSize & 0xff, (fileSize >> 8) & 0xff, (fileSize >> 16) & 0xff, (fileSize >> 24) & 0xff,
    //     0x57, 0x41, 0x56, 0x45, // WAVE
    //     0x66, 0x6d, 0x74, 0x20, // fmt
    //     0x10, 0x00, 0x00, 0x00, // length of format data
    //     0x01, 0x00, // type of format (1=PCM)
    //     0x01, 0x00, // number of channels
    //     wavFreq & 0xff, (wavFreq >> 8) & 0xff, (wavFreq >> 16) & 0xff, (wavFreq >> 24) & 0xff,
    //     sbcHeader & 0xff, (sbcHeader >> 8) & 0xff, (sbcHeader >> 16) & 0xff, (sbcHeader >> 24) & 0xff, // 	(Sample Rate * BitsPerSample * Channels) / 8
    //     0x02, 0x00, 0x10, 0x00,
    //     0x64, 0x61, 0x74, 0x61, // data
    //     dataSize & 0xff, (dataSize >> 8) & 0xff, (dataSize >> 16) & 0xff, (dataSize >> 24) & 0xff,
    // };
    // _mutex->lock();
    // len = sprintf(_buffer, "{\"type\":\"STM_HEADER\", ");
    // len += sprintf(_buffer+len, "\"data\":\"");
    // for (size_t ix = 0; ix < 44; ix++) {
    //     // printf("%02x", wav_header[ix]);
    //     len += sprintf(_buffer+len, "%02x", wav_header[ix]);
    // }
    // len += sprintf(_buffer+len, "\", \"dest\":\"%s\"}", DEST);
    // _cond->notify_all();
    // _mutex->unlock();
}

void MyAudio::setup(){
    printf("Hello from the B-L475E-IOT01A microphone demo\n");
    printf("wavFreq: %d, sbcHeader: %d\n", wavFreq, sbcHeader);

    if (!TARGET_AUDIO_BUFFER) {
        printf("Failed to allocate TARGET_AUDIO_BUFFER buffer\n");
        return;
    }

    // set up the microphone
    MicParams.BitsPerSample = 16;
    MicParams.ChannelsNbr = AUDIO_CHANNELS;
    MicParams.Device = AUDIO_IN_DIGITAL_MIC1;
    MicParams.SampleRate = AUDIO_SAMPLING_FREQUENCY;
    MicParams.Volume = 32;

    int32_t ret = BSP_AUDIO_IN_Init(AUDIO_INSTANCE, &MicParams);

    if (ret != BSP_ERROR_NONE) {
        printf("Error Audio Init (%d)\r\n", ret);
        return;
    } else {
        printf("OK Audio Init\t(Audio Freq=%d)\r\n", AUDIO_SAMPLING_FREQUENCY);
    }

    while (true) {
        if(*_mode=='A'&&isRecording){
            record();
            // _event_queue->call(callback(this, &MyAudio::start_recording));
            ThisThread::sleep_for(1ms);
        }
        else{
            ThisThread::sleep_for(100ms);
        }
    }
}

void MyAudio::start(){
    _thread.start(callback(this, &MyAudio::setup));
}