MCU<H2>2-Channel RGBWW ledstrip controller for home-assistant controlled by Arduino Giga R3 WiFi</H2>

I have been searching a long time for a ledstrip controller integration in Home Assistant, which was not depending on a external app for registrations.<BR>
So I did some research and developed my own controller.<BR>

My requirements:
- integration to Home Assistant
- no need for external internet connections
- adjustable colours and brightness
- controlled by local WiFi
- controlling 2 ledstrips individually and/or both together with 1 device

I used the arduino-home-assistant integration from David Chryrzynski ( https://github.com/dawidchyrzynski/arduino-home-assistant)
and editted his great 'light'and 'switch' examples to my testing program on a Arduino Uno.<BR>
With the release of Arduino Giga R3 wifi (12 PWM ports) the lack of sufficient PWM ports for controlling 2 strips from 1 device was solved<BR>
The HA integration was working without any problems on this new hardware device.<BR>

Second problem was the hardware interfacing. My ledstrips operate on 24VDC.<BR>
After some testing I decided to use a fast switching MOSFET (IRL-540N) to control the ledstrip brightness with PWM.
This MOSFET has a low power dissipation and high performance in low frequency applications (<100kHz)<BR>
Because the Arduino Giga PWM output signal is only 3.3V, you need a MOSFET driver for steering the Gate-Source of the MOSFET with at least 5V.<BR>
The logic non-inverting 

With this 2 Channel Controller it is possible to manage 2 ledstrips at once for example to use at 2 sides of a corridor.<BR>
The integration in Home assistant let you control both ledstrips independing or in sync (CH2 follows CH1)

<img src="extras/HA dashboard.png">
