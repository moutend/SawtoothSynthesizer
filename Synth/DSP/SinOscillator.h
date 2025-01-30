//
//  SinOscillator.h
//  Synth
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

#pragma once
#include <cmath>
#include <numbers>

// Restrict note numbers outside the range of 0 to 127.
constexpr int NumberOfPianoKeys = 127;
constexpr double Pi2 = 2.0 * std::numbers::pi_v<double>;

class SinOscillator {
public:
  SinOscillator(double sampleRate = 44100.0) {
    mSampleRate = sampleRate;

    for (int i = 0; i < NumberOfPianoKeys; i++) {
      // The A4 note value is 69.
      const double frequency = 440.0 * std::pow(2.0, (i - 69) / 12.0);

      mDeltaOmegas[i] =
          frequency * 2.0 * std::numbers::pi_v<double> / sampleRate;
      mOmegas[i] = 0.0;
      mDeltaGates[i] = 0.0;
      mGates[i] = 0.0;
    }
  }
  void noteOn(int noteIndex) {
    if (noteIndex < 0 || noteIndex > NumberOfPianoKeys - 1) {
      return;
    }

    const double deltaGate = 100.0 / mSampleRate;

    mDeltaGates[noteIndex] = deltaGate;
  }
  void noteOff(int noteIndex) {
    if (noteIndex < 0 || noteIndex > NumberOfPianoKeys - 1) {
      return;
    }

    const double deltaGate = 100.0 / mSampleRate;
    mDeltaGates[noteIndex] = -deltaGate;
  }
  double process() {
    double signal = 0.0;

    for (int i = 0; i < NumberOfPianoKeys; i++) {
      const double sinVolume = double(i) / double(NumberOfPianoKeys);
      const double sinSignal = mGates[i] * std::sin(mOmegas[i]);
      const double sawVolume =
          double(NumberOfPianoKeys - i) / double(NumberOfPianoKeys);
      const double sawSignal =
          mGates[i] * (2.0 * (1.0 - mOmegas[i] / Pi2) - 1.0);

      signal += sinVolume * sinSignal;
      signal += sawVolume * sawSignal;

      mOmegas[i] += mDeltaOmegas[i];

      if (mOmegas[i] >= Pi2) {
        mOmegas[i] -= Pi2;
      }

      mGates[i] += mDeltaGates[i];

      if (mGates[i] >= 1.0) {
        mGates[i] = 1.0;
        mDeltaGates[i] = 0.0;
      }
      if (mGates[i] <= 0.0) {
        mGates[i] = 0.0;
        mDeltaGates[i] = 0.0;
      }
    }

    return signal;
  }

private:
  double mSampleRate = {0.0};
  double mDeltaOmegas[NumberOfPianoKeys];
  double mOmegas[NumberOfPianoKeys];
  double mGates[NumberOfPianoKeys];
  double mDeltaGates[NumberOfPianoKeys];
};
