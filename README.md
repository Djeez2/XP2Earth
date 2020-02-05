# XP2Earth

X-Plane plugin to connect to Google Earth

For license see https://github.com/Djeez2/XP2Earth LICENSE.

View the terrain in Google Earth while flying X-Plane.

Currently, this plugin is only available for Windows.

## Installation

- Extract the zip file in the X-Plane\Resources\plugins folder. Ensure that you don't accidentally create a double xp2earth\xp2earth folder.
- Install Google Earth Pro desktop application (free of charge, see its license)
- Install the Microsoft Visual C++ Runtime 2019 from https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads.


## One-time setup

- To get the best experience, experiment with the fly-to speed in Google Earth. (Menu: Tools -> Options -> Navigation -> Fly-To Speed). 
For me, the best setting is 3.5 to 4.0
- Select your viewing preferences. Some suggestions:
	* In View menu:
		- Show atmosphere
		- Hide grid
	* In Layers selection:
		- Uncheck everything, except
		- Check 3D buildings -> Photorealistic
		- Check Terrain


## Operating instructions

- In the X-Plane menu select Plugins -> XP2Earth -> Tracking to start tracking your position. Select again to stop tracking.
- Click XP2Earth -> Connect Earth to start Google Earth automatically (when not already started), and have it update its location.

Viewing options in the XP2Earth plugin menu:
- Pilot View: This sets the Google Earth eye point close to the ground. However, this leads to weird effects at some airports when the elevation in Google Earth is different from X-Plane's.
- Look Around: This allows you to look around in Google Earth by moving your head in the cockpit.

## Remote operation

You can run Google Earth on a second computer.
- In the X-Plane plugin start the tracking, but do NOT connect to Earth (as this would start a local Google Earth session)
- Copy the file xp2earth.kml to the second computer.
- Open the file with a text editor (e.g. Notepad) and change the text "localhost" to the name or IP-address of your X-Plane computer.
- In the Google Earth menu: File -> Open the updated xp2earth.kml file.


## Acknowledgements

This plugin uses ideas and code from
- https://www.fsdeveloper.com/forum/threads/ge-view-64-bit-httpx-v2-2-and-earthproxy-v1-3-fsx-thru-p3d-v4.442397/ EarthProxy, an FSX extension to connect Google Earth to FSX.
- https://github.com/cesanta/mongoose Mongoose, an embedded web server library provided by Cesanta.
- https://github.com/Stark2x/xp2kml xp2kml, an X-Plane plugin to write kml-files provided by Stark2x.

In the spririt of these open source projects I also contribute this plugin to the open source community.

Thanks to
- Kees Houbolt for testing many early versions of the plugin.
- Google for providing Google Earth free of charge.
- Laminar Research for opening up X-Plane through a well-documented SDK.
- Microsoft for providing Visual Studio Community Edition free of charge.


## For developers

The source of this plugin is available on GitHub: https://github.com/Djeez2/XP2Earth

Please create pull requests for improvements. I especially would appreciate contributions for other platforms, 
as I have no experience building plugins for MacOS or Linux, nor am I able to test those.

The plugin was built with Visual Studio 2019 Community Edition. The project assumes the X-Plane SDK is located in D:\XPSDK.





