#include "Analysis/StringDetector.h"
#include <gtest/gtest.h>
#include <cmath>

using namespace GuitarDiagnostics::Analysis;

// Test exact standard tuning frequencies
TEST(StringDetectorTest, ClassifyExactStandardTuning)
{
    // Low E string (82.41 Hz)
    auto result = StringDetector::Classify(82.41f);
    EXPECT_EQ(result.stringNumber, 1);
    EXPECT_EQ(result.stringName, "E");
    EXPECT_NEAR(result.confidence, 1.0f, 0.01f);
    EXPECT_NEAR(result.detuneAmount, 0.0f, 0.1f);

    // A string (110.0 Hz)
    result = StringDetector::Classify(110.0f);
    EXPECT_EQ(result.stringNumber, 2);
    EXPECT_EQ(result.stringName, "A");
    EXPECT_NEAR(result.confidence, 1.0f, 0.01f);
    EXPECT_NEAR(result.detuneAmount, 0.0f, 0.1f);

    // D string (146.83 Hz)
    result = StringDetector::Classify(146.83f);
    EXPECT_EQ(result.stringNumber, 3);
    EXPECT_EQ(result.stringName, "D");
    EXPECT_NEAR(result.confidence, 1.0f, 0.01f);
    EXPECT_NEAR(result.detuneAmount, 0.0f, 0.1f);

    // G string (196.0 Hz)
    result = StringDetector::Classify(196.0f);
    EXPECT_EQ(result.stringNumber, 4);
    EXPECT_EQ(result.stringName, "G");
    EXPECT_NEAR(result.confidence, 1.0f, 0.01f);
    EXPECT_NEAR(result.detuneAmount, 0.0f, 0.1f);

    // B string (246.94 Hz)
    result = StringDetector::Classify(246.94f);
    EXPECT_EQ(result.stringNumber, 5);
    EXPECT_EQ(result.stringName, "B");
    EXPECT_NEAR(result.confidence, 1.0f, 0.01f);
    EXPECT_NEAR(result.detuneAmount, 0.0f, 0.1f);

    // High e string (329.63 Hz)
    result = StringDetector::Classify(329.63f);
    EXPECT_EQ(result.stringNumber, 6);
    EXPECT_EQ(result.stringName, "e");
    EXPECT_NEAR(result.confidence, 1.0f, 0.01f);
    EXPECT_NEAR(result.detuneAmount, 0.0f, 0.1f);
}

// Test slightly detuned strings (within high confidence range)
TEST(StringDetectorTest, ClassifySlightlyDetuned)
{
    // G string detuned +3 cents (should still have high confidence)
    // 196 Hz * 2^(3/1200) ≈ 196.34 Hz
    float detunedFreq = 196.0f * std::pow(2.0f, 3.0f / 1200.0f);
    auto result = StringDetector::Classify(detunedFreq);

    EXPECT_EQ(result.stringNumber, 4);
    EXPECT_EQ(result.stringName, "G");
    EXPECT_GT(result.confidence, 0.9f);
    EXPECT_NEAR(result.detuneAmount, 3.0f, 0.5f);
}

// Test moderately detuned strings
TEST(StringDetectorTest, ClassifyModeratelyDetuned)
{
    // G string detuned +15 cents
    // 196 Hz * 2^(15/1200) ≈ 197.71 Hz
    float detunedFreq = 196.0f * std::pow(2.0f, 15.0f / 1200.0f);
    auto result = StringDetector::Classify(detunedFreq);

    EXPECT_EQ(result.stringNumber, 4);
    EXPECT_EQ(result.stringName, "G");
    EXPECT_GT(result.confidence, 0.5f);
    EXPECT_LT(result.confidence, 0.9f);
    EXPECT_NEAR(result.detuneAmount, 15.0f, 1.0f);
}

// Test severely detuned strings
TEST(StringDetectorTest, ClassifySeverelyDetuned)
{
    // G string detuned -40 cents (very flat)
    // 196 Hz * 2^(-40/1200) ≈ 191.5 Hz
    float detunedFreq = 196.0f * std::pow(2.0f, -40.0f / 1200.0f);
    auto result = StringDetector::Classify(detunedFreq);

    EXPECT_EQ(result.stringNumber, 4);
    EXPECT_EQ(result.stringName, "G");
    EXPECT_LT(result.confidence, 0.5f);
    EXPECT_NEAR(result.detuneAmount, -40.0f, 2.0f);
}

// Test extremely detuned strings (near confidence threshold)
TEST(StringDetectorTest, ClassifyExtremelyDetuned)
{
    // A string detuned +48 cents (near 50 cent threshold)
    // 110 Hz * 2^(48/1200) ≈ 112.76 Hz
    float detunedFreq = 110.0f * std::pow(2.0f, 48.0f / 1200.0f);
    auto result = StringDetector::Classify(detunedFreq);

    EXPECT_EQ(result.stringNumber, 2);
    EXPECT_EQ(result.stringName, "A");
    EXPECT_LT(result.confidence, 0.2f);
    EXPECT_GT(result.confidence, 0.0f);
    EXPECT_NEAR(result.detuneAmount, 48.0f, 2.0f);
}

// Test out-of-range frequencies (below guitar range)
TEST(StringDetectorTest, ClassifyBelowRange)
{
    auto result = StringDetector::Classify(50.0f);

    EXPECT_EQ(result.stringNumber, -1);
    EXPECT_EQ(result.stringName, "Unknown");
    EXPECT_EQ(result.confidence, 0.0f);
    EXPECT_EQ(result.detuneAmount, 0.0f);
}

// Test out-of-range frequencies (above guitar range)
TEST(StringDetectorTest, ClassifyAboveRange)
{
    auto result = StringDetector::Classify(450.0f);

    EXPECT_EQ(result.stringNumber, -1);
    EXPECT_EQ(result.stringName, "Unknown");
    EXPECT_EQ(result.confidence, 0.0f);
    EXPECT_EQ(result.detuneAmount, 0.0f);
}

// Test edge case: exactly at minimum threshold
TEST(StringDetectorTest, ClassifyAtMinimumThreshold)
{
    auto result = StringDetector::Classify(70.0f);

    EXPECT_NE(result.stringNumber, -1);
    EXPECT_NE(result.stringName, "Unknown");
}

// Test edge case: exactly at maximum threshold
TEST(StringDetectorTest, ClassifyAtMaximumThreshold)
{
    auto result = StringDetector::Classify(400.0f);

    EXPECT_NE(result.stringNumber, -1);
    EXPECT_NE(result.stringName, "Unknown");
}

// Test ambiguous case: frequency exactly between two strings
TEST(StringDetectorTest, ClassifyAmbiguousFrequency)
{
    // Geometric mean between D (146.83 Hz) and G (196.0 Hz)
    // sqrt(146.83 * 196.0) ≈ 169.8 Hz
    float midpoint = std::sqrt(146.83f * 196.0f);
    auto result = StringDetector::Classify(midpoint);

    // Should pick either D or G (whichever is closer in cents)
    EXPECT_TRUE(result.stringNumber == 3 || result.stringNumber == 4);
    EXPECT_TRUE(result.stringName == "D" || result.stringName == "G");

    // Confidence should be relatively low due to ambiguity
    EXPECT_LT(result.confidence, 0.7f);
}

// Test positive vs negative detune
TEST(StringDetectorTest, DetectSharpVsFlat)
{
    // E string sharp by 10 cents
    float sharpFreq = 82.41f * std::pow(2.0f, 10.0f / 1200.0f);
    auto sharpResult = StringDetector::Classify(sharpFreq);

    EXPECT_EQ(sharpResult.stringNumber, 1);
    EXPECT_GT(sharpResult.detuneAmount, 0.0f);
    EXPECT_NEAR(sharpResult.detuneAmount, 10.0f, 1.0f);

    // E string flat by 10 cents
    float flatFreq = 82.41f * std::pow(2.0f, -10.0f / 1200.0f);
    auto flatResult = StringDetector::Classify(flatFreq);

    EXPECT_EQ(flatResult.stringNumber, 1);
    EXPECT_LT(flatResult.detuneAmount, 0.0f);
    EXPECT_NEAR(flatResult.detuneAmount, -10.0f, 1.0f);
}

// Test confidence scoring function directly
TEST(StringDetectorTest, ConfidenceScoring)
{
    // Within ±5 cents: confidence = 1.0
    EXPECT_NEAR(StringDetector::ConfidenceFromCents(0.0f), 1.0f, 0.01f);
    EXPECT_NEAR(StringDetector::ConfidenceFromCents(2.5f), 1.0f, 0.01f);
    EXPECT_NEAR(StringDetector::ConfidenceFromCents(5.0f), 1.0f, 0.01f);

    // At 27.5 cents (midpoint between 5 and 50): confidence ≈ 0.5
    EXPECT_NEAR(StringDetector::ConfidenceFromCents(27.5f), 0.5f, 0.05f);

    // At ±50 cents: confidence = 0.0
    EXPECT_NEAR(StringDetector::ConfidenceFromCents(50.0f), 0.0f, 0.01f);

    // Beyond ±50 cents: confidence = 0.0
    EXPECT_NEAR(StringDetector::ConfidenceFromCents(60.0f), 0.0f, 0.01f);
}

// Test cents calculation function directly
TEST(StringDetectorTest, CentsCalculation)
{
    // One octave = 1200 cents
    EXPECT_NEAR(StringDetector::CalculateCents(200.0f, 100.0f), 1200.0f, 0.1f);

    // Perfect fifth ≈ 702 cents
    EXPECT_NEAR(StringDetector::CalculateCents(150.0f, 100.0f), 702.0f, 1.0f);

    // One semitone = 100 cents
    float semitone = 100.0f * std::pow(2.0f, 1.0f / 12.0f); // ≈ 105.95 Hz
    EXPECT_NEAR(StringDetector::CalculateCents(semitone, 100.0f), 100.0f, 0.1f);

    // Negative cents (flat)
    EXPECT_NEAR(StringDetector::CalculateCents(100.0f, 200.0f), -1200.0f, 0.1f);

    // Zero cents (identical frequencies)
    EXPECT_NEAR(StringDetector::CalculateCents(100.0f, 100.0f), 0.0f, 0.01f);
}

// Test realistic guitar tuning scenarios
TEST(StringDetectorTest, RealisticTuningScenarios)
{
    // Scenario 1: Guitar slightly out of tune (+7 cents on G)
    float slightlySharp = 196.0f * std::pow(2.0f, 7.0f / 1200.0f);
    auto result1 = StringDetector::Classify(slightlySharp);

    EXPECT_EQ(result1.stringNumber, 4);
    EXPECT_GT(result1.confidence, 0.85f);

    // Scenario 2: Guitar significantly out of tune (-25 cents on B)
    float significantlyFlat = 246.94f * std::pow(2.0f, -25.0f / 1200.0f);
    auto result2 = StringDetector::Classify(significantlyFlat);

    EXPECT_EQ(result2.stringNumber, 5);
    EXPECT_GT(result2.confidence, 0.3f);
    EXPECT_LT(result2.confidence, 0.7f);

    // Scenario 3: Well-tuned guitar (±1 cent on A)
    float wellTuned = 110.0f * std::pow(2.0f, 1.0f / 1200.0f);
    auto result3 = StringDetector::Classify(wellTuned);

    EXPECT_EQ(result3.stringNumber, 2);
    EXPECT_GT(result3.confidence, 0.95f);
}

// Test that all standard strings can be detected uniquely
TEST(StringDetectorTest, UniqueStringDetection)
{
    std::array<float, 6> standardTuning = { 82.41f, 110.0f, 146.83f, 196.0f, 246.94f, 329.63f };
    std::array<std::string, 6> expectedNames = { "E", "A", "D", "G", "B", "e" };

    for (size_t i = 0; i < standardTuning.size(); ++i)
    {
        auto result = StringDetector::Classify(standardTuning[i]);

        EXPECT_EQ(result.stringNumber, static_cast<int>(i + 1));
        EXPECT_EQ(result.stringName, expectedNames[i]);
        EXPECT_NEAR(result.confidence, 1.0f, 0.01f);
    }
}
