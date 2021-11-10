# CrashSync
Another sound mangler

Thank you for testing this plugin for me, but PLEASE, be careful. This is very much not finished software, so please make sure to put a limiter on your master or the track this plugin is on, or to have some kind of automute set up to protect your ears.

// ---------------------------
// What's the name mean?
// ---------------------------

It kind of makes sense based on how it works because this effect basically smashes your signal through a distortion circuit and syncs an oscillator with that distorted signal, but it's honestly just directly stolen from the guy that made the original circuit. It'll be changing at some point.

// ---------------------------
// What does the plugin do?
// ---------------------------

This plugin is based off of a circuit by John Hollis http://www.hollis.co.uk/john/circuits.html that turns almost any signal into a square wave, then uses that signal to reset the phase of an oscillator. In simple terms, it makes whatever you put into it sound like a screaming robot. The frequency of the oscillator creates a kind of resonant peak, which can be swept for a flanger-like sound.

// ---------------------------
// What do the parameters do?
// ---------------------------

All of these parameters will change, some to the point of being deleted altogether. It's very likely there will be more parameters in later builds. As of 0.1:

Osc Freq: changes the frequency of the oscillator. Frequencies at the far ends of the spectrum don't necessarily pick up the input as well. Sweeping the parameter will make a flanger/phaser effect.

Threshold: changes the level your input signal needs to drop below to reset the oscillator. If your signal is too low/silent, no sound will come out.

Gain: increases or decreases how loud your signal is before it enters the clipping part of the algorithm. If your signal is cutting out a lot but you don't want to lower the threshold, increase the gain.

Waveform: lets you select the waveform of the oscillator. (Triangle, Saw, Rounded Saw, Square, Wiggly Square, Sloped Square, Noise, Filtered Noise)

Input Mode: turns the envelope follower in between the clipping stage and the oscillator on (1) and off (0). 

Env Attack: adjusts the attack time of the envelope follower

Env Release: adjusts the release time of the envelope follower

Output Lvl: acts like the volume knob on a distortion pedal

Tone: cuts out high frequencies (left = lower cutoff = less high frequencies, right = higher cutoff = more high frequencies)

Oversample: runs the processing at 4x your samplerate, helping to reduce aliasing to some extent

Input Filter: Adjusts the cutoff of the resonant low pass filter at the input. Lower values on the slider will make your signal "smoother", meaning the oscillator will reset less and you'll get a more consistent sound. Higher values do the opposite.

Mix: Mixes the dry input signal with the distorted output signal

