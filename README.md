This project was created to fix the noisy vibrations on Valve Index "Knuckles" controllers caused by incorrect interpretation of the default value.
I encountered this issue in the game "Elven Assassin." While stringing the bow, an annoying buzzing sound is heard right next to your ear.
I am sure this problem exists not only in this game but also in others.

If you have encountered this issue in another game, please contact me so I can add it to this readme.

### Tested with:

* Valve index + knuckles
* Pico 4 over PICO Connect + knuckles
* Pico 4 over Virtual Desktop + knuckles

## How to install

0. Check the known issues below.
1. Go to [Releases](./releases) section
2. Download last installer
3. Install it

During install you can check debug option to see more output from layer. It will be stored in `%localappdata%\OpenXR-HapticsOverride` folder.

	
## How to remove
&nbsp;&nbsp;&nbsp;&nbsp;Just like any another windows app

## How to build
1. Clone repo
2. Open repo directory with Visual Studio 
3. Build (chose one):
	* Using Visual Studio IDE GUI:
		Right click on "CMakePresets.json" in Solution Explorer -> Run CMake Workflow
	* From VS command line:
	   `cmake --workflow --preset x64-release`
	 	

## Known Issues

* If you are using Virtual Desktop and have reinstalled it, you may experience game crashes. To fix this, simply reinstall OpenXR-HapticsOverride. This problem is caused by the incorrect order of layers.

