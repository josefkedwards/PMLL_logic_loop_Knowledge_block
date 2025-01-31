#include "Ears.h"

void init_audio_data(AudioData **audio_data) {
    if (!*audio_data) {
        *audio_data = (AudioData*)malloc(sizeof(AudioData));
        if (!*audio_data) {
            fprintf(stderr, "Failed to allocate memory for audio data structure\n");
            exit(1);
        }
        
        (*audio_data)->time_domain = (double*)malloc(FFT_SIZE * sizeof(double));
        (*audio_data)->frequency_domain = (double_complex*)fftw_malloc(sizeof(double_complex) * FFT_SIZE);
        if (!(*audio_data)->time_domain || !(*audio_data)->frequency_domain) {
            fprintf(stderr, "Failed to allocate memory for audio data arrays\n");
            free_audio_data(*audio_data);
            exit(1);
        }

        (*audio_data)->plan = fftw_plan_dft_r2c_1d(FFT_SIZE, (*audio_data)->time_domain, 
                                                    (fftw_complex*)(*audio_data)->frequency_domain, FFTW_ESTIMATE);
    }
}

void free_audio_data(AudioData *audio_data) {
    if (audio_data) {
        free(audio_data->time_domain);
        fftw_free(audio_data->frequency_domain);
        fftw_destroy_plan(audio_data->plan);
        free(audio_data);
    }
}

void perform_fourier_transform(AudioData *audio_data) {
    fftw_execute(audio_data->plan);
}

void apply_cochlear_filter(AudioData *audio_data, InnerEar *inner_ear) {
    for (int i = 0; i < FFT_SIZE/2 + 1; i++) {  // Only process positive frequencies
        double freq = (i * SAMPLING_RATE) / (double)FFT_SIZE;
        
        // Simple band-pass filter simulation
        double weight = exp(-pow((log(freq) - log(inner_ear->cochlea_frequency)) / 0.5, 2));
        
        // Adjust the magnitude of the frequency components
        audio_data->frequency_domain[i] *= weight;
    }
}

void generate_fibonacci_sequence(double *sequence, int *length) {
    sequence[0] = MIN_FREQ;
    sequence[1] = MIN_FREQ + 1;
    int i = 2;
    while (sequence[i-1] < MAX_FREQ) {
        sequence[i] = sequence[i-1] + sequence[i-2];
        if (sequence[i] > MAX_FREQ) break;
        i++;
    }
    *length = i - 1;
}

void simulate_octave_range(InnerEar *inner_ear) {
    double fibonacci_sequence[128];
    int sequence_length;
    generate_fibonacci_sequence(fibonacci_sequence, &sequence_length);

    MusicalNote octave_notes[] = {
        {"A", 440.0}, {"B", 493.88}, {"C", 523.25}, {"D", 587.33}, {"E", 659.26}, {"F", 698.46}, {"G", 783.99}
    };

    for (int i = 0; i < sequence_length; i++) {
        double freq = fibonacci_sequence[i];
        // Adjust frequency to octave fit
        for (int oct = -2; oct <= 2; oct++) { // simulate +- 2 octaves around central frequency
            double adjusted_freq = freq * pow(2, oct);
            if (adjusted_freq >= MIN_FREQ && adjusted_freq <= MAX_FREQ) {
                // Find closest musical note for perceptual processing
                for (int j = 0; j < NOTES_PER_OCTAVE; j++) {
                    if (fabs(adjusted_freq - octave_notes[j].frequency) < 20) { // within 20 Hz of note for simplicity
                        printf("Perceived note: %s at frequency %.2f Hz\n", octave_notes[j].note, adjusted_freq);
                        break;
                    }
                }
            }
        }
    }
}

void populate_time_domain_signal(AudioData *audio_data, double frequency, double duration) {
    for (int i = 0; i < FFT_SIZE; i++) {
        double time = i / (double)SAMPLING_RATE;
        if (time > duration) break;
        audio_data->time_domain[i] = sin(2 * M_PI * frequency * time);
    }
}

void simulate_cochlear_response(InnerEar *inner_ear, double frequency) {
    AudioData *audio_data;
    init_audio_data(&audio_data);
    
    // Populate with a simple sine wave for testing
    populate_time_domain_signal(audio_data, frequency, 0.046); // 0.046 seconds of signal for FFT_SIZE samples
    
    perform_fourier_transform(audio_data);
    apply_cochlear_filter(audio_data, inner_ear);

    // Simulate octave perception
    simulate_octave_range(inner_ear);

    free_audio_data(audio_data);
}
