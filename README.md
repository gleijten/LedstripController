MCU<H2>2-Channel RGBWW ledstrip controller for home-assistant controlled by Arduino Giga R3 WiFi</H2>

I have been searching some time for a ledstrip controller integration in Home Assistant, which is not depending on an external app for registrations.
So I did some research and developed my own controller.<BR>

My requirements:
- integration to Home Assistant
- no need for external internet connections
- adjustable colours and brightness
- controlled by local WiFi
- controlling 2 ledstrips individually and/or both together with 1 device

<H3>Hardware</H3>
With the release of Arduino Giga R3 wifi (12 PWM ports) the lack of sufficient PWM ports for controlling 2 strips from 1 device was solved<BR>
The HA integration was working without any problems on this new hardware device.<BR>

Second problem was the hardware interfacing. My ledstrips operate on 24VDC.<BR>
After some testing I decided to use a fast switching MOSFET (IRL-540N) to control the ledstrip brightness with PWM.
This MOSFET has a low power dissipation and high performance.<BR>
Because the Arduino Giga PWM output signal is only 3.3V, you need a MOSFET driver for steering the gate-source of the MOSFET with at least 5V.<BR>
The TC4427CPA is a perfect logic non-inverting dual driver for this purpose.<BR>
<H5>(Note:  ground any unused driver inputs to avoid a floating state)</H5>
By reducing the 24V ledstrip supply voltage to 9V with a voltage regulator (LM7809), I have both sufficient power for the Arduino's Vin and also for controlling the mosfet driver.<BR>
Because of the power dissipation in the LM7809 it could be necessary to cool it down with a sufficiently large heat sink.<BR>
The drawing below shows the control of 1 colour of the LED strip. For the <a href="https://github.com/gleijten/LedstripController/blob/main/extras/Schema_cpl.png" target="_blank",toolbar="no", location="no", directories="no", status="no", menubar="yes", scrollbars="yes", resizable="yes", copyhistory="yes", width="400", height="400">complete schematic</a> and my PCB, <a href="https://github.com/gleijten/LedstripController/tree/main/DesignSpark" >click here</a>.<BR>

<BR>
<img src="extras/Schema.png"> 
<BR>

<H3>Programming</H3>
Before starting you need to have a MQTT server available in your network. I used the standard MQTT integration from Home Assistant, but any other will do fine.<BR>
https://www.home-assistant.io/integrations/mqtt/<BR>
<BR>
<BR>
I used the arduino-home-assistant integration from David Chryrzynski ( https://github.com/dawidchyrzynski/arduino-home-assistant)
and editted his great 'light' and 'switch' examples for my testing program on Arduino.<BR>
The code includes 2 channels, which can be controlled separately or simply together by switching Ch2 in sync with Ch1.<BR>

<BR>
<img src="extras/HA dashboard.png">
<BR>

I also included some DEBUG routines for testing purpose.<BR>
When you choose to change the driver into an inverting type (like the TC4426 or your own driver variation with NPN transistors) you can simlpy activate the "invert Function" to correct the Arduino output.<BR>
