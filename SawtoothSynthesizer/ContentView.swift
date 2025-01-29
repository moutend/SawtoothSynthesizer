//
//  ContentView.swift
//  SawtoothSynthesizer
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

import SwiftUI

struct TouchPad: View {
  let onChange: () -> Void
  let onEnded: () -> Void

  @State private var isActive = false

  var touch: some Gesture {
    DragGesture(minimumDistance: 0.0, coordinateSpace: .local)
      .onChanged { gesture in
        if self.isActive {
          return
        }

        self.isActive = true
        self.onChange()
      }
      .onEnded { _ in
        self.isActive = false
        self.onEnded()
      }
  }
  var body: some View {
    Text(Image(systemName: "music.quarternote.3"))
      .font(.system(size: 64))
      .frame(width: 192, height: 192)
      .background(.indigo)
      .foregroundColor(.white)
      .clipShape(Circle())
      .gesture(self.touch)
      .accessibilityAddTraits(.allowsDirectInteraction)
      .accessibilityLabel("Play")
  }
}

struct ContentView: View {
  @EnvironmentObject var hostModel: AudioUnitHostModel

  var body: some View {
    VStack {
      TouchPad(
        onChange: {
          self.hostModel.noteOn(index: 0x45)
        },
        onEnded: {
          self.hostModel.noteOff(index: 0x45)
        })
      if let viewController = hostModel.viewModel.viewController {
        AUViewControllerUI(viewController: viewController)
          .frame(width: 256, height: 128)
          .padding()
      } else {
        Text(hostModel.viewModel.message)
          .padding()
      }
      if hostModel.viewModel.showMIDIContols {
        Text("MIDI Input: Enabled")
      }
    }
  }
}

struct ContentView_Previews: PreviewProvider {
  static var previews: some View {
    ContentView()
  }
}
