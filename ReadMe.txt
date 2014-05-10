
      CFugue MusicNote Library

-------------------------------------------------

This is the source code of CFugue MusicNote library. 

Download the latest version (latest dated) source code and build it to use CFugue in your own applications. 

CFugue uses the CMake build system. It has been tested succcesfully with VS2013 and GNU C++ 4.8

(Note: CFugue uses C++11x - so please ensure that you are using latest GNU C++ compiler (ver 4.8 and above) to succeed the build.)

For sample usage, check the testCFugueLib test application code (in the test/StaticLibTestApp directory). 

Non-windows platforms can use CFugue only as a satic library linked directly into their code, while Windows users has the option to use CFugue in shared DLL form also, which is mandatory for using CFugue from .Net applications (through P/Invoke or COM).


Making some sound on Linux Variants
-------------------------------------------------
Linux variants has many sound problems, and its not pretty straight forward to get the MIDI sound working. Here are few steps that can get you started.

Make sure you have the below packages / applications already installed before building CFugue:
	libasound2-dev
	qjackctl
	qsynth
	fluidsynth
	fluid-soundfont-gm
	vmpk
	timidity
	timidity-deamon

Typically you should be able to get them with Synaptic package manager or with apt-get commands, such as:
	 sudo apt-get install libasound2-dev

After installing the above packages, you may want to restart your machine so that the timidity-daemon can start working correctly.

Use qSynth to configure the sound fonts from the fluid-synth. Follow the below steps, if you have trouble. 		1. In the qsynth setup,
		a.) In the Audio tab, Select alsa as the audio driver
		b.) In the Sound Fonts tab, Click on Open and load the FluidR3_GM.sf2 file you find in the /usr/share/sounds/sf2/ directory
	Restart qSynth. In the vmpk, under Edit | connections menu
		a.) Check Enable Midi Thru on MIDI Output
		b.) Select Midi Through:0 for the input
		c.) Select Fluid Synth or Timidity:0 for the output

Now, when you press any key on the VMPK, you should be able to hear sound and also see the qSynth green light going on and off. 

In case of no sound yet, run qjackctl once, start it and stop it and then exit it. This may help refresh the drivers. After quitting the qjackctl, your vmpk may start making sounds. Also, in the vmpk, if you find timidity ports in the output selection, you can choose timidity:0 port (instead of fluid synth) as another option.

Next, when you run testCFugueLib, you can choose the 'vmpk' port, so that any MIDI notes produced by CFugue are handled by VMPK (and then routed to Timidity / Fluid Synth).


Copyright 
------------------------------------------------- 

This is a product of Cenacle Research India Private Limited, made available free of charge for personal and research use only. For commercial usage, please contact the author. 

CFugue is distributed with the hope that it will be useful. No warranty of what-so-ever is implied, including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

Documentation: http://gk.palem.in/CFugue.html

Copyright (C) 2009-2014 Cenacle Research India Private Limited

-------------------------------------------------
