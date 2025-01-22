#include <pocketsphinx.h>
#include <iostream>
#include <fstream>

int main() {
    FILE *fh;
        size_t len;
    
    // Paths to your model files
    const char *acousticModelPath = "model/en-us"; // E.g., "cmusphinx-it-5.2/model_parameters/voxforge_it_sphinx.cd_cont_5000"
    const char *languageModelPath = "model/en-us.lm.bin"; // E.g., "it.lm"
    const char *dictionaryPath = "model/cmudict-en-us.dict";     // E.g., "it.dic"
    const char *audioFilePath = "hello-single.wav";

    /* Look for a single audio file as input parameter. */
    if ((fh = fopen(audioFilePath, "rb")) == NULL)
        E_FATAL_SYSTEM("Failed to open %s", audioFilePath);

    /* Get the size of the input. */
    if (fseek(fh, 0, SEEK_END) < 0)
        E_FATAL_SYSTEM("Unable to find end of input file %s", audioFilePath);
    len = ftell(fh);
    rewind(fh);

    // Initialize PocketSphinx configuration
    ps_config_t *config;
    config =  ps_config_init(NULL);
    
    if (config == NULL) {
        std::cerr << "Failed to initialize configuration." << std::endl;
        return 1;
    }

    ps_default_search_args(config);

    // Set configuration parameters
    ps_config_set_str(config, "hmm", acousticModelPath);
    ps_config_set_str(config, "lm", languageModelPath);
    ps_config_set_str(config, "dict", dictionaryPath);
    
    if (ps_config_soundfile(config, fh, audioFilePath) < 0)
        E_FATAL("Unsupported input file %s\n", audioFilePath);

    

    // Initialize PocketSphinx decoder
    ps_decoder_t *ps = ps_init(config);
    if (ps == NULL) {
        std::cerr << "Failed to initialize PocketSphinx decoder." << std::endl;
        ps_config_free(config);
        return 1;
    }

    // Open the audio file
    FILE *audioFile = fopen(audioFilePath, "rb");
    if (audioFile == NULL) {
        std::cerr << "Failed to open audio file: " << audioFilePath << std::endl;
        ps_free(ps);
        ps_config_free(config);
        return 1;
    }

    // Start utterance processing
    ps_start_utt(ps);
    int16_t buffer[512];
    size_t samplesRead;

    while ((samplesRead = fread(buffer, sizeof(int16_t), 512, audioFile)) > 0) {
        ps_process_raw(ps, buffer, samplesRead, FALSE, FALSE);
    }

    // End utterance processing
    ps_end_utt(ps);

    // Get segmentation for phonemes
    ps_seg_t *seg = ps_seg_iter(ps);
    if (seg == NULL) {
        std::cerr << "No segmentation available." << std::endl;
    } else {
        std::cout << "Phonemes detected:\n";
        while (seg != NULL) {
            const char *word;
            int startFrame, endFrame;
            word = ps_seg_word(seg); // Get phoneme or word
            ps_seg_frames(seg, &startFrame, &endFrame); // Get start and end frame

            std::cout << "Phoneme: " << word
                      << ", Start Frame: " << startFrame
                      << ", End Frame: " << endFrame
                      << std::endl;

            seg = ps_seg_next(seg); // Move to the next segment
        }
    }

    // Clean up
    fclose(audioFile);
    ps_free(ps);
    ps_config_free(config);

    return 0;
}
