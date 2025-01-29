//
//  AudioUnitViewModel.swift
//  SawtoothSynthesizer
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

import AudioToolbox
import CoreAudioKit
import SwiftUI

struct AudioUnitViewModel {
  var showAudioControls: Bool = false
  var showMIDIContols: Bool = false
  var title: String = "-"
  var message: String = "No Audio Unit loaded.."
  var viewController: UIViewController?
}
