#include "notes.h"
#include "wav.h"

short int sawtooth(WAV* wav, double freq, int amplitude, int* pin) {
    int samplesPerWavelength = wav->header.sample_rate / (freq / wav->header.num_chans);
    int ampStep = (amplitude * 2) / samplesPerWavelength;
    return ((*pin % samplesPerWavelength) * ampStep);
}

void write_key(WAV* wav, char* note, int* pin, int* end_pin, double t) {
    static long double fq; fq = get_note(note);
                                        //0.005
    COEFS co = set_coefs(wav, t, 0.005, 0.9, 500);
    static short int bit;
    *end_pin = *pin + (t * (double)wav->header.bytes_per_second / 2);
    for (int j = *pin; j < *end_pin; j++) {
        if (j < wav->buffer_size) {
            bit = (sin(PI_2 * fq * j * co.cs_coef) * get_fifol(&co));

            while (bit % 200 != 0) {
                bit -= 1;
            }

            wav->buffer[j] += bit;
            wav->buffer[j] += sawtooth(wav, fq, 200, &j);
            if ((*end_pin - j) < co.fade_o) co.f_o--;
            if (j < (*pin + co.fade_i)) co.f_i++;
        }
        else { printf("Pin has exceeded the buffer size.\n"); break; }
    }
}

void write_kick(WAV* wav, int* pin, int* end_pin, double t) {
    static float fq = KCK;
    COEFS co = set_coefs(wav, t, 0.01, 0.999, 1400);
    *end_pin = *pin + (t * (double)wav->header.bytes_per_second / 2);
    static short int bit;
    float envelope = 500;
    for (int j = *pin, i=0; j < *end_pin; j++, i++) {
        if (j < wav->buffer_size) {
            bit = (sin(PI_2 * 0.5 * fq * j * co.cs_coef) * get_fifol(&co)) +
                (sin(PI_2 * 1 * fq * j * co.cs_coef) * 8*get_fifol(&co)) +
                (sin(PI_2 * 2 * fq * j * co.cs_coef) * get_fifol(&co));

            while (bit % 200 != 0) {
                bit -= 1;
            }
            
            float noise = ((float)rand() / RAND_MAX) * 2.0 - 1.0;  // White noise
            float filtered_noise = bandpass_filter(noise);

            wav->buffer[j] += bit;
            wav->buffer[j] += sawtooth(wav, fq, 600, &j);
            wav->buffer[j] += filtered_noise * envelope;
            if ((*end_pin - j) < co.fade_o) co.f_o--;
            if (j < (*pin + co.fade_i)) co.f_i++;
            if (i > 1000 && envelope >= 1) envelope -= 0.1;
        }
        else { printf("Pin has exceeded the buffer size.\n"); break; }
    }
}

void write_hihat(WAV* wav, int* pin, int* end_pin, double t) {
    *end_pin = *pin + (t * (double)wav->header.bytes_per_second / 2);

    float envelope = 500;
    for (int i = *pin, j=0; i < *end_pin; i++, j++) {
        float noise = ((float)rand() / RAND_MAX) * 2.0 - 1.0;  // White noise
        float filtered_noise = bandpass_filter(noise);

        wav->buffer[i+0] += filtered_noise * envelope;

        if(j > 1000 && envelope > 1) envelope -= 0.1;
    }
}

float bandpass_filter(float sample) {
    static float prev1 = 0.0;
    static float prev2 = 0.0;
    float output = 0.98 * (sample - prev1 + prev2);
    prev2 = prev1;
    prev1 = sample;
    return output;
}

void write_snare(WAV* wav, int* pin, int* end_pin, double t) {
    *end_pin = *pin + (t * (double)wav->header.bytes_per_second / 2);

    float envelope = 3000;
    for (int i = *pin, j=0; i < *end_pin; i++, j++) {
        float noise = ((float)rand() / RAND_MAX) * 2.0 - 1.0;  // White noise
        float filtered_noise = bandpass_filter(noise);

        wav->buffer[i] += filtered_noise * envelope;


        if(j > 500 && envelope > 1) envelope -= 0.5;
    }
}

void write_crash(WAV* wav, int* pin, int* end_pin, double t) {
    static const int amplitude = 500;
    static const short int fq = 400;
    COEFS co = set_coefs(wav, t, 0.02, 0.5, 5000);
    *end_pin = *pin + (t * (double)wav->header.bytes_per_second / 2);

    for (int j = *pin, i = 0; j < *end_pin; j += 8, i++) {

        wav->buffer[j + 0] += sawtooth(wav, fq * 1, amplitude, &i);
        wav->buffer[j + 1] += sawtooth(wav, fq * 2, amplitude, &i);
        wav->buffer[j + 2] += sawtooth(wav, fq * 4, amplitude, &i);


        if ((*end_pin - j) < co.fade_o) co.f_o--;
        if (j < (*pin + co.fade_i)) co.f_i++;
    }
}
