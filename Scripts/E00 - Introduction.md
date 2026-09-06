# Episode 0 - Introduction

Hello! and welcome to the introduction to audio software development!
In this episode 0, I'm going to give a brief overview of the course,
as well as provide some justification for why I have designed it this
way. That way, I won't have to explain any of this in the middle of
the actual episodes.

## Who is this course for?

This course is designed for people who are familiar with the basic
concepts of programming in C++, and who have an interest in audio
programming.

## What is the purpose of this course?

The goal of this course is to give a practical understanding of DSP
and how to implement common audio programs. It is *not* going to be
a full course on digital audio theory from mathematical fundamentals.
I will, however, explain as much as is necessary to understand how to
implement the topics, and I will give resources with which you can dive
deeper into any individual topic.

## How will the course work?

In this course, we're going to be using the JUCE framework to build
VST3/AU/AAX plugins. I have many justifications for this, but the
main one is that this will provide a common cross-platform base that
everyone can work from. We won't have to worry about writing separate
code for VST3 on Windows vs. on Mac vs. AU vs. AAX, none of that.
In addition, VST plugins allow us a quick and easy way to test our
code on real audio sources instead of having to build everything
from scratch before we hear anything.

## How will the course be structured?

Episode 1 will be a brief introduction and setup. We're going to install
JUCE and get it set up, and by the end of the episode, we'll have a
fully functional hard-clipper plugin that you can drop into your DAW
and actually use.

Episodes 2 and 3 are a bit of a departure from the regular structure and
will focus on more DSP fundamentals, audio programming best practices,
and general plugin architecture respectively. I've chosen to dedicate
two episodes to this because these things are not intuitive in the
slightest, and later on, understanding these topics will give context
to why certain design decisions have been made.

The rest of the episodes will focus on various common audio programming
topics, including delays, filters, distortions, oscillators, MIDI control,
and Envelopes. For now those are the topics I plan on covering, but I
do hope to continue work on this in the future.

In these episodes, we'll be building generic classes for these topics
that can be used in JUCE plugins or in other audio applications! We'll
be using as little JUCE-exclusive stuff as much as possible. Think of
it as building a toolkit of effects and generators that you can then
cobble together into whatever audio application your heart desires.