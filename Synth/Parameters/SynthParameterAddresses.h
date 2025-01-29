//
//  SynthParameterAddresses.h
//  Synth
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

#pragma once

#include <AudioToolbox/AUParameters.h>

#ifdef __cplusplus
namespace SynthParameterAddress {
#endif

typedef NS_ENUM(AUParameterAddress, SynthParameterAddress) {
    gain = 0
};

#ifdef __cplusplus
}
#endif
