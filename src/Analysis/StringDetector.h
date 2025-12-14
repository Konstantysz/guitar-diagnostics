#pragma once

#include <array>
#include <string>

namespace GuitarDiagnostics::Analysis
{
    /**
     * @brief Information about a detected guitar string.
     */
    struct StringInfo
    {
        int stringNumber = -1;              ///< 1-6 (1=low E, 6=high e), -1 if unknown
        std::string stringName = "Unknown"; ///< "E", "A", "D", "G", "B", "e", or "Unknown"
        float confidence = 0.0f;            ///< 0.0-1.0 (1.0 = exact match, 0.0 = no confidence)
        float detuneAmount = 0.0f;          ///< Cents deviation from reference pitch (+ = sharp, - = flat)
    };

    namespace StringDetector
    {
        /// Standard tuning frequencies in Hz (E2, A2, D3, G3, B3, E4)
        constexpr std::array<float, 6> kStandardTuning = {
            82.41f,  // E2 (low E string)
            110.0f,  // A2
            146.83f, // D3
            196.0f,  // G3
            246.94f, // B3
            329.63f  // E4 (high e string)
        };

        /// String names corresponding to kStandardTuning
        constexpr std::array<const char *, 6> kStringNames = { "E", "A", "D", "G", "B", "e" };

        /// Minimum guitar frequency (below low E - 15%)
        constexpr float kMinGuitarFreq = 70.0f;

        /// Maximum guitar frequency (above high e + 20%)
        constexpr float kMaxGuitarFreq = 400.0f;

        /// Cents threshold for high confidence (within ±5 cents = perfect)
        constexpr float kHighConfidenceThreshold = 5.0f;

        /// Cents threshold for zero confidence (>±50 cents = unreliable)
        constexpr float kZeroConfidenceThreshold = 50.0f;

        /**
         * @brief Calculates the cents deviation between measured and reference frequencies.
         *
         * @param measured The measured frequency in Hz.
         * @param reference The reference frequency in Hz.
         * @return Cents deviation (1200 * log2(measured / reference)).
         *         Positive = sharp, negative = flat.
         */
        float CalculateCents(float measured, float reference);

        /**
         * @brief Calculates confidence score based on cents deviation.
         *
         * @param cents Absolute cents deviation from reference.
         * @return Confidence in range [0.0, 1.0].
         *         1.0 if within ±5 cents, linear decay to 0.0 at ±50 cents.
         */
        float ConfidenceFromCents(float cents);

        /**
         * @brief Classifies a frequency to the nearest guitar string in standard tuning.
         *
         * @param frequency Fundamental frequency in Hz (typically 70-400 Hz for guitar).
         * @return StringInfo containing string number, name, confidence, and detune amount.
         *
         * Confidence scoring:
         * - 1.0 if within ±5 cents of reference pitch
         * - Linear decay to 0.0 at ±50 cents
         * - 0.0 if out of guitar range (<70 Hz or >400 Hz)
         *
         * Thread-safe: Pure computation, no allocations, no I/O.
         */
        StringInfo Classify(float frequency);

    } // namespace StringDetector

} // namespace GuitarDiagnostics::Analysis
