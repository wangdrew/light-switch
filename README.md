# light-switch
Control your lights (and other smart home devices) with an internet-connected button board!

I'm lazy and I hate getting out of bed to turn off the lights. I bought a [Wink Hub](https://www.wink.com/products/wink-hub-2/) and some [Z-wave switches](http://amzn.to/2EEIUkB) which turn my lights into "smart" lights that I can control with my phone. Not having to get out of bed is a step in the right direction, but pulling out my phone, unlocking it, finding the app, and turning off the correct lights is still not convenient! And using a smart speaker such as Amazon Echo or Google Home isn't always great because your voice commands may disturb someone sleeping. 

So I'm taking it a step further by building an internet-connected button board using the [Spark Photon](https://www.particle.io/products/hardware/photon-wifi) that sits next to my bed. The Photon makes requests to the [Wink API](https://winkapiv2.docs.apiary.io) to manipulate the state of my smart home devices - such as controlling the lights. Now I can tap a button on the button board, and the Spark Photon inside makes an authenticated request to the Wink's servers which commands my Wink Hub to turn the lights turn off! This may be the most technologically complex solution to do something stupidly simple - but it works great! 

![light switch button board](media/imgcombine.jpg?raw=true)
## Hardware:
Some prerequisite hardware is required:
* Wink Hub 2 
* some "smart" devices to be controlled - Hue light bulbs, smart switches/outlets etc.
* Some compute resource (desktop computer, AWS, raspberry Pi etc.) capable of running Docker containers (more about this in a second)

For the button board, I opted for something with 4 buttons arranged in a 2x2 grid:
* [Spark Photon](http://amzn.to/2EEqdx6) - $19
* [2x2 button pad PCB](https://www.sparkfun.com/products/9277) - $5
* [2x2 button pad](https://www.sparkfun.com/products/7836) - $3
* [Some RGB LEDs to make it look cool](https://www.sparkfun.com/products/12986) - $3
* 3D printed case (~6m of 1.75mm PLA filament) - < $1
* 2 x M2x3mm and 4x M3x8mm screws- < $1

Cost of the button board: $32 (excluding all the smart home stuff above)

See [hardware/README.md](hardware/README.md) for assembly instructions and 3D printable STL files.

## Design:
There are 2 software components to this system:
* The Spark Photon .ino sketch which makes HTTP REST calls to a middleware server on the local network
* A middleware HTTP server written for Python 2.7 with [Falcon](https://falcon.readthedocs.io/en/stable/index.html) and [Gunicorn](http://gunicorn.org/) running in a docker container. Wraps the Photon's HTTP requests with Wink required authentication and forwards the request and response to/from the Wink API. 

## Limitations
I originally wanted the Spark Photon to communicate directly with the Wink API but this isn't possible because while the Spark Photon does support TCP/HTTP, there is no software support for SSL meaning it is unable to make HTTPS calls required by the Wink API. The middleware server wraps the Wink API and exposes an HTTP interface on the local network for the Spark Photon. It also handles Wink API authentication and refreshes the Wink access token. The local API it exposes is thus unencrypted and unauthenticated.

The middleware server requires you to first obtain an access token and refresh token from Wink's oAuth endpoint as the server doesn't host the oAuth flow required to exchange Wink user credentials and auth code for the access/refresh tokens. See below for steps

## Getting Started
1) Request and wait for [Wink API developer credentials](https://developer.wink.com/)
2) With the granted Client ID and Client Secret, exchange these for an authorization code by using a browser to navigate to:
```
https://api.wink.com/oauth2/authorize?client_id=$CLIENT_ID&redirect_uri=$REDIRECT_URI
```
3) In the browser, login with Wink user account credentials. The browser will redirect to `$REDIRECT_URI`. The authorization code will be appended as a URL query param. Copy and paste this.
4) cURL the Wink oAuth endpoint to retrieve access and refresh tokens
```
curl -X POST \
  https://api.wink.com/oauth2/token \
  -H 'Content-Type: application/json' \
  -d '{
  "client_secret": "'$CLIENT_SECRET'",
  "grant_type": "authorization_code",
  "code": "'$AUTH_CODE'"
}'
```
The response contains the access and refresh tokens.

5) Run the middleware server and specify your client secret, access and refresh tokens. Specify a port for the server to bind to. 8000 is default, I have mine specified as 4200.
```
docker pull wangdrew/lightswitch
docker run -e "CLIENT_ID=" -e "CLIENT_SECRET=" -e "ACCESS_TOKEN=" -e "REFRESH_TOKEN=" -p 8000:8000 -d lightswitch:latest
```

6) Use the [Web Particle IDE](https://build.particle.io/build), copy and paste the code from `/particle` into the web editor. You will need to change `serverHostname` and `serverPort` to be the hostname and port of the local middleware server. Flash the code onto the Spark Photon.

## Building the server:
To build a new docker image after modifying the server:
```
docker build -t lightswitch:latest .
```
