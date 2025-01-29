//
//  SimplePlayEngine.swift
//  SawtoothSynthesizer
//
//  Created by Yoshiyuki Koyanagi on 2025/01/29.
//

import AVFoundation
import CoreAudioKit
import UIKit

extension AVAudioUnit {
  static fileprivate func findComponent(type: String, subType: String, manufacturer: String)
    -> AVAudioUnitComponent?
  {
    let description = AudioComponentDescription(
      componentType: type.fourCharCode!,
      componentSubType: subType.fourCharCode!,
      componentManufacturer: manufacturer.fourCharCode!,
      componentFlags: 0,
      componentFlagsMask: 0)
    return AVAudioUnitComponentManager.shared().components(matching: description).first
  }
  fileprivate func loadAudioUnitViewController(completion: @escaping (UIViewController?) -> Void) {
    auAudioUnit.requestViewController { viewController in
      DispatchQueue.main.async {
        completion(viewController)
      }
    }
  }
}

class SimplePlayEngine {
  private var avAudioUnit: AVAudioUnit?
  private let stateChangeQueue = DispatchQueue(
    label: "com.example.SawtoothSynthesizer.StateChangeQueue")
  private let engine = AVAudioEngine()
  private(set) var isPlaying = false
  private var scheduleMIDIEventListBlock: AUMIDIEventListBlock? = nil

  init() {
    setupMIDI()
  }
  private func setupMIDI() {
    if !MIDIManager.shared.setupPort(
      midiProtocol: MIDIProtocolID._2_0,
      receiveBlock: { [weak self] eventList, _ in
        if let scheduleMIDIEventListBlock = self?.scheduleMIDIEventListBlock {
          _ = scheduleMIDIEventListBlock(AUEventSampleTimeImmediate, 0, eventList)
        }
      })
    {
      fatalError("Failed to setup Core MIDI")
    }
  }
  func initComponent(
    type: String, subType: String, manufacturer: String,
    completion: @escaping (Result<Bool, Error>, UIViewController?) -> Void
  ) {
    self.reset()

    guard
      let component = AVAudioUnit.findComponent(
        type: type, subType: subType, manufacturer: manufacturer)
    else {
      fatalError(
        "Failed to find component with type: \(type), subtype: \(subType), manufacturer: \(manufacturer))"
      )
    }

    AVAudioUnit.instantiate(
      with: component.audioComponentDescription,
      options: .loadOutOfProcess
    ) { avAudioUnit, error in
      guard let au = avAudioUnit, error == nil else {
        completion(.failure(error!), nil)
        return
      }

      self.avAudioUnit = au
      self.connect(avAudioUnit: au)

      au.loadAudioUnitViewController { viewController in
        completion(.success(true), viewController)
      }
    }
  }
  private func setSessionActive(_ active: Bool) {
    do {
      let session = AVAudioSession.sharedInstance()

      try session.setCategory(.playback, mode: .default)
      try session.setActive(active)
    } catch {
      fatalError("Could not set Audio Session active \(active). error: \(error).")
    }
  }
  func startPlaying() {
    stateChangeQueue.sync {
      if !self.isPlaying {
        self.startPlayingInternal()
      }
    }
  }
  func stopPlaying() {
    stateChangeQueue.sync {
      if self.isPlaying {
        self.stopPlayingInternal()
      }
    }
  }
  private func startPlayingInternal() {
    self.setSessionActive(true)

    let hardwareFormat = self.engine.outputNode.outputFormat(forBus: 0)

    self.engine.connect(
      self.engine.mainMixerNode, to: self.engine.outputNode, format: hardwareFormat)
    self.engine.prepare()

    do {
      try self.engine.start()
    } catch {
      fatalError("Could not start engine. error: \(error).")
    }

    self.isPlaying = true
  }
  private func stopPlayingInternal() {
    self.engine.stop()
    self.isPlaying = false

    self.setSessionActive(false)
  }
  func reset() {
    connect(avAudioUnit: nil)
  }
  func connect(avAudioUnit: AVAudioUnit?, completion: @escaping (() -> Void) = {}) {
    guard let avAudioUnit = self.avAudioUnit else {
      return
    }

    self.engine.disconnectNodeInput(self.engine.mainMixerNode)
    self.engine.detach(avAudioUnit)

    let hardwareFormat = engine.outputNode.outputFormat(forBus: 0)
    let stereoFormat = AVAudioFormat(
      standardFormatWithSampleRate: hardwareFormat.sampleRate, channels: 2)

    self.engine.attach(avAudioUnit)
    self.engine.connect(avAudioUnit, to: self.engine.mainMixerNode, format: stereoFormat)
    self.scheduleMIDIEventListBlock = avAudioUnit.auAudioUnit.scheduleMIDIEventListBlock

    completion()
  }
  func sendMessage(message: [UInt8]) {
    guard let avAudioUnit = self.avAudioUnit else {
      return
    }
    guard let scheduleMIDIEventBlock = avAudioUnit.auAudioUnit.scheduleMIDIEventBlock else {
      return
    }

    // var message: [UInt8] = [0x90, 0x51, 0x64]

    message.withUnsafeBufferPointer { bufferPointer in
      guard let baseAddress = bufferPointer.baseAddress else {
        return
      }

      scheduleMIDIEventBlock(AUEventSampleTimeImmediate, 0, 3, baseAddress)
    }
  }
}
