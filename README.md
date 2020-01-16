# WEISS Ultrasound Tracker
LabVIEW program and compiled C++ executables that save ultrasound images captured from an Epiphan frame grabber as .avi files with synchronized probe position and orientation (6 DoF) in a corresponding .csv file. Using different tracking/video framerates is possible. Probe orientation is visualized in 3D, and previous datasets can be played back.

## Installation
1. Run Setup_com0com.exe as an administrator and allow installation to finish.
2. Open Setup (should be located in your start menu: [screenshot here](doc/com0com_setup_location.png)) and create two COM port pairs named COM1 and COM2 with emulated baud rates: [screenshot here](doc/create_pair.png) and press apply.
3. Open Setup Command Prompt (should be located below Setup in start menu) and enter the following commands:
```
list
change CNCA0 PortName=COM1
change CNCB0 PortName=COM2
```
4. Extract the USB drivers for the Epiphan frame grabber (epiphan-usb-driver.zip) and run setup.exe as administrator.
5. With the frame grabber plugged into your computer, open NI MAX and expand "Devices and Interfaces" to see if the Epiphan camera is present:
- If it is present as "cam1", the LabVIEW code does not need to be modified. 
- If it is present as "cam0" or any other type of camera, before running Main.vi, change the camera control in Connection Settings to the appropriate device name.
- If it is not present, try installing the Epiphan capture software located in EpiphanCaptureConfig.zip and see if the frame grabber is recognized by the computer.
6. Ensure there are two COM ports present in "Devices and Interfaces" (ASRL1::INSTR "COM1" and ASRL2::INSTR "COM2").

### OptiTrack Configuration
Ensure [Motive 2.1.2](https://www.optitrack.com/products/motive/) is installed in your C:\Program Files folder and that it can track two rigid bodies (probe and patient/phantom).

### Aurora Configuration
Install the [NDI Toolbox](https://support.ndigital.com/downloads.php) for your corresponding bitness of Windows. Connect the USB adapter for the NDI control box before installing the drivers. The default location of the USB drivers after installation is "C:\Program Files\Northern Digital Inc\ToolBox\USB Driver". When you first connect the NDI SCU or NDI Host USB Converter, an error message may appear. Ignore the message and complete the following steps to install the USB drivers:
1. Open Device Manager.
2. Under Other Devices, right click NDI Host USB Converter.
3. Select Update Driver Software... then select Browse my computer for driver software.
4. Select Program Files\Northern Digital Inc\ToolBox\USB Driver. Select Next.
5. In the Windows security dialog, select Install this driver software anyway option. The first set of drivers will install.
6. After the first set of drivers are installed, browse to Other Devices and right click USB Serial Port. Repeat steps 3-5. The second set of drivers will install.
7. The system will now appear to be connected through a virtual COM port. Note the COM port number it is present as: 
- If it is listed as COM4, the LabVIEW code does not need to be modified.
- If it is listed as any other COM port, before running Main.vi, change the Port control in Connection Settings to the appropriate COM port number.
