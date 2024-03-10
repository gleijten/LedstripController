MCU<H2>2-Channel LedstripController for Home assistant controlled by Arduino Giga R3 WiFi</H2>

I have been searching a long time for a ledstrip controller integrated in Home Assistan, which was not depending on a internet application.
(I just like to have all my home automation for myselve and not controlled or registered by external compagnies...) <BR>
So I decided to do some research and developed my own controller.<BR>

My requirements:
- integration to Home Assistant
- controlled by WiFi
- controlling 2 ledstrips individually or
- controlling 2 ledstrips at once

My first attempt was with an nodeMCU, but the problem was the lack of sufficient PWM ports. I needed 5 PWM ports for each ledstrip.<BR>
With the release of Arduino Giga R3 wifi, this problems were solved.<BR>

Second problem was the interfacing. My ledstrips operate on 24VDC. The Arduino PWM output signal is only 3.3V
After some research and testing a decided to use a fast switching MOSFET with a logic non-inverting MOSFET driver

With this 2 Channel Controller it is possible to manage 2 ledstrips at once for example to use at 2 sides of a corridor.<BR>
The integration in Home assistant let you control both ledstrips independing or in sync (CH2 follows CH1)

<img src="extras/HA dashboard.png">
