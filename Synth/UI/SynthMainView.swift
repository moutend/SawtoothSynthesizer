//
//  SynthMainView.swift
//  Synth
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

import SwiftUI

struct SynthMainView: View {
    var parameterTree: ObservableAUParameterGroup
    
    var body: some View {
        ParameterSlider(param: parameterTree.global.gain)
    }
}
