//
//  SynthDSPKernel.hpp
//  Synth
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

#pragma once
#import "SinOscillator.h"
#import "Synth-Swift.h"
#import "SynthParameterAddresses.h"
#import <AudioToolbox/AudioToolbox.h>
#import <CoreMIDI/CoreMIDI.h>
#import <algorithm>
#import <span>
#import <vector>
/*
 SynthDSPKernel
 As a non-ObjC class, this is safe to use from render thread.
 */
class SynthDSPKernel {
public:
  void initialize(int channelCount, double inSampleRate) {
    mSampleRate = inSampleRate;
    mSinOsc = SinOscillator(inSampleRate);
  }
  void deInitialize() {}
  // MARK: - Bypass
  bool isBypassed() { return mBypassed; }
  void setBypass(bool shouldBypass) { mBypassed = shouldBypass; }
  // MARK: - Parameter Getter / Setter
  // Add a case for each parameter in SynthParameterAddresses.h
  void setParameter(AUParameterAddress address, AUValue value) {
    switch (address) {
    case SynthParameterAddress::gain:
      mGain = value;
      break;
    }
  }
  AUValue getParameter(AUParameterAddress address) {
    // Return the goal. It is not thread safe to return the ramping value.
    switch (address) {
    case SynthParameterAddress::gain:
      return (AUValue)mGain;
    default:
      return 0.f;
    }
  }
  // MARK: - Max Frames
  AUAudioFrameCount maximumFramesToRender() const { return mMaxFramesToRender; }
  void setMaximumFramesToRender(const AUAudioFrameCount &maxFrames) {
    mMaxFramesToRender = maxFrames;
  }
  // MARK: - Musical Context
  void setMusicalContextBlock(AUHostMusicalContextBlock contextBlock) {
    mMusicalContextBlock = contextBlock;
  }
  // MARK: - MIDI Protocol
  MIDIProtocolID AudioUnitMIDIProtocol() const { return kMIDIProtocol_2_0; }
  inline double MIDINoteToFrequency(int note) {
    constexpr auto kMiddleA = 440.0;
    return (kMiddleA / 32.0) * pow(2, ((note - 9) / 12.0));
  }
  // MARK: - Internal Process - This function does the core siginal processing.
  // Do your custom DSP here.
  void process(std::span<float *> outputBuffers,
               AUEventSampleTime bufferStartTime,
               AUAudioFrameCount frameCount) {
    if (mBypassed) {
      // Fill the 'outputBuffers' with silence
      for (UInt32 channel = 0; channel < outputBuffers.size(); ++channel) {
        std::fill_n(outputBuffers[channel], frameCount, 0.f);
      }
      return;
    }
    // Use this to get Musical context info from the Plugin Host,
    // Replace nullptr with &memberVariable according to the
    // AUHostMusicalContextBlock function signature
    if (mMusicalContextBlock) {
      mMusicalContextBlock(nullptr /* currentTempo */,
                           nullptr /* timeSignatureNumerator */,
                           nullptr /* timeSignatureDenominator */,
                           nullptr /* currentBeatPosition */,
                           nullptr /* sampleOffsetToNextBeat */,
                           nullptr /* currentMeasureDownbeatPosition */);
    }
    // Generate per sample dsp before assigning it to out
    for (UInt32 frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
      // Do your frame by frame dsp here...
      const auto sample = mSinOsc.process() * mGain;
      for (UInt32 channel = 0; channel < outputBuffers.size(); ++channel) {
        outputBuffers[channel][frameIndex] = sample;
      }
    }
  }
  void handleOneEvent(AUEventSampleTime now, AURenderEvent const *event) {
    switch (event->head.eventType) {
    case AURenderEventParameter: {
      handleParameterEvent(now, event->parameter);
      break;
    }
    case AURenderEventMIDIEventList: {
      handleMIDIEventList(now, &event->MIDIEventsList);
      break;
    }
    default:
      break;
    }
  }
  void handleParameterEvent(AUEventSampleTime now,
                            AUParameterEvent const &parameterEvent) {
    // Implement handling incoming Parameter events as needed
  }
  void handleMIDIEventList(AUEventSampleTime now,
                           AUMIDIEventList const *midiEvent) {
    auto visitor = [](void *context, MIDITimeStamp timeStamp,
                      MIDIUniversalMessage message) {
      auto thisObject = static_cast<SynthDSPKernel *>(context);
      switch (message.type) {
      case kMIDIMessageTypeChannelVoice2: {
        thisObject->handleMIDI2VoiceMessage(message);
      } break;
      default:
        break;
      }
    };
    MIDIEventListForEachEvent(&midiEvent->eventList, visitor, this);
  }
  void handleMIDI2VoiceMessage(const struct MIDIUniversalMessage &message) {
    const auto &note = message.channelVoice2.note;
    switch (message.channelVoice2.status) {
    case kMIDICVStatusNoteOff: {
      mSinOsc.noteOff(note.number);
    } break;
    case kMIDICVStatusNoteOn: {
      mSinOsc.noteOn(note.number);
    } break;
    default:
      break;
    }
  }
  // MARK: - Member Variables
  AUHostMusicalContextBlock mMusicalContextBlock;
  double mSampleRate = 44100.0;
  double mGain = 1.0;
  bool mBypassed = false;
  AUAudioFrameCount mMaxFramesToRender = 1024;
  SinOscillator mSinOsc;
};
