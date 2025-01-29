//
//  SynthAudioUnit.h
//  Synth
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

@interface SynthAudioUnit : AUAudioUnit
- (void)setupParameterTree:(AUParameterTree *)parameterTree;
@end
