# WebRFGateway

Web to 433mHz gateway ESP8266 - Small, Simple, Fast, Reliable, Robust simple integration with Node-Red (Big timer, Amzon Alexa) bluetooth sensors based on ESP32 detectors (e.g. detect someones Mi band) PIRs via ESP8266 and embedded controller such as TV transponders. Also has support for IR sending, OTA updated, etc. [Hello Oleg]

This project creates and RF Blaster web server running on an ESP8266. 

Make an HTTP request to a specific URL and the ESP8266 based module will send out 433mHz commands to control devices.

e.g.
HTTP://RFHUB/AVSPEAKERSON
HTTP://RFHUB/AVSPEAKERSOFF
HTTP://RFHUB/HALLON
HTTP://RFHUB/HALLOFF
HTTP://RFHUB/OFFICELIGHTON
HTTP://RFHUB/OFFICEAMPOFF

Use it around the house to power on lights, AV equipment (Tvs, Amps, Speakers) and integrate functions into Kodi. 
E.g. spare buttons on Kodi remote used to control lighting, gym equipment, AV kit etc.

This project allows playback of Radio and IR codes via a call to a web page: Issue the commands from Node-Red, 
Alexa (via Node-Red), smart phone using the great "HTTP shortcut app", and of course Kodi!

There are lots of feaures added to the project, thanks to other open-source projects :)

-Fast Web Server
-Over the Air Updates
-mDNS support
-IRRemote
-RCSwitch
