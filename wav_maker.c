#include "wav.h"
//fn decs---------------------------------------------------------------------------------------
void handle_input(WAV* wav);

//main fn---------------------------------------------------------------------------------------
int main() {
    WAV* wav = wav_init();
    handle_input(wav);
    wav_write(wav, wav->file_name);
    free(wav->buffer);
    free(wav);
    return EXIT_SUCCESS;
}
//fn defs---------------------------------------------------------------------------------------
void handle_input(WAV* wav) {
    char line[256];
    bool start = 0;
    bool end = 0;
    while (fgets(line, sizeof(line), stdin) != NULL) {
        if (strcmp(line, "\n") != 0) {
            if (strncmp(line, ">", 1) == 0 && !end) 
                process_start_end(wav, line, &start, &end);
            else if (line[0] >= '0' && line[0] <= '9' && start && !end)
                process_note(wav, line);
            else if(!strncmp(line, "~", 1) && !end)
                set_wav_members(wav, line);
        }
    }
}




