#include "Ears.h"
#include <string.h>
#include <math.h>

void init_audio_data(AudioData **audio_data) {
    if (!*audio_data) {
        *audio_data = (AudioData*)malloc(sizeof(AudioData));
        if (!*audio_data) {
            fprintf(stderr, "Failed to allocate memory for audio data structure\n");
            return; // Better to return than exit, let caller handle errors
        }

        (*audio_data)->time_domain = (double*)malloc(FFT_SIZE * sizeof(double));
        (*audio_data)->frequency_domain = (double_complex*)fftw_malloc(sizeof(double_complex) * FFT_SIZE);
        if (!(*audio_data)->time_domain || !(*audio_data)->frequency_domain) {
            fprintf(stderr, "Failed to allocate memory for audio data arrays\n");
            free_audio_data(*audio_data);
            return; // Return error to caller
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
    for (int i = 0; i < FFT_SIZE/2 + 1; i++) {  
        double freq = (i * SAMPLING_RATE) / (double)FFT_SIZE;
        double weight = exp(-pow((log(freq) - log(inner_ear->cochlea_frequency)) / 0.5, 2));
        audio_data->frequency_domain[i] *= weight;
    }
}

void generate_fibonacci_sequence(double *sequence, int *length) {
    sequence[0] = MIN_FREQ;
    sequence[1] = MIN_FREQ + 1;
    int i = 2;
    while (sequence[i-1] < MAX_FREQ && i < 128) { 
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
        for (int oct = -2; oct <= 2; oct++) { 
            double adjusted_freq = freq * pow(2, oct);
            if (adjusted_freq >= MIN_FREQ && adjusted_freq <= MAX_FREQ) {
                for (int j = 0; j < NOTES_PER_OCTAVE; j++) {
                    if (fabs(adjusted_freq - octave_notes[j].frequency) < 20) { 
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

void apply_window_function(AudioData *audio_data) {
    for (int i = 0; i < FFT_SIZE; i++) {
        double window = 0.5 * (1 - cos(2 * M_PI * i / (FFT_SIZE - 1)));
        audio_data->time_domain[i] *= window;
    }
}

void apply_convolution(AudioData *audio_data, const double *kernel, int kernel_size) {
    double *convolved = (double*)malloc(FFT_SIZE * sizeof(double));
    if (!convolved) {
        fprintf(stderr, "Memory allocation failed for convolution result\n");
        return;
    }

    memset(convolved, 0, FFT_SIZE * sizeof(double));

    for (int n = 0; n < FFT_SIZE; ++n) {
        for (int k = 0; k < kernel_size; ++k) {
            if (n - k >= 0) {
                convolved[n] += audio_data->time_domain[n - k] * kernel[k];
            }
        }
    }

    memcpy(audio_data->time_domain, convolved, FFT_SIZE * sizeof(double));
    free(convolved);

    perform_fourier_transform(audio_data);
}

void perform_inverse_fft(AudioData *audio_data) {
    fftw_plan inverse_plan = fftw_plan_dft_c2r_1d(FFT_SIZE, 
        (fftw_complex*)audio_data->frequency_domain,
        audio_data->time_domain,
        FFTW_ESTIMATE);
    
    fftw_execute(inverse_plan);
    
    for (int i = 0; i < FFT_SIZE; i++) {
        audio_data->time_domain[i] /= FFT_SIZE; // Normalize
    }
    
    fftw_destroy_plan(inverse_plan);
}

void simulate_cochlear_response(InnerEar *inner_ear, double frequency) {
    AudioData *audio_data = NULL;
    init_audio_data(&audio_data);

    if (!audio_data) {
        fprintf(stderr, "Failed to initialize audio data\n");
        return;
    }

    populate_time_domain_signal(audio_data, frequency, 0.046);

    apply_window_function(audio_data);

    perform_fourier_transform(audio_data);
    apply_cochlear_filter(audio_data, inner_ear);

    double kernel[] = {0.1, 0.2, 0.4, 0.2, 0.1};
    int kernel_size = sizeof(kernel) / sizeof(kernel[0]);
    apply_convolution(audio_data, kernel, kernel_size);

    // Optionally, perform inverse FFT
    // perform_inverse_fft(audio_data);

    simulate_octave_range(inner_ear);

    free_audio_data(audio_data);
}
