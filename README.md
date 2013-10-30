eClarin
=======
eClarin is a Open Source project that brings to you the opportunity to build an electronic aragonese bagpipe, it is based on Arduino platform, 3D printing models and USB MIDI Firmware.

How to build the hardware
-------------------------
The hardware can be built following the steps in the "howto build the eChanter" http://www.echanter.com/home/howto-build, eClarin is also based on it, it is a cheap, easy, and absolutly great way to make an electronic chanter... as they say: if you can solder, you can make it!.

Maybe you find helpfull the 3D models of chanters included here, you can print it with a 3D printer, and use it for case.

How to load the firmware
------------------------
This project run on an Arduino board, so you can get the sketch from this source code and load to it.

Once you load the sketch on the board you can program the Moco LUFA Firmware, that allows PC to recognize the Arduino board as a standard USB MIDI controller. You can find that firmware and instructions here http://morecatlab.akiba.coocan.jp/morecat_lab/MocoLUFA.html

Take care because if you want re-program the Arduino board you must program the Arduino firmware before, then load the modifyed sketch, then program Moco LUFA Firmware again.

I want to play it!
------------------
Sure!, when you was build it and program the arduino board, then plug it in your computer, they must recognize as USB MIDI controller that allows you to talk MIDI commands with your prefered MIDI synth software (Mine is fluidsynth).

Why eClarin?
------------
Yeah, that's a good question. All of this stuff is eChanter based and I can only thank the author for that, in a little tribute to him and using the word that we use to name this bagpipe part (clarin means chanter), I chose eClarin for project name.

Can I help with that?
---------------------
Maybe, if you think that you can help then you can help!, contact me about your suggestions, codes, ideas of whatever else you want, I'm sure that we can do more all together.
