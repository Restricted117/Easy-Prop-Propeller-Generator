# Easy Prop Propeller Generator

Easy Prop is a GUI based propeller generator which uses simplified aerodynamics to generate blade geometry. It's mainly developed as an external tool to assist in CAD development of drone propellers. Easy Prop generates x,y,z txt files for the leading edge, trailing edge, as well as a user defined amount of airfoil cross sections which can be imported into CAD programs. Easy Prop also outputs a point cloud file which can be imported into tools such as MeshLab and be converted into a 3D mesh.

## Methodology

Easy Prop splits the propeller blade into two sections: the blade hub and the blade. The blade hub is a spline that connects the center of the prop to the start of the blade at a user defined r/R. The blade section is starts at a user defined r/R and ends at the prop tip (r/R = 1). The blade section is divided into a user defined amount of steps and uses blade element theory along with a constant user defined angle of attack in order to generate the blade geometry. This isn't an ideal way to generate a prop but, it's a good first order method.

## Importing Airfoils and Polar Files

Easy Prop comes included with airfoil and polar files which can be found in the airfoil and polars files. To import different airfoils you can use [airfoil tools](http://airfoiltools.com/) and copy the x and y points into a txt file. You can also get the polar files from [airfoil tools](http://airfoiltools.com/) and download them as a csv. The files can be placed anywhere but is recommended to place them in the included directories .

## Installation

Simply download EasyProp.exe from the release page, unzip it, and run the exe.

## Usage

It's recommended to use Easy Prop as a tool to assist in CAD modelling by importing the airfoils, leading edge, and traling edge and then creating the rest of the prop geometry yourself but, you can also use the generated point cloud to generate a 3D mesh.

### CAD example

### Parameter Definitions 

#### Propeller Blade
* <ins>Steps:</ins> The number of sections for the blade to be divided into.
* <ins>Diameter:</ins> Diameter of the prop. Units can be changed under the settings tab.
* <ins>Airspeed:</ins> Operating airspeed of prop.
* <ins>RPM:</ins> Rotations per minute.
* <ins>Blade Count:</ins> Number of blades to be displayed and used for prop stats calculations.
* <ins>r/R Start:</ins> Location of where to start blade generation where an r/R of 1 is the tip and 0 is the center.
* <ins>Design AOA:</ins> The angle of attack to constrain BET to. This value should be around close to peak cl/cd for the expected reynolds number.
* <ins>Number of Airfoils:</ins> Number of airfoils to display and save for the point cloud.
* <ins>Airfoil Sub Divisions:</ins> Number of times to sub divide the airfoil into. 1 is reccomended for CAD and more than 1 for point cloud.
* <ins>Chord Start:</ins> The chord width at r/R start.
* <ins>Chord End:</ins> The chord width at the tip.
* <ins>Chord Power:</ins> 1 = linear chord across the blade.
* <ins>Tip Offset x:</ins> How much to offset the centerline along the x axis.
* <ins>Tip Offset y:</ins> How much to offset the centerline along the y axis.
* <ins>Centerline power:</ins> 1 = linear centerline across the blade.
* <ins>Tip Smooth:</ins> Enables tip smoothing
* <ins>Tip Smooth Start:</ins> r/R at which to begin smoothing
* <ins>Tip Smoothing:</ins> amount to reduce end chord by. 1 = no smoothing
* <ins>Tip Smoothing Strength:</ins> 1 = smoothes linearly.
* <ins>Rake:</ins> Enables raked tip.
* <ins>Rake Location:</ins> r/R location to begin rake.
* <ins>Rake Offset:</ins> Amount to push the blade along y axis.
* <ins>Rake Strength:</ins> 1 = linear rake.

#### Propeller Hub
* <ins>Generate Hub:</ins> Generates the central hub which connects all the blades.
* <ins>Generate Hub Blade:</ins> Generates the hub blade section which connects the blade to the hub.
* <ins>Hub Diameter:</ins> Diameter of the central hub.
* <ins>Hub Inner Diameter:</ins> Diamter of the hole in the central hub.
* <ins>Hub Height:</ins> Height of the central hub.
* <ins>Hub Chord Start:</ins> The chord width at the center.
* <ins>Hub Curve Start:</ins> r/R end point for the control points to generate the hub blade spline.
* <ins>Hub x Offset:</ins> Amount to offset the hub blade along x axis.
* <ins>Hub y Offset:</ins> Amount to offset the hub blade along y axis.


## License

This project is licensed under the MIT License License - see the LICENSE.md file for details


## Acknowledgments

* [ImGui](https://github.com/ocornut/imgui)
* [ImPlot](https://github.com/epezent/implot)
* [ImGui-filebrowser](https://github.com/AirGuanZ/imgui-filebrowser)
* [GLFW](https://github.com/glfw/glfw)
* [GLEW](https://github.com/nigels-com/glew)
* [stb](https://github.com/nothings/stb)
* [toml++](https://github.com/marzer/tomlplusplus)

## Contact

Russtam Wafayee  
russtam.wafayee@live.com
