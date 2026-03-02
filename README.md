# Intro to Plugin Development Course

In this course, you will learn how to get started **making
your own real-time audio plugins**! For the purpose of ease,
we will be using the JUCE framework, but the course has
been designed to teach concepts in a way that can be
*transferred to any platform*. By the end of the first semester,
which covers twelve episodes and culminates in a midterm project,
you will have enough knowledge to create a basic synthesizer
with a small collection of effects!

The course expects you are *familiar* with basic C++ syntax and
concepts before beginning, but **no digital signal processing
knowledge is needed in advance**.

## TODOs (Episodes 1-12)

Everything for E11 and E12

Classes

- E08 - HighOrderFilter.h

Plugins

- E06 - pdcEcho
- E08 - pdcFilter
- E09 - pdcDistortion

## Repo Layout

Each episode has its own directory.
The final product produced at the end of each episode will be available to
download there. Additionally, any resources mentioned in the episode will be
linked to in the `README.md` in the corresponding folder.

Templates and materials can be found in the `Templates_Materials` folder, and
will be used in various episodes throughout the course.
Be sure to keep these handy.

## Episodes

### Semester 1: E01-E12 & Midterm Project

**[E01 - Introduction:](/Episodes/E01-Introduction/)**
Learn how to start a new JUCE project and build your first working plugin:
a simple hard-clipper!

**[E02 - Nyquist and Sampling:](/Episodes/E02-Nyquist/)**
Learn the basics of Digital Signal Processing (DSP)
and essential concepts when designing audio processing algorithms.

**[E03 - Plugins: Dos and Do Nots:](/Episodes/E03-Plugins/)**
Learn the essentials of plugin structure and best practices
to not shoot yourself in the foot while making real-time audio algorithms.

**[E04 - Classes:](/Episodes/E04-Classes/)**
Learn how object-oriented classes can be designed and used in plugins,
by porting our hard-clipper into a class.

**[E05 - Ring Buffer and Delay Line:](/Episodes/E05-RingBuffer/)**
Learn how to create long-term delays through a circular buffer and delay line.

**[E06 - Echo:](/Episodes/E06-Echo/)**
Learn how to implement feedback and host (DAW) tempo-syncing
by creating a basic Echo delay plugin.

**[E07 - Filters Pt. I:](/Episodes/E07-Filters/)**
Learn about block diagrams and transfer functions, and how to translate them
into code by implementing an abstract Biquad class.

**[E08 - Filters Pt. II:](/Episodes/E08-Filters2/)**
Implement the Robert Bristow-Johnson Filter algorithms, as well as how to adapt
them into higher order filters.

**[E09 - Distortion:](/Episodes/E09-Distortion/)**
Create a Distortion abstract class to easily create distortions and hysteresis.
Additionally, learn the basic concepts of oversampling.

**[E10 - Oscillators:](/Episodes/E10-Oscillators/)**
Learn how to synthesize the basic shapes digitally, and how to prevent aliasing.

**[E11 - MIDI:](/Episodes/E11-MIDI/)**
Learn how to use MIDI input from the host. Use it to control the
pitch of an oscillator, and learn about monophony vs. ployphony

**[E12 - Envelopes:](/Episodes/E12-Envelopes/)**
Learn about generating ADSR modulation signals, and apply them to
our basic synthesizer.

**[Project 1 - Midterm:](/Projects/P1-MidtermProject/)**
Build a synthesizer plugin! Detailed description can be found in
the episode folder.
