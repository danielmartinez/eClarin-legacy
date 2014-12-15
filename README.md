eClarin
=======
eClarin is an Open Source project that brings you the opportunity to build an electronic aragonese bagpipe. It is based on the Arduino platform, 3D printing models and the USB MIDI Firmware.

How to build the hardware
-------------------------
The hardware can be built by following the steps in the "howto build the eChanter" http://www.echanter.com/home/howto-build. eClarin is also based on it, it is a cheap, easy and absolutly great way to make an electronic chanter... as they say: if you can solder, you can make it!

Maybe you will find helpfull the 3D models of chanters included here, you can print them with a 3D printer, and use them for the case.

How to load the firmware
------------------------
This project runs on an Arduino board, so you can get the sketch from this source code and load to it.

Once you load the sketch on the board you can program the Moco LUFA Firmware, that allows the PC to recognize the Arduino board as a standard USB MIDI controller. You can find that firmware and instructions here http://morecatlab.akiba.coocan.jp/lab/index.php/aruino/midi-firmware-for-arduino-uno-moco/?lang=en

Be careful because if you want to re-program the Arduino board you must program the Arduino firmware before, then load the modifyed sketch, then program the Moco LUFA Firmware again.

I want to play it!
------------------
Sure! When you have built it and have programmed the arduino board, plug it in your computer, it must be recognized as an USB MIDI controller that allows you to talk MIDI commands with your prefered MIDI synth software (Mine is fluidsynth).

Why eClarin?
------------
Yeah, that's a good question. All of this stuff is eChanter based and I can only thank the author for that. In a little tribute to him and using the word that names this part of the bagpipe (clarin means chanter), I chose eClarin for the project's name.

Can I help?
-----------
Maybe. If you think you can help, then you can help! Contact me about your suggestions, codes, ideas of whatever else you want, I'm pretty sure we can make it better together.
