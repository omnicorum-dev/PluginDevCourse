/*
  ==============================================================================
    midi_cursor.h

    Part of the Intro to Plugin Development Course
    https://github.com/omnicorum-dev/PluginDevCourse

    Copyright (c) 2026 Nicolas Russo
    SPDX-License-Identifier: MIT
  ==============================================================================
*/

#pragma once

#include "juce_audio_basics/juce_audio_basics.h"

class MidiCursor {
  public:
    MidiCursor(const juce::MidiBuffer &midi)
        : current(midi.begin()), end(midi.end()) {}

    bool hasEvent() const { return current != end; }

    const juce::MidiMessageMetadata event() const { return *current; }

    void advance() { ++current; }

  private:
    juce::MidiBufferIterator current;
    juce::MidiBufferIterator end;
};
