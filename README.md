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
* Steps: The number of sections for the blade to be divided into.
* Diameter: Diameter of the prop. Units can be changed under the settings tab.
* Airspeed: Operating airspeed of prop.
* RPM: Rotations per minute.
* Blade Count: Number of blades to be displayed and used for prop stats calculations.
* r/R Start: Location of where to start blade generation where an r/R of 1 is the tip and 0 is the center.
* Design AOA: The angle of attack to constrain BET to. This value should be around close to peak cl/cd for the expected reynolds number.
* Number of Airfoils: Number of airfoils to display and save for the point cloud.
* Airfoil Sub Divisions: Number of times to sub divide the airfoil into. 1 is reccomended for CAD and more than 1 for point cloud.
* Chord Start: The chord width at r/R start.
* Chord End: The chord width at the tip.
* Chord Power: 1 = linear chord across the blade.
* Tip Offset x: How much to offset the centerline along the x axis.
* Tip Offset y: How much to offset the centerline along the y axis.
* Centerline power: 1 = linear centerline across the blade.
* Tip Smooth: Enables tip smoothing
* Tip Smooth Start: r/R at which to begin smoothing
* Tip Smoothing: amount to reduce end chord by. 1 = no smoothing
* Tip Smoothing Strength: 1 = smoothes linearly.
* Rake: Enables raked tip.
* Rake Location: r/R location to begin rake.
* Rake Offset: Amount to push the blade along y axis.
* Rake Strength: 1 = linear rake.

#### Propeller Hub
* Generate Hub: Generates the central hub which connects all the blades.
* Generate Hub Blade: Generates the hub blade section which connects the blade to the hub.
* Hub Diameter: Diameter of the central hub.
* Hub Inner Diameter: Diamter of the hole in the central hub.
* Hub Height: Height of the central hub.
* Hub Chord Start: The chord width at the center.
* Hub Curve Start: r/R end point for the control points to generate the hub blade spline.
* Hub x Offset: Amount to offset the hub blade along x axis.
* Hub y Offset: Amount to offset the hub blade along y axis.


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
