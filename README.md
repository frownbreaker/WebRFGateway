# WebRFGateway

Web to 433mHz gateway ESP8266 

This project creates and RF Blaster web server running on an ESP8266. 

Make an HTTP request to a specific URL and the ESP8266 based module will send out 433mHz commands to control devices.

I’ve used it around the house to power on lights and AV equipment, and integrate functions into Kodi. 
E.g. spare buttons on my Kodi remote control lighting, gym equipment, AV kit etc.

Originally I purchase a Wemo switch and like that I could automate things but I did not like the proprietary nature of 
the device and the cost £30 each in the UK.

I had an AV room with TV, Amp, Subwoofer, centre speakers, front speaker, rear, etc. -all active so each individual 
speaker requires a dedicated main supply. I wanted the units off when not in use so I got some inexpensive remote control 
sockets and used these. This worked fine but after a while the remote broke

I wanted to automate things – ideally using my existing remote sockets - I purchased some cheap 433mHz transmitter / 
receiver boards 10 cost under $5 USD delivered,  wired a receiver to an Arduino Uno ran some code from the RCSwitch 
project and got the codes for my sockets.

This project allows me to playback these codes via a call to a web page: Issue the commands from Node-Red, 
Alexa (via Node-Red), smart phone using the great "HTTP shortcut app", and of course Kodi!
