# Sawtooth Synthesizer

A synthesizer app that plays a sawtooth wave, implemented as an Audio Unit v3 Extension.

## Requirements

- iOS 15.0 or later
- Xcode 14 or later

## Usage

### Using External Hardware

Connect a USB MIDI keyboard to your iPhone via the Camera Connection Kit. If you have an iPhone 15 or later, you can connect the MIDI keyboard directly using a USB-C cable.  
Once the app is launched, try pressing any key on the MIDI keyboard. You will hear the sawtooth wave sound.  
There is no limitation on the number of simultaneous notes, so you can press multiple keys at the same time to play chords.  
Velocity detection is not implemented, so the sound will be played at a constant volume regardless of how hard or soft you press the keys.

### Without External Hardware

Tap the note icon displayed in the center of the app to play an A4 (440 Hz) tone. The sound will stop when you release your finger.

## License

MIT
