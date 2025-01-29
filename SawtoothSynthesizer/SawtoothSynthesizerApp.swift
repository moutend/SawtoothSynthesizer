//
//  SawtoothSynthesizerApp.swift
//  SawtoothSynthesizer
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

import SwiftUI

@main
struct SawtoothSynthesizerApp: App {
  @StateObject private var hostModel = AudioUnitHostModel()

  var body: some Scene {
    WindowGroup {
      ContentView()
        .environmentObject(self.hostModel)
    }
  }
}
