#include "StringDetector.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace GuitarDiagnostics::Analysis::StringDetector
{
    float CalculateCents(float measured, float reference)
    {
        return 1200.0f * std::log2(measured / reference);
    }

    float ConfidenceFromCents(float cents)
    {
        // High confidence if within ±5 cents
        if (cents <= kHighConfidenceThreshold)
        {
            return 1.0f;
        }

        // Linear decay from 1.0 at 5 cents to 0.0 at 50 cents
        if (cents >= kZeroConfidenceThreshold)
        {
            return 0.0f;
        }

        float normalized = (cents - kHighConfidenceThreshold) / (kZeroConfidenceThreshold - kHighConfidenceThreshold);

        return std::max(0.0f, 1.0f - normalized);
    }

    StringInfo Classify(float frequency)
    {
        // Check if frequency is within guitar range
        if (frequency < kMinGuitarFreq || frequency > kMaxGuitarFreq)
        {
            return { -1, "Unknown", 0.0f, 0.0f };
        }

        // Find nearest standard tuning reference
        float minCentsDistance = std::numeric_limits<float>::infinity();
        int bestMatchIndex = -1;

        for (size_t i = 0; i < kStandardTuning.size(); ++i)
        {
            float cents = CalculateCents(frequency, kStandardTuning[i]);
            float absCents = std::abs(cents);

            if (absCents < minCentsDistance)
            {
                minCentsDistance = absCents;
                bestMatchIndex = static_cast<int>(i);
            }
        }

        // Should never happen (range check above ensures at least one match)
        if (bestMatchIndex == -1)
        {
            return { -1, "Unknown", 0.0f, 0.0f };
        }

        // Calculate confidence based on distance
        float confidence = ConfidenceFromCents(minCentsDistance);

        // Calculate signed detune amount
        float detuneCents = CalculateCents(frequency, kStandardTuning[bestMatchIndex]);

        return { bestMatchIndex + 1, // 1-indexed string number
            std::string(kStringNames[bestMatchIndex]),
            confidence,
            detuneCents };
    }

} // namespace GuitarDiagnostics::Analysis::StringDetector
