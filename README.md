# Falcon head tracker

A precise and responsive hands-free gaming experience.

## Usage



## Installation

### 1. Install dependencies
The falcon head tracker requires [python3](https://www.python.org/downloads/) to be installed on the host system.<br />
Use the package manager [pip](https://pip.pypa.io/en/stable/) to install the dependencies.
```bash
pip install pyautogui playsound
```
To configure the WiFi settings and update the firmware on the head tracking module you will need to download and install [Arduino IDE](https://www.arduino.cc/en/software)<br />

### 2. Update the head tracker firmware
Download the [latest version](https://github.com/littlepetal/studious-dollop/releases)<br />

Open ArdUDP.h with the Arduino IDE and change "SECRET_SSID" and "SECRET_PASS" to your WiFi network's name and password.

Connect the head tracker to your computer with the USB cable.

Under  Tools > Port  select the com port that the tracker is connected to.

Click upload and wait until complete.

### 3. Configuration
Open the main.py script in a text editor and set your monitor resolution [width, height].<br />

At this point you can customize any other configuration setting you want, but if it's your first time using the head tracker we recommend using the defaults.

### 4. Test
Switch the head tracker on. Once the LED stops blinking you may place it on your head.

Navigate to the directory containing the downloaded .py files and run the main.py script
```bash
py main.py
```

The script will search your local network for the head tracker. Once located, it will connect and assume control of your cursor.<br />

To close the program press 'q' or 'esc' on your keyboard. You can also terminate the script from the command line with 'ctrl+c'<br />

## License
[MIT](https://choosealicense.com/licenses/mit/)
