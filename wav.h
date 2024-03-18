#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

#define FAIL_IF(EXP) { if(EXP) { exit(EXIT_FAILURE); } }
#define FAIL_IF_MSG(EXP, MSG) { if(EXP) { printf(MSG "\n"); exit(EXIT_FAILURE); }}
#define IF_MSG_SF(EXP, MSG_S, MSG_F) { if(EXP) { printf(MSG_F "\n");exit(EXIT_FAILURE); } else { printf(MSG_S "\n"); }}

#define PI   3.141592653589793238462643383279502884197169399
#define PI_2 (2 * PI)

#define BAD_KEY -1
#define C  16.3515978312874143467031358476881170272827148437
#define Db 17.3239144360545060570225578373835086822509765625
#define D  18.3540479948379775471018064965388774871826171875 
#define Eb 19.4454364826300581731463045697216391563415527343 
#define E  20.6017223070543662812060133521407847404479980468 
#define F  21.8267644645627459332144979633163213729858398437 
#define Fs 23.1246514194771497806140643888119459152221679687 
#define G  24.4997147488593255972764868537188768386840820312 
#define Ab 25.9565435987465735895802835497110481262207031252 
#define A  27.4999999999999964472863211994990701675415039061 
#define Bb 29.1352350948806206488751692592350845336914062501 
#define B  30.8677063285077445229250913859604597091674804687 

#define KCK 50.0
#define SNR 100.0
#define HIH 1000.0
#define CRA 1000.0

typedef struct {
    char riff[4];
    int32_t flength;
    char wave[4];
    char fmt[4];
    int32_t chunk_size;
    int16_t format_tag;
    int16_t num_chans;
    int32_t sample_rate;
    int32_t bytes_per_second;
    int16_t bytes_per_sample;
    int16_t bits_per_sample;
    char data[4];
    int32_t dlength;
} WAV_HEADER;

typedef struct {
    WAV_HEADER header;
    short int* buffer;

    int buffer_size;
    char file_name[100];
    float loudness;
    float tempo;
} WAV;

typedef struct { 
    char* name; 
    long double val; 
} key_val;

typedef struct {
    double sam_rate_coef;
    double cs_coef;
    double l_coef; 
    double fade_i, fade_o; 
    double f_i, f_o;
} COEFS;

WAV* wav_init(void);
void set_wav_members(WAV* wav, char* line);
void process_start_end(WAV* wav, char* line, bool* start, bool* end);
void process_note(WAV* wav, char* line);
long double get_note(char* key);
void wav_write(WAV* wav, char* filepath);
void play(WAV* wav, double duration, int note_count, ...);
void play_stdin(WAV* wav, double t, char* note);
bool is_key(char* key);
bool is_drum(char* key);
COEFS set_coefs(WAV* wav, double t, float fade_in, float fade_out, int loudness);
double get_fifol(COEFS* coefs_struct);