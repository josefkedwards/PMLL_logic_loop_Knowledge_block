#include "ears.h"
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>

class Grok3AudioProcessor {
private:
    // RAII wrapper for AudioData
    struct AudioDataDeleter {
        void operator()(AudioData* ptr) { free_audio_data(ptr); }
    };
    std::unique_ptr<AudioData, AudioDataDeleter> audio_data_;
    
    // Helper function to check initialization
    void checkInitialization() const {
        if (!audio_data_) {
            throw std::runtime_error("Audio processor not initialized");
        }
    }

public:
    Grok3AudioProcessor() : audio_data_(nullptr) {
        AudioData* raw_data = nullptr;
        if (init_audio_data(&raw_data) != EARS_SUCCESS) {
            throw std::runtime_error("Failed to initialize audio processor");
        }
        audio_data_.reset(raw_data);
    }

    // Process a single frequency with advanced features
    void processFrequency(double frequency, bool enable_cochlear = true) {
        checkInitialization();
        
        std::cout << "Processing frequency: " << frequency << " Hz\n";
        
        // Generate input signal
        populate_time_domain_signal(audio_data_.get(), frequency, 0.1); // 100ms duration
        
        // Apply window function for better frequency resolution
        apply_window_function(audio_data_.get());
        
        // Perform FFT
        if (perform_fourier_transform(audio_data_.get()) != EARS_SUCCESS) {
            throw std::runtime_error("FFT processing failed");
        }
        
        if (enable_cochlear) {
            // Setup and apply cochlear filter
            InnerEar inner_ear = {
                .cochlea_frequency = frequency,
                .auditory_signal = 0.0,
                .vestibular_adjustment = 0.0
            };
            apply_cochlear_filter(audio_data_.get(), &inner_ear);
            
            // Analyze musical properties
            simulate_octave_range(&inner_ear);
        }
        
        // Demonstrate convolution with a simple low-pass filter
        std::vector<double> kernel = {0.1, 0.2, 0.4, 0.2, 0.1};
        apply_frequency_domain_convolution(audio_data_.get(), kernel.data(), kernel.size());
    }
    
    // Process continuous audio stream with overlap-add
    void processContinuousStream(const std::vector<double>& input_stream) {
        checkInitialization();
        
        std::cout << "Processing continuous stream of " << input_stream.size() << " samples\n";
        
        // Process with 50% overlap
        process_with_overlap_add(audio_data_.get(), 
                               input_stream.data(), 
                               input_stream.size(), 
                               FFT_SIZE / 2);
    }
    
    // Analyze frequency range using Fibonacci sequence
    void analyzeFrequencyRange() {
        std::vector<double> frequencies(128);
        int length;
        generate_fibonacci_sequence(frequencies.data(), &length);
        
        std::cout << "Analyzing frequency range using Fibonacci sequence:\n";
        for (int i = 0; i < length; ++i) {
            if (frequencies[i] >= MIN_FREQ && frequencies[i] <= MAX_FREQ) {
                processFrequency(frequencies[i]);
            }
        }
    }
};

// Example usage
int main() {
    try {
        Grok3AudioProcessor processor;
        
        // Process single frequencies
        std::cout << "\n=== Processing single frequencies ===\n";
        processor.processFrequency(440.0);  // A4 note
        processor.processFrequency(880.0);  // A5 note
        
        // Process continuous stream
        std::cout << "\n=== Processing continuous stream ===\n";
        std::vector<double> test_signal(FFT_SIZE * 4);
        for (size_t i = 0; i < test_signal.size(); ++i) {
            test_signal[i] = sin(2 * M_PI * 440.0 * i / SAMPLING_RATE);
        }
        processor.processContinuousStream(test_signal);
        
        // Analyze frequency range
        std::cout << "\n=== Analyzing frequency range ===\n";
        processor.analyzeFrequencyRange();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
