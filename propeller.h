#ifndef  PROPELLOR_H
#define PROPELLOR_H

#include<iostream>
#include<fstream>
#include<sstream>
#include<cmath>
#include<vector>
#include<string> 
#include<filesystem>
#include<algorithm>


namespace Propeller
{
	enum Units
	{
		mm,
		m,
		in,
		ft
	};

	constexpr float PI{ 3.14159265358979323846f };
	constexpr float kinematic_visc{ 1.5111e-5 };
	constexpr float density{ 1.225 };

	extern int unit_selection;
	extern int unit_selection_previous;
	extern bool units_changed;
	extern int pitch_unit_selection;
	extern int graph_unit_selection;
	extern int steps;
	const int max_steps{ 10000 };
	extern int hub_steps;
	extern bool hub;
	extern bool hub_blade;
	extern float hub_diameter;
	extern float hub_diamter_inner;
	extern float hub_height;
	extern float hub_curve_start;
	extern float c_hub0;
	extern float hub_x_offset;
	extern float hub_y_offset;

	extern float diameter;
	extern float diameter_meters;
	extern float diameter_true;
	extern float air_speed;    //design operating air speed
	extern float air_speed_meters;    // m/s
	extern float rpm;          //design rpm 
	extern int num_blades;

	extern float c0;           //chord start value
	extern float ce;           //chord end value
	extern float c0_meters;
	extern float ce_meters;
	extern float chord_power;
	extern float r0;           //blade start r/R (start generating prop at z = r0/R)
	extern float a;            //design blade aoa (assume constant across blade)

	extern float center_line_x_end;
	extern float center_line_y_end;
	extern float angle_offset;
	extern float center_line_power;

	extern bool tip_smooth;
	extern float tip_smooth_loc;
	extern float tip_smooth_amt;
	extern float tip_smooth_power;

	extern bool rake;
	extern float rake_loc;
	extern float rake_offset;
	extern float rake_power;

	extern float pitch_meters;
	extern float pitch;
	extern float max_re;
	extern float min_re;

	extern std::vector<float> z_loc_meters;
	extern std::vector<float> z_loc;
	extern std::vector<float> r_R;
	extern std::vector<float> blade_angles;
	extern std::vector<float> chord_meters;
	extern std::vector<float> chord;
	extern std::vector<float> reynolds;
	extern std::vector<float> reynolds_min;
	extern std::vector<float> reynolds_max;
	extern std::vector<std::vector<float>> leading_edge;
	extern std::vector<std::vector<float>> center_line;
	extern std::vector<std::vector<float>> trailing_edge;
	extern std::vector<std::vector<float>> prop_point_cloud;

	extern std::string airfoil_file;
	extern int subdivisions;
	extern int num_af;
	extern std::vector<std::vector<float>> airfoil_base;
	extern std::vector<std::vector<float>> airfoils;

	extern std::string polar_file;

	struct polars
	{
		float aoa;
		float cl;
		float cd;
	};

	extern bool graphs;
	constexpr int thrust_curve_steps{ 100 };
	extern std::vector<polars> polar_table;
	extern std::vector<float> thrust_curve;
	extern std::vector<float> thrust_curve_min;
	extern std::vector<float> thrust_curve_max;
	extern std::vector<float> torque_curve;
	extern std::vector<float> torque_curve_min;
	extern std::vector<float> torque_curve_max;
	extern std::vector<float> eff_curve;
	extern std::vector<float> eff_curve_min;
	extern std::vector<float> eff_curve_max;
	extern std::vector<float> rpm_curve;
	extern float max_v_meters;
	extern float min_v_meters;
	extern float max_v;
	extern float min_v;
	extern float max_rpm;
	extern float min_rpm;
	extern float thrust;
	extern float torque;
	extern float power;
	extern float eff;

	extern std::vector<float> z_loc_hub;
	extern std::vector<float> r_R_hub;
	extern std::vector<float> blade_angles_hub;
	extern std::vector<float> chord_hub;
	extern std::vector<std::vector<float>> leading_edge_hub;
	extern std::vector<std::vector<float>> center_line_hub;
	extern std::vector<std::vector<float>> trailing_edge_hub;
	extern std::vector<std::vector<float>> hub_cyl;


	//function for reseting the prop to the default example
	void reset();

	//for selecting units 
	void unitSelection();

	//for selecting the display units of pitch
	void pitchUnitSelection();

	//function used to determine if input parameters were changed to see if it should re calc the propeller
	bool paramsChanged();

	//function used to determine if units were changed to see if it should convert inputs to current selection
	void unitsChanged();

	//saves current prop config as a toml
	void saveConfig(std::filesystem::path filename);

	//opens and loads toml configs
	void openConfig(std::filesystem::path filename);
}


//saves a 2d vector as a txt file
void save2D_Vec(std::filesystem::path name, std::vector<std::vector<float>> vec);


//conjoins 2 vectors together
std::vector<std::vector<float>> append2D_Vec(std::vector<std::vector<float>>& hub, std::vector<std::vector<float>>& blade);


//deletes points that are inside of the central hub
void pointCuller(std::vector<std::vector<float>>& vec);


//opens airfoil file and sets the base airfoil vector
void getAirfoil(std::string filename, int subdivisions);


//generates airfoil sections for the prop
void generateAirfoils();


//opens polar files and gets cl and cd for current aoa
void getPolarTable(std::string filename);


//uses trapezodial rule to integrate along a vector given x and y points
float trapezodialRule(std::vector<float> x, std::vector<float> y);


//finds thrust for given airspeed v and rpm in newtons
float findThrust(float v, float rpm);


//finds torque for given airspeed v and rpm in newton meters
float findTorque(float v, float rpm);


//finds reynolds number for given airspeed c, rpm, radius z, and width c
float findReynolds(float v, float rpm, float z, float c);


//generates 3 thrust and torque curves for the displayed graphs
void generateThrustCurve();


//saves 5-10 airfoil cross sections as seperate txt files
void saveAirfoils(int num_af, bool hub, std::string name);


//generates a bezier spline for connecting the blade to the hub
std::vector<std::vector<float>> generateBezier(std::vector<std::vector<float>> controlPoints,int steps);


//generate blade angles, chord distribution, trailing edge, leading edge, and center line
void generateBladeGeom();


//generates the hub section of the blade
void generateHub();


//generates a circular pattern of a vector around the origin for n times
std::vector<std::vector<float>> generateCircularPattern(std::vector<std::vector<float>>& vec, int n);


//conjoins all points into a single vector for use as a point cloud
void generatePropPointCloud();


//struct used to convert vectors to c style arrays
struct VecToCarray
{
	float arr[Propeller::max_steps] = { 0 };

	VecToCarray(std::vector<float>& vec)
	{
		std::copy(vec.begin(), vec.end(), arr);
	}

	~VecToCarray() = default;

	VecToCarray& operator= (std::vector<float>& vec)
	{
		std::copy(vec.begin(), vec.end(), arr);

		return *this;
	}
};


#endif // ! PROPELLOR.H
