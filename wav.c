#include "wav.h"
#include "utils.h"
#include "drawing.h"
#include "notes.h"


key_val keys_lookuptable[] = {
    { "SL", 0.0 },
    { "Cb", B/2},
    { "C" , C  },
    { "C#", Db },
    { "Db", Db },
    { "D" , D  },
    { "D#", Eb },   
    { "Eb", Eb },
    { "E" , E  },
    { "E#", F  },
    { "Fb", E  },
    { "F" , F  },
    { "F#", Fs },
    { "Gb", Fs },
    { "G" , G  },
    { "G#", Ab },
    { "Ab", Ab },
    { "A" , A  },
    { "A#", Bb },
    { "Bb", Bb },
    { "B" , B  },
    { "B#", C*2},
};

key_val drum_lookuptable[] = {
    { "Ki", KCK },
    { "Sn", SNR },
    { "Hi", HIH },
    { "Cr", CRA },
};
void play_stdin(WAV* wav, double t, char* note) {
    static int pin, end_pin;
    if (t == 0 || note == NULL) {
        pin = end_pin; return;
    }
    if (is_key(note))
        write_key(wav, note, &pin, &end_pin, t);
    else if (is_drum(note)) {
        if (!strcmp(note, "Ki"))
            write_kick(wav, &pin, &end_pin, t);
        else if(!strcmp(note, "Hi"))
            write_hihat(wav, &pin, &end_pin, t);
        else if(!strcmp(note, "Sn"))
            write_snare(wav, &pin, &end_pin, t);
        else if(!strcmp(note, "Cr"))
            write_crash(wav, &pin, &end_pin, t);
        
    }
}



WAV* wav_init(void) {
    WAV n_wav = {
        .header = {
            .num_chans = 0,
            .sample_rate = 0,
            .bytes_per_sample = 0,
            .bytes_per_second = 0,
            .flength = 0,
            .dlength = 0,
            .format_tag = 1,
            .chunk_size = 16,
            .bits_per_sample = 16,
        },
        .buffer_size = 0,
        .loudness = 0,
        .tempo = 0,
        .buffer = NULL,
    };

    memcpy(n_wav.header.riff, "RIFF", 4);
    memcpy(n_wav.header.wave, "WAVE", 4);
    memcpy(n_wav.header.data, "data", 4);
    memcpy(n_wav.header.fmt, "fmt ", 4);
    memcpy(n_wav.file_name, "non", 3);

    WAV* n_mal = (WAV*)malloc(sizeof(WAV));
    memcpy(n_mal, &n_wav, sizeof(WAV));
    return n_mal;
}
long double get_note(char* key) {
    if (key == NULL) {
        printf("> key is NULL\n");
        return 0;
    }
    static int n_keys = (sizeof(keys_lookuptable) / sizeof(key_val));
    static int n_drum = (sizeof(drum_lookuptable) / sizeof(key_val));
    char key_name[10] = { 0 };
    char key_reg[10] = { 0 };
    for (int idx = 0, i_ch = 0, i_num = 0; key[idx] != '\0'; idx++)
        if (key[idx] >= '0' && key[idx] <= '9')
            key_reg[i_num++] = key[idx];
        else key_name[i_ch++] = key[idx];
    for (int i = 0; i < n_keys; i++) {
        key_val* note_kv = &keys_lookuptable[i];
        if (strcmp(note_kv->name, key_name) == 0)
            return note_kv->val * pow(2, atoi(key_reg));
    }
    for (int i = 0; i < n_drum; i++) {
        key_val* note_kv = &drum_lookuptable[i];
        if (strcmp(note_kv->name, key_name) == 0)
            return note_kv->val;
    }
    printf("!~ there are no sounds called \"%s\"\n", key);
    return BAD_KEY;
}
bool is_drum(char* key) {
    if (key == NULL) return 0;
    int n_drum = (sizeof(drum_lookuptable) / sizeof(key_val));
    char drum_name[10] = { 0 };
    for (int idx = 0, i_ch = 0; key[idx] != '\0'; idx++)
        if (!(key[idx] >= '0' && key[idx] <= '9'))
            drum_name[i_ch++] = key[idx];
    for (int i = 0; i < n_drum; i++) {
        key_val* note_kv = &drum_lookuptable[i];
        if (strcmp(note_kv->name, drum_name) == 0)
            return 1;
    }
    return 0;
}
bool is_key(char* key) {
    if (key == NULL) return 0;
    int n_keys = (sizeof(keys_lookuptable) / sizeof(key_val));
    char key_name[10] = { 0 };
    for (int idx = 0, i_ch = 0; key[idx] != '\0'; idx++)
        if (!(key[idx] >= '0' && key[idx] <= '9'))
            key_name[i_ch++] = key[idx];
    for (int i = 0; i < n_keys; i++) {
        key_val* note_kv = &keys_lookuptable[i];
        if (strcmp(note_kv->name, key_name) == 0)
            return 1;
    }
    return 0;
}
void wav_write(WAV* wav, char* filepath) {
    FAIL_IF_MSG(!strcmp(filepath, "non"), "! file_name is unspecified - saving canceled");
    FILE* file_o = fopen(filepath, "wb");
    IF_MSG_SF(file_o == NULL, "> file opened succesfully", "! file failed to open");
    IF_MSG_SF(fwrite(&wav->header, sizeof(WAV_HEADER), 1, file_o) != 1,
        "> header write SUCCESS", "! header write FAIL");
    IF_MSG_SF(fwrite(wav->buffer, sizeof(short int), wav->buffer_size, file_o) != (size_t)wav->buffer_size,
        "> buffer write SUCCESS", "! buffer write FAIL");
    fclose(file_o);
    printf("> file closed\n");
}
COEFS set_coefs(WAV* wav, double t, float fade_in, float fade_out, int loudness) {
    COEFS n_coefs = {
        .sam_rate_coef = t * wav->header.sample_rate * wav->header.num_chans,
        .l_coef = wav->loudness * loudness,
        .cs_coef = (1.0 / wav->header.num_chans) * (1.0 / wav->header.sample_rate),
    };
    n_coefs.fade_i = n_coefs.sam_rate_coef * fade_in;
    n_coefs.fade_o = n_coefs.sam_rate_coef * fade_out;
    n_coefs.f_i = 0;
    n_coefs.f_o = n_coefs.fade_o;
    return n_coefs;
}
double get_fifol(COEFS* coefs_struct) {
    return (coefs_struct->f_i / coefs_struct->fade_i) *
        (coefs_struct->f_o / coefs_struct->fade_o) * coefs_struct->l_coef;
}
void set_wav_members(WAV* wav, char* line) {
    static const char s1[] = "file_name";
    static const char s2[] = "tempo";
    static const char s3[] = "loudness";
    static const char s4[] = "sample_rate";
    static const char s5[] = "num_of_channels";
    const char delims[] = " =\n,;~";
    char* tok = NULL;
    tok = strtok(line, delims);
    if (!strcmp(tok, s1)) {
        tok = strtok(NULL, delims);
        strcpy(wav->file_name, tok);
        FAIL_IF_MSG(strncmp(strreverse(tok), "vaw.", 4) != 0, "! file_name has to have a \".wav\" suffix");
        printf("> file_name: %s\n", wav->file_name);
        return;
    }
    else if (!strcmp(tok, s2)) {
        wav->tempo = atof(strtok(NULL, delims));
        FAIL_IF_MSG(wav->tempo <= 0, "! tempo must positive\n");
        printf("> tempo: %.0f bpm\n", wav->tempo);
        return;
    }
    else if (!strcmp(tok, s3)) {
        wav->loudness = atof(strtok(NULL, delims));
        FAIL_IF_MSG(wav->loudness <= 0, "! loudness level must be positive (float)");
        printf("> loudness: %.02lf\n", wav->loudness);
        return;
    }
    else if (!strcmp(tok, s4)) {
        wav->header.sample_rate = atof(strtok(NULL, delims));
        FAIL_IF_MSG(wav->header.sample_rate <= 0, "! sample rate must be positive (integer)");
        printf("> sample rate: %d\n", wav->header.sample_rate);
        return;
    }
    else if (!strcmp(tok, s5)) {
        wav->header.num_chans = atof(strtok(NULL, delims));
        FAIL_IF_MSG(wav->header.num_chans != 2 && wav->header.num_chans != 1,
            "! number of channels can either only be 1 (mono) or 2 (stereo)");
        printf("> # of channels: %d\n", wav->header.num_chans);
        return;
    }
    printf("!~ nothing happens when writing \"%s\"\n", line);
}
void process_start_end(WAV* wav, char* line, bool* start, bool* end) {
    const char delims[] = ",;> \n";
    char* tok = strtok(line, delims);
    printf("%s\n", tok);
    if (!strcmp(tok, "start")) {
        *start = 1;
        if (wav->header.num_chans && wav->header.sample_rate) {
            wav->header.bytes_per_sample = (wav->header.bits_per_sample / 8) * wav->header.num_chans;
            wav->header.bytes_per_second = wav->header.sample_rate * wav->header.bytes_per_sample;
        } else FAIL_IF_MSG(1, "! number of channels or sample rate is not yet set");
    } else if (!strcmp(tok, "end")) *end = 1;
}
void process_note(WAV* wav, char* line) {
    static int iter = 0;
    static double time, duration;
    static const char delims[] = ",;> \n";
    char* note = NULL;
    int expr = iter % 2;
    colour_print(expr, RED, GREEN, "> START - time: ", time, " seconds ");
    duration = (60.0 / wav->tempo) * atof(strtok(line, delims)) * 0.25;
    printf("\t> time span %.2lf s -> %.2lf s\n", time, time + duration);
    printf("\t> duration  %.2lf s\n", duration);

    int new_buffer_size = (wav->header.sample_rate * wav->header.num_chans * (time + duration));
    if (wav->buffer_size < new_buffer_size) {
        printf(" !~ more memory for the buffer is needed - %ld → %ld bytes\n", 
            wav->buffer_size*sizeof(short int), new_buffer_size*sizeof(short int));
        wav->buffer = my_realloc(wav->buffer, sizeof(short int), &wav->buffer_size, new_buffer_size);
    }
    note = strtok(NULL, delims);
    for (int j = 0; note != NULL; j++) {
        printf("\t %d) %s :: %.2Lf Hz\n", j + 1, note_str_format(note), get_note(note));
        play_stdin(wav, duration, note);
        note = strtok(NULL, delims);
    }
    play_stdin(wav, 0, NULL);
    colour_print(expr, RED, GREEN, "> END   - time: ", time += duration, " seconds ");
    iter++;
}
void play(WAV* wav, double duration, int note_count, ...) {
    va_list args;
    va_start(args, note_count);
    double note = 0;
    static int pin;
    static int end_pin;
    end_pin = pin + (duration * (double)wav->header.bytes_per_second / 2);
    printf("> buffer size: %d\n", wav->buffer_size);
    printf("> end pin: %d\n", end_pin);
    for (int i = 0; i < note_count; i++) {
        note = va_arg(args, double);
        printf("> note(%d) :: %.3lf Hz\n", i, note);
        for (int j = pin; j < end_pin; j++) {
            if (j < wav->buffer_size) {
                wav->buffer[j] += (short int)(sin((2 * PI * note * j) /
                    wav->header.num_chans /
                    wav->header.sample_rate) * wav->loudness * 1000);
            }
            else { printf("Pin has exceeded the buffer size.\n"); break; }
        }
    }
    pin = end_pin;
    va_end(args);
}
