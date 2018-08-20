/* =========================================================================

   DESCRIPTION
   cae_demo

   Copyright (c) 2016 by iFLYTEK, Co,LTD.  All Rights Reserved.
   ============================================================================ */

/* =========================================================================

   REVISION

   when            who              why
   --------        ---------        -------------------------------------------
   2016/10/09      cyhu             Created.
   ============================================================================ */

/* ------------------------------------------------------------------------
 ** Includes
 ** ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cae_intf.h"
#include "cae_lib.h"
#include "cae_errors.h"

/* ------------------------------------------------------------------------
 ** Macros
 ** ------------------------------------------------------------------------ */
#define  INPUT_FILE "test.pcm"
#define  PIECE_LEN  12288 


/* ------------------------------------------------------------------------
 ** Defines
 ** ------------------------------------------------------------------------ */
#define SAVE_CAE_OUT_PCM 1

/* ------------------------------------------------------------------------
 ** Types
 ** ------------------------------------------------------------------------ */
typedef struct _CAEUserData{
    FILE *fp_out;
}CAEUserData;


/* ------------------------------------------------------------------------
 ** Global Variable Definitions
 ** ------------------------------------------------------------------------ */	
static Proc_CAENew api_cae_new;
static Proc_CAEAudioWrite api_cae_audio_write;
static Proc_CAEResetEng api_cae_reset_eng;
static Proc_CAESetRealBeam api_cae_set_real_beam;
static Proc_CAESetWParam api_cae_set_wparam;
static Proc_CAEGetWParam api_cae_get_wparam;	
static Proc_CAEGetVersion api_cae_get_version;
static Proc_CAEGetChannel api_cae_get_channel;
static Proc_CAESetShowLog api_cae_set_show_log;
static Proc_CAEDestroy api_cae_destroy;

static FILE * finput;
CAE_HANDLE cae = NULL;

/* ------------------------------------------------------------------------
 ** Function Definitions
 ** ------------------------------------------------------------------------ */

static void CAEIvwCb(short angle, short channel, float power, short CMScore, short beam, char *param1, void *param2, void *userData)
{
    printf("\nCAEIvwCb ....\nangle:%d\n  param1:%s\n", angle, param1);

}

static void CAEAudioCb(const void *audioData, unsigned int audioLen, int param1, const void *param2, void *userData)
{
    CAEUserData *usDta = (CAEUserData*)userData;
#if SAVE_CAE_OUT_PCM
    fwrite(audioData, audioLen, 1, usDta->fp_out);
#endif
}

static int initFuncs()
{
    int ret = MSP_SUCCESS;
    const char* libname = "/usr/lib/libcae.so";
    void* hInstance = cae_LoadLibrary(libname);

    if(hInstance == NULL)
    {
        printf("Can not open library!\n");
        return MSP_ERROR_OPEN_FILE;
    }
    api_cae_new = (Proc_CAENew)cae_GetProcAddress(hInstance, "CAENew");
    api_cae_audio_write = (Proc_CAEAudioWrite)cae_GetProcAddress(hInstance, "CAEAudioWrite");
    api_cae_reset_eng = (Proc_CAEResetEng)cae_GetProcAddress(hInstance, "CAEResetEng");
    api_cae_set_real_beam = (Proc_CAESetRealBeam)cae_GetProcAddress(hInstance, "CAESetRealBeam");
    api_cae_set_wparam = (Proc_CAESetWParam)cae_GetProcAddress(hInstance, "CAESetWParam");
    api_cae_get_wparam = (Proc_CAEGetWParam)cae_GetProcAddress(hInstance, "CAEGetWParam");
    api_cae_get_version = (Proc_CAEGetVersion)cae_GetProcAddress(hInstance, "CAEGetVersion");
    api_cae_get_channel= (Proc_CAEGetChannel)cae_GetProcAddress(hInstance, "CAEGetChannel");
    api_cae_destroy = (Proc_CAEDestroy)cae_GetProcAddress(hInstance, "CAEDestroy");
    api_cae_set_show_log = (Proc_CAESetShowLog)cae_GetProcAddress(hInstance, "CAESetShowLog");
    return ret;
}

int  mChannels = 8;
int  mIs24bitMode = 0;
int preProcessBuffer(void *data, void *out, int bytes)
{
    int i = 0, j = 0;

    //fprintf(stderr,"preProcessBuffer mChannels:%d",mChannels);
    if (!mIs24bitMode) {
        for (i = 0; i < bytes / 2 ; ) {
            for (j = 0; j < mChannels; j++) {
                int tmp = 0;
                short tmp_data = (*((short *)data + i + j));
                tmp = ((tmp_data) << 16 | ((j+1) << 8)) & 0xffffff00;
                *((int *)out + i + j) = tmp;
            }
            i += mChannels;
        }
    } else {
        for (i = 0; i < bytes / 4 ; ) {
            for (j = 0; j < mChannels; j++) {
                int tmp = 0;
                int tmp_data = (*((int *)data + i + j)) << 1;
                tmp = ((((tmp_data & 0xfffff000) >> 12) << 12) | ((j+1) << 8)) & 0xffffff00;
                *((int *)out + i + j) = tmp;
            }
            i += mChannels;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int ret = MSP_SUCCESS;
    const char *resPath = "fo|/etc/ivw_resource.jet|0|1024";
    static CAEUserData userData;
    unsigned int pcmSize = 0;
    int totalsize = 0;
    int onec_szie = PIECE_LEN;
    char *pcmBuf = NULL;
    char *temp;

    if(initFuncs() != MSP_SUCCESS)
    {
        printf("load cae library failed\n");	
        return -1;
    }

#if SAVE_CAE_OUT_PCM
    userData.fp_out = fopen("cae_out.pcm", "wb");
    if (NULL == userData.fp_out)
    {
        printf("fopen cae_out.pcm file ....failed\n");
        ret = MSP_ERROR_OPEN_FILE;
        goto error;
    }
#endif

    ret = api_cae_new(&cae, resPath, CAEIvwCb, CAEAudioCb, NULL, &userData);
    if (MSP_SUCCESS != ret)
    {
        printf("CAENew ....failed\n");
        ret = MSP_ERROR_FAIL;
        goto error;;
    }
    /* 设置单麦克模式（视频通话模式） */
    //api_cae_set_wparam(cae, "single_mic", "1");

    /* 关闭唤醒功能（默认开启） */
    //api_cae_set_wparam(cae, "wakeup_enable", "0");
    //printf("print api_cae_set_wparam \n");
    /* 设置第一个唤醒词的门限值 */
    //api_cae_set_wparam(cae, "ivw_threshold_1", "15");

    finput = fopen(INPUT_FILE, "rb"); 
    if (NULL == finput)
    {
        printf("fopen input file ....failed\n");
        ret = MSP_ERROR_OPEN_FILE;
        goto error;
    }
    fseek(finput, 0, SEEK_END);
    totalsize = ftell(finput);
    fseek(finput, 0, SEEK_SET);

    pcmSize = onec_szie/2;
    pcmBuf = (char*)malloc(pcmSize);
    if (NULL == pcmBuf)
    {
        printf("malloc ....failed\n");
        ret  = MSP_ERROR_OUT_OF_MEMORY;
        goto error;
    }

    temp = (char*)malloc(onec_szie);
    if (temp == pcmBuf)
    {
        printf("malloc ....failed\n");
        ret  = MSP_ERROR_OUT_OF_MEMORY;
        goto error;
    }

    memset((void*)pcmBuf, 0, pcmSize);
    memset((void*)temp, 0, onec_szie);

    while( fread(pcmBuf, pcmSize, 1, finput)  > 0 )
    {
        printf("fread buffer:%d \n",pcmSize);
        preProcessBuffer(pcmBuf, temp, pcmSize);
        api_cae_audio_write(cae, temp, onec_szie);
    }


    sleep(3); 
    goto exit;
error:
exit:
    if(NULL != pcmBuf)
    {
        free(pcmBuf);
        pcmBuf= NULL;
    }
    if(NULL != temp)
    {
        free(temp);
        temp= NULL;
    }
    if(NULL != finput)
    {
        fclose(finput);
        finput = NULL;
    }
    api_cae_destroy(cae);
    cae = NULL;
#if SAVE_CAE_OUT_PCM
    if(NULL != userData.fp_out)
    {
        fclose(userData.fp_out);
        userData.fp_out = NULL;
    }
#endif
}


