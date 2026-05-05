# Easy Prop Propeller Generator

Easy Prop is a GUI based propeller generator which uses simplified aerodynamics to generate blade geometry. It's mainly developed as an external tool to assist in CAD development of drone propellers. The program generates x,y,z txt files for the leading edge, trailing edge, as well as a user defined amount of airfoil cross sections which can be imported into CAD programs. Easy Prop also outputs a point cloud file which can be imported into tools such as MeshLab and be converted into a mesh.

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
* Steps
* Diameter
* Airspeed
* RPM
* Blade Count
* r/R Start
* Design AOA
* Number of Airfoils
* Airfoil Sub Divisions
* Chord Start
* Chord End
* Chord Power
* Tip Offset x
* Tip Offset y
* Centerline power
* Tip Smooth
* Tip Smooth Start
* Tip Smoothing
* Tip Smoothing Strength
* Rake
* Rake Location
* Rake Offset
* Rake Strength

#### Propeller Hub
* Generate Hub
* Generate Hub Blade
* Hub Diameter
* Hub Inner Diameter
* Hub Height
* Hub Chord Start
* Hub Curve Start
* Hub x Offset
* Hub y Offset


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
