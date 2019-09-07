# sketch_aug20b_som_remote4

Firmware used to build this project  
https://www.instructables.com/id/Remote-Volume-Control-for-old-stereo-amp/


[![image](https://user-images.githubusercontent.com/3372598/64479124-c9a06400-d188-11e9-8e63-e4e43545a2ac.png)
](https://www.instructables.com/id/Remote-Volume-Control-for-old-stereo-amp/)

I wrote the firmware in pieces that you may use if you want to make your own firmware:

Library for using PT2257 - Electronic Volume Controller IC:

https://github.com/victornpb/Evc_pt2257

Sketch with good pattern to use one or more remotes to execute actions:

https://github.com/victornpb/generic_remote_controlled_sketch

This arduino sketch provides software to drive 7-segments display with multiplexing, no external driver required to drive 1 or a few displays. No hardware interrupt required, asynchronous execution:

https://github.com/victornpb/ledDisp

Arduino example code to drive a 7-segments display, from a bitmap array:

https://github.com/victornpb/display7

Generate individual characters "font" for 7segment displays

https://victornpb.github.io/7segments/7segments-char.html

Animation Builder (for 7segment displays)

https://victornpb.github.io/7segments/7segments-anim.html

Other related stuff

Sketch example for running the generated code

https://github.com/victornpb/display7

Arduino sketch that provides multiplexing for a single or multiple 7 segments displays without extra hardware

https://github.com/victornpb/ledDisp
