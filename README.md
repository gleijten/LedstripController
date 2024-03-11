MCU<H2>2-Channel RGBWW ledstrip controller for home-assistant controlled by Arduino Giga R3 WiFi</H2>

I have been searching some time for a ledstrip controller integration in Home Assistant, which is not depending on an external app for registrations.<BR>
So I did some research and developed my own controller.<BR>

My requirements:
- integration to Home Assistant
- no need for external internet connections
- adjustable colours and brightness
- controlled by local WiFi
- controlling 2 ledstrips individually and/or both together with 1 device

<H3>Programming</H3>
I used the arduino-home-assistant integration from David Chryrzynski ( https://github.com/dawidchyrzynski/arduino-home-assistant)
and editted his great 'light' and 'switch' examples for my testing program on a Arduino Uno.<BR>


<H3>Hardware</H3>
With the release of Arduino Giga R3 wifi (12 PWM ports) the lack of sufficient PWM ports for controlling 2 strips from 1 device was solved<BR>
The HA integration was working without any problems on this new hardware device.<BR>

Second problem was the hardware interfacing. My ledstrips operate on 24VDC.<BR>
After some testing I decided to use a fast switching MOSFET (IRL-540N) to control the ledstrip brightness with PWM.
This MOSFET has a low power dissipation and high performance in low frequency applications (<100kHz)<BR>
Because the Arduino Giga PWM output signal is only 3.3V, you need a MOSFET driver for steering the gate-source of the MOSFET with at least 5V.<BR>
The TC4427CPA is a perfect logic non-inverting dual driver for this purpose.<BR>
<H5>(Note:  ground any unused driver inputs to avoid a floating state)</H5>
By reducing the 24V supply voltage to 9V with a voltage regulator (LM7809), I have both sufficient power for the Arduino's Vin and also for controlling the mosfet driver.<BR>


<img scr="extras/Schema.png"> 
<BR>

<img src="extras/HA dashboard.png">
