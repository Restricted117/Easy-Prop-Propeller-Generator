#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<cmath>
#include<vector>
#include<string> 
#include<filesystem>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include"propeller.h"
#include"toml.hpp"



namespace Propeller
{
	int unit_selection{ Units::mm };            
	int unit_selection_previous{ Units::mm };
	int pitch_unit_selection{ Units::in };     
	bool units_changed{ false };

	int steps{ 200 };               
	int hub_steps{ 100 };       
	bool hub{ true };         
	bool hub_blade{ true };     
	float hub_diameter{ 15 };  
	float hub_diamter_inner{ 5 };  
	float hub_height{ 8 };  
	float hub_curve_start{ 0.1 }; 
	float c_hub0{ 10 };  
	float hub_x_offset{ 0 };  
	float hub_y_offset{ 0 };  

	float diameter{ 208 };  
	float diameter_meters{};
	float diameter_true{};
	float air_speed{ 27.7 };     
	float air_speed_meters{};    
	float rpm{ 8000 };        
	int num_blades{ 3 };  

	float c0{ 22 };        
	float ce{ 10 };        
	float c0_meters;          
	float ce_meters;
	float chord_power{ 1 };  
	float r0{ 0.28 };           
	float a{ 5 };            

	float center_line_x_end{ 5 };  
	float center_line_y_end{ 1};  
	float angle_offset{ 0 };  
	float center_line_power{ 2 };  

	bool tip_smooth{ true };  
	float tip_smooth_loc{ 0.7 };  
	float tip_smooth_amt{ 0.7 };  
	float tip_smooth_power{ 2 };  

	bool rake{ true };  
	float rake_loc{ 0.95 };  
	float rake_offset{ 3 };  
	float rake_power{ 2 };  

	float pitch_meters;
	float pitch;
	float max_re;
	float min_re;

	std::vector<float>				z_loc_meters(steps);
	std::vector<float>				z_loc(steps);
	std::vector<float>				r_R(steps);
	std::vector<float>				blade_angles(steps);
	std::vector<float>				chord_meters(steps);
	std::vector<float>				chord(steps);
	std::vector<float>				reynolds(steps);
	std::vector<float>				reynolds_min(steps);
	std::vector<float>				reynolds_max(steps);
	std::vector<std::vector<float>> leading_edge(steps);
	std::vector<std::vector<float>> center_line(steps);
	std::vector<std::vector<float>> trailing_edge(steps);
	std::vector<std::vector<float>> prop_point_cloud;

	std::string airfoil_file{ "airfoils/NACA_4412.txt" };
	int subdivisions{ 1 };
	int num_af{ 10 };  
	std::vector<std::vector<float>> airfoil_base;
	std::vector<std::vector<float>> airfoils;

	bool graphs{ true };  
	std::string polar_file{ "polars/xf-naca4412-il-50000-n5.csv" };
	std::vector<polars> polar_table;
	std::vector<float> thrust_curve;
	std::vector<float> thrust_curve_min;
	std::vector<float> thrust_curve_max;
	std::vector<float> torque_curve;
	std::vector<float> torque_curve_min;
	std::vector<float> torque_curve_max;
	std::vector<float> eff_curve;
	std::vector<float> eff_curve_min;
	std::vector<float> eff_curve_max;
	std::vector<float> rpm_curve;
	float max_v_meters;
	float min_v_meters;
	float max_v{ 36 }; 
	float min_v{ 18 };  
	float max_rpm{ 15000 };  
	float min_rpm{ 5000 };  
	float thrust;
	float torque;
	float power;
	float eff;

	std::vector<float>				z_loc_hub(hub_steps);
	std::vector<float>				r_R_hub(hub_steps);
	std::vector<float>				blade_angles_hub(hub_steps);
	std::vector<float>				chord_hub(hub_steps);
	std::vector<std::vector<float>> leading_edge_hub(hub_steps);
	std::vector<std::vector<float>> center_line_hub(hub_steps);
	std::vector<std::vector<float>> trailing_edge_hub(hub_steps);
	std::vector<std::vector<float>> hub_cyl;

	void reset()
	{
		unit_selection = Units::mm;
		unit_selection_previous = Units::mm ;

		steps = 200;
		diameter = 208;
		air_speed= 27.7;   
		rpm = 8000;    
		num_blades = 3;
		c0 = 22;           
		ce = 10;       
		chord_power = 1;
		r0 = 0.28;           
		center_line_x_end = 5;
		center_line_y_end =  1;
		center_line_power =  2;
		tip_smooth = true ;
		tip_smooth_loc = 0.7;
		tip_smooth_amt = 0.7;
		tip_smooth_power = 2;
		rake = true;
		rake_loc = 0.95;
		rake_offset = 3;
		rake_power = 2;

		hub = true;
		hub_blade = true;
		hub_diameter = 15;
		hub_diamter_inner = 5;
		hub_height = 8;
		hub_curve_start = 0.1;
		c_hub0 = 10;
		hub_x_offset = 0;
		hub_y_offset = 0;

		subdivisions = 1;
		num_af = 10;
		a = 5;

		max_v = 36;
		min_v = 18;
		max_rpm = 15000;
		min_rpm = 5000;

	}

	void unitSelection()
	{
		switch (unit_selection)
		{

		case Units::m:
		{
			Propeller::diameter_meters = Propeller::diameter;
			Propeller::c0_meters = Propeller::c0;
			Propeller::ce_meters = Propeller::ce;
			Propeller::air_speed_meters = (Propeller::air_speed / 3.6);
			Propeller::max_v_meters = Propeller::max_v / 3.6;
			Propeller::min_v_meters = Propeller::min_v / 3.6;
			break;
		}

		case Units::mm:
		{
			Propeller::diameter_meters = (Propeller::diameter * 0.001);
			Propeller::c0_meters = (Propeller::c0 * 0.001);
			Propeller::ce_meters = (Propeller::ce * 0.001);
			Propeller::air_speed_meters = (Propeller::air_speed / 3.6);
			Propeller::max_v_meters = Propeller::max_v / 3.6;
			Propeller::min_v_meters = Propeller::min_v / 3.6;
			break;
		}

		case Units::ft:
		{
			Propeller::diameter_meters = (Propeller::diameter * 0.3048);
			Propeller::c0_meters = (Propeller::c0 * 0.3048);
			Propeller::ce_meters = (Propeller::ce * 0.3048);
			Propeller::air_speed_meters = (Propeller::air_speed / 2.237);
			Propeller::max_v_meters = Propeller::max_v / 2.237;
			Propeller::min_v_meters = Propeller::min_v / 2.237;
			break;
		}

		case Units::in:
		{
			Propeller::diameter_meters = (Propeller::diameter * 0.0254);
			Propeller::c0_meters = (Propeller::c0 * 0.0254);
			Propeller::ce_meters = (Propeller::ce * 0.0254);
			Propeller::air_speed_meters = (Propeller::air_speed / 2.237);
			Propeller::max_v_meters = Propeller::max_v / 2.237;
			Propeller::min_v_meters = Propeller::min_v / 2.237;
			break;
		}

		default:
			break;
		}

	}

	void pitchUnitSelection()
	{
		switch (pitch_unit_selection)
		{

		case Units::m:
		{
			Propeller::pitch = Propeller::pitch_meters;
			break;
		}

		case Units::mm:
		{
			Propeller::pitch = (Propeller::pitch_meters * 1000);
			break;
		}

		case Units::ft:
		{
			Propeller::pitch = (Propeller::pitch_meters * 3.28084);
			break;
		}

		case Units::in:
		{
			Propeller::pitch = (Propeller::pitch_meters * 39.3701);
			break;
		}

		default:
			break;
		}
	}

	bool paramsChanged()
	{
		static int   p_unit_selection = unit_selection;
		static int   p_pitch_unit_selection = pitch_unit_selection;
		static int   p_steps = steps;
		static int   p_hub_steps = hub_steps;
		static bool  p_hub = hub;
		static bool  p_hub_blade = hub_blade;
		static float p_hub_diameter = hub_diameter;
		static float p_hub_diameter_inner = hub_diamter_inner;
		static float p_hub_height = hub_height;
		static float p_hub_curve_start = hub_curve_start;
		static float p_c_hub0 = c_hub0;
		static float p_hub_x_offset = hub_x_offset;
		static float p_hub_y_offset = hub_y_offset;
		static float p_diameter = diameter;
		static float p_air_speed = air_speed;
		static float p_rpm = rpm;
		static int   p_num_blades = num_blades;
		static float p_c0 = c0;
		static float p_ce = ce;
		static float p_chord_power = chord_power;
		static float p_r0 = r0;
		static float p_a = a;
		static float p_center_line_x_end = center_line_x_end;
		static float p_center_line_y_end = center_line_y_end;
		static float p_angle_offset = angle_offset;
		static float p_center_line_power = center_line_power;
		static bool  p_tip_smooth = tip_smooth;
		static float p_tip_smooth_loc = tip_smooth_loc;
		static float p_tip_smooth_amt = tip_smooth_amt;
		static float p_tip_smooth_power = tip_smooth_power;
		static bool  p_rake = rake;
		static float p_rake_loc = rake_loc;
		static float p_rake_offset = rake_offset;
		static float p_rake_power = rake_power;
		static std::string p_airfoil_file = airfoil_file;
		static std::string p_polar_file = polar_file;
		static int p_subdivisions = subdivisions;
		static int p_num_af = num_af;
		static float p_max_rpm = max_rpm;
		static float p_min_rpm = min_rpm;
		static float p_max_v = max_v;
		static float p_min_v = min_v;
		static bool p_graphs = graphs;

		bool changed =
			p_unit_selection != unit_selection ||
			p_pitch_unit_selection != pitch_unit_selection ||
			p_steps != steps ||
			p_hub_steps != hub_steps ||
			p_hub != hub ||
			p_hub_blade != hub_blade ||
			p_hub_diameter != hub_diameter ||
			p_hub_diameter_inner != hub_diamter_inner ||
			p_hub_height != hub_height ||
			p_hub_curve_start != hub_curve_start ||
			p_c_hub0 != c_hub0 ||
			p_hub_x_offset != hub_x_offset ||
			p_hub_y_offset != hub_y_offset ||
			p_diameter != diameter ||
			p_air_speed != air_speed ||
			p_rpm != rpm ||
			p_num_blades != num_blades ||
			p_c0 != c0 ||
			p_ce != ce ||
			p_chord_power != chord_power ||
			p_r0 != r0 ||
			p_a != a ||
			p_center_line_x_end != center_line_x_end ||
			p_center_line_y_end != center_line_y_end ||
			p_angle_offset != angle_offset ||
			p_center_line_power != center_line_power ||
			p_tip_smooth != tip_smooth ||
			p_tip_smooth_loc != tip_smooth_loc ||
			p_tip_smooth_amt != tip_smooth_amt ||
			p_tip_smooth_power != tip_smooth_power ||
			p_rake != rake ||
			p_rake_loc != rake_loc ||
			p_rake_offset != rake_offset ||
			p_rake_power != rake_power ||
			p_airfoil_file != airfoil_file ||
			p_polar_file != polar_file ||
			p_subdivisions != subdivisions ||
			p_num_af != num_af ||
			p_max_rpm != max_rpm ||
			p_min_rpm != min_rpm ||
			p_max_v != max_v ||
			p_min_v != min_v ||
			p_graphs != graphs;

		if (changed)
		{
			p_unit_selection = unit_selection;
			p_pitch_unit_selection = pitch_unit_selection;
			p_steps = steps;
			p_hub_steps = hub_steps;
			p_hub = hub;
			p_hub_blade = hub_blade;
			p_hub_diameter = hub_diameter;
			p_hub_diameter_inner = hub_diamter_inner;
			p_hub_height = hub_height;
			p_hub_curve_start = hub_curve_start;
			p_c_hub0 = c_hub0;
			p_hub_x_offset = hub_x_offset;
			p_hub_y_offset = hub_y_offset;
			p_diameter = diameter;
			p_air_speed = air_speed;
			p_rpm = rpm;
			p_num_blades = num_blades;
			p_c0 = c0;
			p_ce = ce;
			p_chord_power = chord_power;
			p_r0 = r0;
			p_a = a;
			p_center_line_x_end = center_line_x_end;
			p_center_line_y_end = center_line_y_end;
			p_angle_offset = angle_offset;
			p_center_line_power = center_line_power;
			p_tip_smooth = tip_smooth;
			p_tip_smooth_loc = tip_smooth_loc;
			p_tip_smooth_amt = tip_smooth_amt;
			p_tip_smooth_power = tip_smooth_power;
			p_rake = rake;
			p_rake_loc = rake_loc;
			p_rake_offset = rake_offset;
			p_rake_power = rake_power;
			p_airfoil_file = airfoil_file;
			p_polar_file = polar_file;
			p_subdivisions = subdivisions;
			p_num_af = num_af;
			p_max_rpm = max_rpm;
			p_min_rpm = min_rpm;
			p_max_v = max_v;
			p_min_v = min_v;
			p_graphs = graphs;
		}

		return changed;
	}

	void unitsChanged()
	{
		bool changed = unit_selection_previous != unit_selection;
		units_changed = changed;

		if (changed)
		{
			switch (unit_selection_previous)
			{

			case Units::m:
			{
				diameter = diameter;
				c0 = c0;
				ce = ce;
				center_line_x_end = center_line_x_end;
				center_line_y_end = center_line_y_end;
				rake_offset = rake_offset;
				hub_diameter = hub_diameter;
				hub_diamter_inner = hub_diamter_inner;
				hub_height = hub_height;
				c_hub0 = c_hub0;
				hub_x_offset = hub_x_offset;
				hub_y_offset = hub_y_offset;
				air_speed = air_speed;
				max_v = max_v;
				min_v = min_v;
				break;
			}

			case Units::mm:
			{
				diameter = diameter * 0.001;
				c0 = c0 * 0.001;
				ce = ce * 0.001;
				center_line_x_end = center_line_x_end * 0.001;
				center_line_y_end = center_line_y_end * 0.001;
				rake_offset = rake_offset * 0.001;
				hub_diameter = hub_diameter * 0.001;
				hub_diamter_inner = hub_diamter_inner * 0.001;
				hub_height = hub_height * 0.001;
				c_hub0 = c_hub0 * 0.001;
				hub_x_offset = hub_x_offset * 0.001;
				hub_y_offset = hub_y_offset * 0.001;
				air_speed = air_speed;
				max_v = max_v;
				min_v = min_v;
				break;
			}

			case Units::ft:
			{
				diameter = diameter * 0.3048;
				c0 = c0 * 0.3048;
				ce = ce * 0.3048;
				center_line_x_end = center_line_x_end * 0.3048;
				center_line_y_end = center_line_y_end * 0.3048;
				rake_offset = rake_offset * 0.3048;
				hub_diameter = hub_diameter * 0.3048;
				hub_diamter_inner = hub_diamter_inner * 0.3048;
				hub_height = hub_height * 0.3048;
				c_hub0 = c_hub0 * 0.3048;
				hub_x_offset = hub_x_offset * 0.3048;
				hub_y_offset = hub_y_offset * 0.3048;
				air_speed = air_speed * 1.609;
				max_v = max_v * 1.609;
				min_v = min_v * 1.609;
				break;
			}

			case Units::in:
			{
				diameter = diameter * 0.0254;
				c0 = c0 * 0.0254;
				ce = ce * 0.0254;
				center_line_x_end = center_line_x_end * 0.0254;
				center_line_y_end = center_line_y_end * 0.0254;
				rake_offset = rake_offset * 0.0254;
				hub_diameter = hub_diameter * 0.0254;
				hub_diamter_inner = hub_diamter_inner * 0.0254;
				hub_height = hub_height * 0.0254;
				c_hub0 = c_hub0 * 0.0254;
				hub_x_offset = hub_x_offset * 0.0254;
				hub_y_offset = hub_y_offset * 0.0254;
				air_speed = air_speed * 1.609;
				max_v = max_v * 1.609;
				min_v = min_v * 1.609;
				break;
			}
			default:
				break;
			}


			switch (unit_selection)
			{

			case Units::m:
			{
				diameter = diameter;
				c0 = c0;
				ce = ce;
				center_line_x_end = center_line_x_end;
				center_line_y_end = center_line_y_end;
				rake_offset = rake_offset;
				hub_diameter = hub_diameter;
				hub_diamter_inner = hub_diamter_inner;
				hub_height = hub_height;
				c_hub0 = c_hub0;
				hub_x_offset = hub_x_offset;
				hub_y_offset = hub_y_offset;
				air_speed = air_speed;
				max_v = max_v;
				min_v = min_v;
				break;
			}

			case Units::mm:
			{
				diameter = diameter / 0.001;
				c0 = c0 / 0.001;
				ce = ce / 0.001;
				center_line_x_end = center_line_x_end / 0.001;
				center_line_y_end = center_line_y_end / 0.001;
				rake_offset = rake_offset / 0.001;
				hub_diameter = hub_diameter / 0.001;
				hub_diamter_inner = hub_diamter_inner / 0.001;
				hub_height = hub_height / 0.001;
				c_hub0 = c_hub0 / 0.001;
				hub_x_offset = hub_x_offset / 0.001;
				hub_y_offset = hub_y_offset / 0.001;
				air_speed = air_speed;
				max_v = max_v;
				min_v = min_v;
				break;
			}

			case Units::ft:
			{
				diameter = diameter / 0.3048;
				c0 = c0 / 0.3048;
				ce = ce / 0.3048;
				center_line_x_end = center_line_x_end / 0.3048;
				center_line_y_end = center_line_y_end / 0.3048;
				rake_offset = rake_offset / 0.3048;
				hub_diameter = hub_diameter / 0.3048;
				hub_diamter_inner = hub_diamter_inner / 0.3048;
				hub_height = hub_height / 0.3048;
				c_hub0 = c_hub0 / 0.3048;
				hub_x_offset = hub_x_offset / 0.3048;
				hub_y_offset = hub_y_offset / 0.3048;
				air_speed = air_speed / 1.609;
				max_v = max_v / 1.609;
				min_v = min_v / 1.609;
				break;
			}

			case Units::in:
			{
				diameter = diameter / 0.0254;
				c0 = c0 / 0.0254;
				ce = ce / 0.0254;
				center_line_x_end = center_line_x_end / 0.0254;
				center_line_y_end = center_line_y_end / 0.0254;
				rake_offset = rake_offset / 0.0254;
				hub_diameter = hub_diameter / 0.0254;
				hub_diamter_inner = hub_diamter_inner / 0.0254;
				hub_height = hub_height / 0.0254;
				c_hub0 = c_hub0 / 0.0254;
				hub_x_offset = hub_x_offset / 0.0254;
				hub_y_offset = hub_y_offset / 0.0254;
				air_speed = air_speed / 1.609;
				max_v = max_v / 1.609;
				min_v = min_v / 1.609;
				break;
			}
			default:
				break;
			}

			unit_selection_previous = unit_selection;

		}


	}

	void saveConfig(std::filesystem::path filename)
	{
		if (filename.extension() != ".toml")
		{
			filename = filename.string() + ".toml";
		}
		else if (filename.extension() == ".toml")
		{
			filename.replace_extension("");
			filename = filename.string() + ".toml";
		}

		auto tbl = toml::table{
			{"Units", toml::table{			
				{"unit_selection", unit_selection},
				{"unit_selection_previous", unit_selection_previous},
				{"pitch_unit_selection", pitch_unit_selection}
			}},
			{"Blade", toml::table{
				{"steps", steps},
				{"diameter", diameter},
				{"airspeed", air_speed},
				{"rpm", rpm},
				{"blade_count", num_blades},
				{"r/R_start", r0},
				{"chord_start", c0},
				{"chord_end", ce},
				{"chord_power", chord_power},
				{"tip_offset_x", center_line_x_end},
				{"tip_offset_y", center_line_y_end},
				{"center_line_power", center_line_power},
				{"tip_smooth", tip_smooth},
				{"tip_smooth_location_r/R", tip_smooth_loc},
				{"tip_smoothing_amount", tip_smooth_amt},
				{"tip_smooth_power", tip_smooth_power},
				{"rake", rake},
				{"rake_location_r/R", rake_loc},
				{"rake_offset", rake_offset},
				{"rake_power", rake_power}
			}},
			{"Airfoil", toml::table{
				{"airfoil_file", airfoil_file},
				{"design_AOA", a},
				{"number_of_airfoils", num_af},
				{"af_subdivisions", subdivisions}
			}},
			{"Hub", toml::table{
				{"hub", hub},
				{"hub_blade", hub_blade},
				{"hub_diameter", hub_diameter},
				{"hub_inner_diameter", hub_diamter_inner},
				{"hub_height", hub_height},
				{"hub_curve_start", hub_curve_start},
				{"chord_hub_start", c_hub0},
				{"hub_x_offset", hub_x_offset},
				{"hub_y_offset", hub_y_offset}
			}},
			{"Graphs", toml::table{
				{"graphs", graphs},
				{"polar_file", polar_file},
				{"max_airspeed", max_v},
				{"min_airspeed", min_v},
				{"max_rpm", max_rpm},
				{"min_rpm", min_rpm}
			}}
		};

		std::ofstream outf(filename, std::ios::out);
		outf << tbl;
	}

	void openConfig(std::filesystem::path filename)
	{
		toml::table tbl;

		try
		{
			tbl = toml::parse_file(filename.string());

			subdivisions =			*tbl["Airfoil"]["af_subdivisions"].value<int>();
			airfoil_file =			*tbl["Airfoil"]["airfoil_file"].value<std::string>();
			a =						*tbl["Airfoil"]["design_AOA"].value<float>();
			num_af =				*tbl["Airfoil"]["number_of_airfoils"].value<int>();

			air_speed =				*tbl["Blade"]["airspeed"].value<float>();
			num_blades =			*tbl["Blade"]["blade_count"].value<int>();
			center_line_power =		*tbl["Blade"]["center_line_power"].value<float>();
			ce =					*tbl["Blade"]["chord_end"].value<float>();
			chord_power =			*tbl["Blade"]["chord_power"].value<float>();
			c0 =					*tbl["Blade"]["chord_start"].value<float>();
			diameter =				*tbl["Blade"]["diameter"].value<float>();
			r0 =					*tbl["Blade"]["r/R_start"].value<float>();
			rake =					*tbl["Blade"]["rake"].value<bool>();
			rake_loc =				*tbl["Blade"]["rake_location_r/R"].value<float>();
			rake_offset =			*tbl["Blade"]["rake_offset"].value<float>();
			rake_power =			*tbl["Blade"]["rake_power"].value<float>();
			rpm =					*tbl["Blade"]["rpm"].value<float>();
			steps =					*tbl["Blade"]["steps"].value<int>();
			center_line_x_end =		*tbl["Blade"]["tip_offset_x"].value<float>();
			center_line_y_end =		*tbl["Blade"]["tip_offset_y"].value<float>();
			tip_smooth =			*tbl["Blade"]["tip_smooth"].value<bool>();
			tip_smooth_loc =		*tbl["Blade"]["tip_smooth_location_r/R"].value<float>();
			tip_smooth_power =		*tbl["Blade"]["tip_smooth_power"].value<float>();
			tip_smooth_amt =		*tbl["Blade"]["tip_smoothing_amount"].value<float>();

			graphs =				*tbl["Graphs"]["graphs"].value<bool>();
			max_v =					*tbl["Graphs"]["max_airspeed"].value<float>();
			max_rpm =				*tbl["Graphs"]["max_rpm"].value<float>();
			min_v =					*tbl["Graphs"]["min_airspeed"].value<float>();
			min_rpm =				*tbl["Graphs"]["min_rpm"].value<float>();
			polar_file =			*tbl["Graphs"]["polar_file"].value<std::string>();

			c_hub0 =				*tbl["Hub"]["chord_hub_start"].value<float>();
			hub =					*tbl["Hub"]["hub"].value<bool>();
			hub_blade =				*tbl["Hub"]["hub_blade"].value<bool>();
			hub_curve_start =		*tbl["Hub"]["hub_curve_start"].value<float>();
			hub_diameter =			*tbl["Hub"]["hub_diameter"].value<float>();
			hub_height =			*tbl["Hub"]["hub_height"].value<float>();
			hub_diamter_inner =		*tbl["Hub"]["hub_inner_diameter"].value<float>();
			hub_x_offset =			*tbl["Hub"]["hub_x_offset"].value<float>();
			hub_y_offset =			*tbl["Hub"]["hub_y_offset"].value<float>();

			pitch_unit_selection =		*tbl["Units"]["pitch_unit_selection"].value<Units>();
			unit_selection =			*tbl["Units"]["unit_selection"].value<Units>();
			unit_selection_previous =	*tbl["Units"]["unit_selection_previous"].value<Units>();

		}
		catch (const toml::parse_error& err)
		{
			std::cerr << "Parsing failed:\n" << err << "\n";
		}
	}

}


void pointCuller(std::vector<std::vector<float>>& vec)
{
	float hub_d = Propeller::hub_diameter;
	vec.erase(std::remove_if(vec.begin(), vec.end(), 
		[hub_d](std::vector<float>& a) 
		{
			float d = (2 * sqrtf(powf(a[0], 2) + powf(a[2], 2)));
			return d < hub_d;
		})
		, vec.end());

}


void save2D_Vec(std::filesystem::path name, std::vector<std::vector<float>> vec)
{
	if (name.extension() != ".txt")
	{
		name = name.string() + ".txt";
	}
	else if (name.extension() == ".txt")
	{
		name.replace_extension("");
		name = name.string() + ".txt";
	}

	std::ofstream outf(name, std::ios::out);

	if (!outf)
		std::cout << "error\n";

	for (int i{ 0 }; i < vec.size(); ++i)
	{
		char text[255];
		snprintf(text, 255, "%f %f %f\n", vec[i][0], vec[i][1], vec[i][2]); //x, y, z

		outf << static_cast<std::string>(text);
	}
}


std::vector<std::vector<float>> append2D_Vec(std::vector<std::vector<float>>& hub, std::vector<std::vector<float>>& blade)
{
	std::vector<std::vector<float>> c;

	c.insert(c.end(), hub.begin(), hub.end());
	c.insert(c.end(), blade.begin(), blade.end());

	return c;
}


void getAirfoil(std::string filename, int subdivisions)
{
	Propeller::airfoil_base.clear();

	std::vector<float> airfoil_l;
	std::ifstream file_l{ filename };
	if (!file_l)
		std::cout << "error";
	std::string in{};
	int length{ 0 };


	while (std::getline(file_l, in))
	{
		++length;
	}
	file_l.close();

	std::vector<std::vector<float>> airfoil_base(length, std::vector<float>(3));
	std::vector<std::vector<float>> airfoil;
	std::ifstream file{ filename };

	while (std::getline(file, in))
	{
		float t{};
		std::stringstream ins{ in };

		while (ins >> t)
		{
			airfoil_l.push_back(t);
		}
	}
	file.close();

	int row{ -1 };


	for (int i{ 0 }; i <= length - 1; ++i)
	{
		for (int j{ 0 }; j <= 1; ++j)
		{
			++row;
			airfoil_base[i][j] = airfoil_l[row];
		}

		airfoil_base[i][2] = 0;
	}

	airfoil_base[length - 1][1] = airfoil_base[0][1];


	if (subdivisions > 1)
	{
		for (int i{ 0 }; i <= length - 2; ++i)
		{
			for (int j{ 0 }; j <= subdivisions; ++j)
			{
				float x = (((float(j) / float(subdivisions)) * (airfoil_base[i][0] - airfoil_base[i + 1][0])) + airfoil_base[i + 1][0]);
				float y = (((float(j) / float(subdivisions)) * (airfoil_base[i][1] - airfoil_base[i + 1][1])) + airfoil_base[i + 1][1]);
				airfoil.push_back(std::vector<float>{x, y, airfoil_base[i][2]});

			}
		}

	}

	Propeller::airfoil_base.clear();
	Propeller::airfoil_base = (subdivisions > 1 ? airfoil : airfoil_base);
}


void generateAirfoils()
{
	Propeller::airfoils.clear();

	if (Propeller::hub_blade == true)
	{
		int steps = (Propeller::steps + Propeller::hub_steps);
		std::vector<float> chord;
		std::vector<float> b;
		std::vector<std::vector<float>> le;

		for (int i{ 0 }; i < Propeller::hub_steps; ++i)
		{
			chord.push_back(Propeller::chord_hub[i]);
			b.push_back(Propeller::blade_angles_hub[i]);
			le.push_back(Propeller::leading_edge_hub[i]);
		}

		for (int i{ 0 }; i < Propeller::steps; ++i)
		{
			chord.push_back(Propeller::chord[i]);
			b.push_back(Propeller::blade_angles[i]);
			le.push_back(Propeller::leading_edge[i]);
		}


		for (int i{ 0 }; i < Propeller::num_af; ++i)
		{

			for (int j{ 0 }; j < Propeller::airfoil_base.size(); ++j)
			{
				int n = (float(i) / float(Propeller::num_af - 1)) * (float(steps - 1));

				float x = Propeller::airfoil_base[j][0] * chord[n];
				float y = Propeller::airfoil_base[j][1] * chord[n];
				float z = le[n][2];

				float rot_x = (std::cosf((2 * Propeller::PI) - b[n]) * x) - (std::sinf((2 * Propeller::PI) - b[n]) * y) + le[n][0];
				float rot_y = (std::cosf((2 * Propeller::PI) - b[n]) * y) + (std::sinf((2 * Propeller::PI) - b[n]) * x) + le[n][1];

				Propeller::airfoils.push_back(std::vector<float>{rot_x, rot_y, z});
			}
		}

	}
	else
	{
		for (int i{ 0 }; i < Propeller::num_af; ++i)
		{

			for (int j{ 0 }; j < Propeller::airfoil_base.size(); ++j)
			{
				int n = (float(i) / float(Propeller::num_af - 1)) * (float(Propeller::steps - 1));

				float x = Propeller::airfoil_base[j][0] * Propeller::chord[n];
				float y = Propeller::airfoil_base[j][1] * Propeller::chord[n];
				float z = Propeller::leading_edge[n][2];

				float rot_x = (std::cosf((2 * Propeller::PI) - Propeller::blade_angles[n]) * x) - (std::sinf((2 * Propeller::PI) - Propeller::blade_angles[n]) * y) + Propeller::leading_edge[n][0];
				float rot_y = (std::cosf((2 * Propeller::PI) - Propeller::blade_angles[n]) * y) + (std::sinf((2 * Propeller::PI) - Propeller::blade_angles[n]) * x) + Propeller::leading_edge[n][1];

				Propeller::airfoils.push_back(std::vector<float>{rot_x, rot_y, z});
			}
		}
	}

}


void getPolarTable(std::string filename)
{
	Propeller::polar_table.clear();
	int skip_lines{ 11 };
	int lines{ 0 };

	std::vector<std::string> cell;
	std::ifstream file{ filename };
	if (!file)
		std::cout << "error";
	std::string in{};
	int length{ 0 };


	while (std::getline(file, in))
	{
		++lines;
		if (lines > skip_lines)
		{
			std::stringstream input{ in };
			cell.push_back(in);
		}
	}

	for (auto line : cell)
	{
		std::string a;
		std::string cl;
		std::string cd;
		int num_commas{ 0 };


		for (int i{0}; i < line.length(); ++i)
		{
			if (num_commas > 3)
			{
				break;
			}
			else if (line[i] != ',' && num_commas == 0)
			{
				a = a + line[i];
			}
			else if (line[i] != ',' && num_commas == 1)
			{
				cl = cl + line[i];
			}
			else if (line[i] != ',' && num_commas == 2)
			{
				cd = cd + line[i];
			}
			else if (line[i] == ',')
			{
				++num_commas;
			}
		}
		
		Propeller::polars table;
		table.aoa = std::stof(a);
		table.cl = std::stof(cl);
		table.cd = std::stof(cd);
		Propeller::polar_table.push_back(table);
	}

	file.close();
}


float trapezodialRule(std::vector<float> x, std::vector<float> y)
{
	float xp{ 0 };
	int n = x.size();

	for (int i{ 0 }; i < n - 1; ++i)
	{
		xp += (((y[i] + y[i + 1]) / 2) * (x[i + 1] - x[i]));
	}

	return xp;
}


float findThrust(float v, float rpm)
{
	Propeller::polars polar = *std::lower_bound(Propeller::polar_table.begin(), Propeller::polar_table.end(), Propeller::a,
		[](Propeller::polars i, float j)
		{
			return i.aoa < j;
		});

	float gamma = std::atanf(polar.cd / polar.cl);
	std::vector<float> Tc(Propeller::steps);


	for (int i{ 0 }; i < Propeller::steps; ++i)
	{
		float phi = std::atanf(v / (2 * Propeller::PI * 0.5 * Propeller::diameter_meters * (rpm / 60)));
		float k = (polar.cl * Propeller::chord_meters[i]) / (std::sinf(phi) * std::sinf(phi) * std::cosf(gamma));
		Tc[i] = (k * std::cosf(phi + gamma));
	}

	float thrust = ((0.5 * Propeller::density * powf(Propeller::air_speed_meters, 2) * Propeller::num_blades) * trapezodialRule(Propeller::z_loc_meters, Tc));

	return thrust;
}


float findTorque(float v, float rpm)
{
	Propeller::polars polar = *std::lower_bound(Propeller::polar_table.begin(), Propeller::polar_table.end(), Propeller::a,
		[](Propeller::polars i, float j)
		{
			return i.aoa < j;
		});

	float gamma = std::atanf(polar.cd / polar.cl);
	std::vector<float> Qc(Propeller::steps);


	for (int i{ 0 }; i < Propeller::steps; ++i)
	{
		float phi = std::atanf(v / (2 * Propeller::PI * 0.5 * Propeller::diameter_meters * (rpm / 60)));
		float k = (polar.cl * Propeller::chord_meters[i]) / (std::sinf(phi) * std::sinf(phi) * std::cosf(gamma));
		Qc[i] = (k * 0.5 * Propeller::diameter_meters * std::sinf(phi + gamma));
	}

	float torque = ((0.5 * Propeller::density * powf(Propeller::air_speed_meters, 2) * Propeller::num_blades) * trapezodialRule(Propeller::z_loc_meters, Qc));

	return torque;
}


float findReynolds(float v, float rpm, float z, float c)
{
	float w = ((rpm / 60) * 2 * Propeller::PI * z);
	float v_i = sqrtf(powf(w, 2) + powf(v, 2));
	float re = (v_i * c) / Propeller::kinematic_visc;

	return re;
}


void generateThrustCurve()
{
	Propeller::rpm_curve.clear();
	Propeller::thrust_curve.clear();
	Propeller::thrust_curve_min.clear();
	Propeller::thrust_curve_max.clear();
	Propeller::torque_curve.clear();
	Propeller::torque_curve_min.clear();
	Propeller::torque_curve_max.clear();
	Propeller::eff_curve.clear();
	Propeller::eff_curve_min.clear();
	Propeller::eff_curve_max.clear();
	Propeller::reynolds.resize(Propeller::steps);
	Propeller::reynolds_min.resize(Propeller::steps);
	Propeller::reynolds_max.resize(Propeller::steps);

	if (Propeller::unit_selection == Propeller::Units::m || Propeller::unit_selection == Propeller::Units::mm)
	{
		Propeller::thrust = findThrust(Propeller::air_speed_meters, Propeller::rpm);
		Propeller::torque = findTorque(Propeller::air_speed_meters, Propeller::rpm);
	}
	else
	{
		Propeller::thrust = findThrust(Propeller::air_speed_meters, Propeller::rpm) / 4.448f;
		Propeller::torque = findTorque(Propeller::air_speed_meters, Propeller::rpm) / 1.355f;
	}

	Propeller::eff = (findThrust(Propeller::air_speed_meters, Propeller::rpm) * Propeller::air_speed_meters) / (2 * Propeller::PI * (Propeller::rpm / 60) * findTorque(Propeller::air_speed_meters, Propeller::rpm));

	for (int i{ 0 }; i < Propeller::thrust_curve_steps; ++i)
	{
		float rpm = ((((float(i) / float(Propeller::thrust_curve_steps - 1))) * (Propeller::max_rpm - Propeller::min_rpm)) + (Propeller::min_rpm));
		Propeller::rpm_curve.push_back(rpm);

		float t = findThrust(Propeller::air_speed_meters, rpm);
		Propeller::thrust_curve.push_back((Propeller::unit_selection == Propeller::Units::m || Propeller::unit_selection == Propeller::Units::mm) ? t : (t / 4.448f));
		float t_min = findThrust(Propeller::min_v_meters, rpm);
		Propeller::thrust_curve_min.push_back((Propeller::unit_selection == Propeller::Units::m || Propeller::unit_selection == Propeller::Units::mm) ? t_min : (t_min / 4.448f));
		float t_max = findThrust(Propeller::max_v_meters, rpm);
		Propeller::thrust_curve_max.push_back((Propeller::unit_selection == Propeller::Units::m || Propeller::unit_selection == Propeller::Units::mm) ? t_max : (t_max / 4.448f));

		float q = findTorque(Propeller::air_speed_meters, rpm);
		Propeller::torque_curve.push_back((Propeller::unit_selection == Propeller::Units::m || Propeller::unit_selection == Propeller::Units::mm) ? q : (q / 1.355f));
		float q_min = findTorque(Propeller::min_v_meters, rpm);
		Propeller::torque_curve_min.push_back((Propeller::unit_selection == Propeller::Units::m || Propeller::unit_selection == Propeller::Units::mm) ? q_min : (q_min / 1.355f));
		float q_max = findTorque(Propeller::max_v_meters, rpm);
		Propeller::torque_curve_max.push_back((Propeller::unit_selection == Propeller::Units::m || Propeller::unit_selection == Propeller::Units::mm) ? q_max : (q_max / 1.355f));

		float eff = (t * Propeller::air_speed_meters) / (2 * Propeller::PI * (rpm / 60) * q);
		Propeller::eff_curve.push_back(eff);
		float eff_min = (t_min * Propeller::min_v_meters) / (2 * Propeller::PI * (rpm / 60) * q_min);
		Propeller::eff_curve_min.push_back(eff_min);
		float eff_max = (t_max * Propeller::max_v_meters) / (2 * Propeller::PI * (rpm / 60) * q_max);
		Propeller::eff_curve_max.push_back(eff_max);
	}


	//re num
	for (int i{ 0 }; i < Propeller::r_R.size(); ++i)
	{
		Propeller::reynolds[i] = findReynolds(Propeller::air_speed_meters, Propeller::rpm, Propeller::z_loc_meters[i], Propeller::chord_meters[i]);
		Propeller::reynolds_min[i] = findReynolds(Propeller::air_speed_meters, Propeller::min_rpm, Propeller::z_loc_meters[i], Propeller::chord_meters[i]);
		Propeller::reynolds_max[i] = findReynolds(Propeller::air_speed_meters, Propeller::max_rpm, Propeller::z_loc_meters[i], Propeller::chord_meters[i]);
	}

	Propeller::max_re = *std::max_element(Propeller::reynolds.begin(), Propeller::reynolds.end());
	Propeller::min_re = *std::min_element(Propeller::reynolds.begin(), Propeller::reynolds.end());
}


void saveAirfoils(int num_af, bool hub, std::string name)
{
	std::vector<std::vector<float>> airfoil;
	name = (name + "_airfoil");

	if (hub == true)
	{
		int steps = (Propeller::steps + Propeller::hub_steps);
		std::vector<float> chord;
		std::vector<float> b;
		std::vector<std::vector<float>> center_line;
		std::vector<std::vector<float>> le;

		for (int i{ 0 }; i < Propeller::hub_steps; ++i)
		{
			chord.push_back(Propeller::chord_hub[i]);
			b.push_back(Propeller::blade_angles_hub[i]);
			center_line.push_back(Propeller::center_line_hub[i]);
			le.push_back(Propeller::leading_edge_hub[i]);
		}

		for (int i{ 0 }; i < Propeller::steps; ++i)
		{
			chord.push_back(Propeller::chord[i]);
			b.push_back(Propeller::blade_angles[i]);
			center_line.push_back(Propeller::center_line[i]);
			le.push_back(Propeller::leading_edge[i]);
		}


		for (int i{ 0 }; i < num_af; ++i)
		{
			airfoil.clear();

			for (int j{ 0 }; j < Propeller::airfoil_base.size(); ++j)
			{
				int n = (float(i) / float(num_af - 1)) * (float(steps - 1));

				float x = Propeller::airfoil_base[j][0] * chord[n];
				float y = Propeller::airfoil_base[j][1] * chord[n];
				float z = center_line[n][2];

				float rot_x = (std::cosf((2 * Propeller::PI) - b[n]) * x) - (std::sinf((2 * Propeller::PI) - b[n]) * y) + le[n][0];
				float rot_y = (std::cosf((2 * Propeller::PI) - b[n]) * y) + (std::sinf((2 * Propeller::PI) - b[n]) * x) + le[n][1];

				airfoil.push_back(std::vector<float>{rot_x, rot_y, z});
			}

			save2D_Vec(static_cast<std::string>(name + "_" + std::to_string(i)), airfoil);
		}

	}
	else
	{
		for (int i{ 0 }; i < num_af; ++i)
		{
			airfoil.clear();

			for (int j{ 0 }; j < Propeller::airfoil_base.size() - 1; ++j)
			{
				int n = (float(i) / float(num_af - 1)) * (float(Propeller::steps - 1));

				float x = Propeller::airfoil_base[j][0] * Propeller::chord[n];
				float y = Propeller::airfoil_base[j][1] * Propeller::chord[n];
				float z = Propeller::center_line[n][2];

				float rot_x = (std::cosf((2 * Propeller::PI) - Propeller::blade_angles[n]) * x) - (std::sinf((2 * Propeller::PI) - Propeller::blade_angles[n]) * y) + Propeller::leading_edge[n][0];
				float rot_y = (std::cosf((2 * Propeller::PI) - Propeller::blade_angles[n]) * y) + (std::sinf((2 * Propeller::PI) - Propeller::blade_angles[n]) * x) + Propeller::leading_edge[n][1];

				airfoil.push_back(std::vector<float>{rot_x, rot_y, z});
			}

			save2D_Vec(static_cast<std::string>(name + "_" + std::to_string(i)), airfoil);
		}
	}

}


std::vector<std::vector<float>> generateBezier(std::vector<std::vector<float>> controlPoints,int steps = 100)
{
	std::vector<std::vector<float>> pts;

	if (controlPoints.size() < 2)
		return pts;

	pts.reserve(steps + 1);

	for (int i = 0; i <= steps; ++i)
	{
		float t = (float)i / (float)steps;

		// De Casteljau's algorithm — repeatedly lerp between
		// adjacent points until one point remains
		std::vector<std::vector<float>> level = controlPoints;

		while (level.size() > 1)
		{
			std::vector<std::vector<float>> next;
			next.reserve(level.size() - 1);

			for (int j = 0; j < (int)level.size() - 1; ++j)
			{
				float x = (1 - t) * level[j][0] + t * level[j + 1][0];
				float y = (1 - t) * level[j][1] + t * level[j + 1][1];
				float z = (1 - t) * level[j][2] + t * level[j + 1][2];
				next.push_back({ x, y, z });
			}

			level = std::move(next);
		}

		pts.push_back(level[0]);
	}

	return pts;
}


void generateBladeGeom()
{
	Propeller::diameter_true = Propeller::diameter;

	if (Propeller::center_line_x_end != 0)
	{
		float t = std::asinf(std::fabs(Propeller::center_line_x_end) / (Propeller::diameter * 0.5));
		Propeller::diameter_true = (Propeller::diameter * std::cosf(t));
	}

	Propeller::r_R.resize(Propeller::steps);
	Propeller::z_loc_meters.resize(Propeller::steps);
	Propeller::z_loc.resize(Propeller::steps);
	Propeller::blade_angles.resize(Propeller::steps);
	Propeller::chord_meters.resize(Propeller::steps);
	Propeller::chord.resize(Propeller::steps);
	Propeller::center_line.resize(Propeller::steps);
	Propeller::leading_edge.resize(Propeller::steps);
	Propeller::trailing_edge.resize(Propeller::steps);


	float pitch_w = ((Propeller::rpm * 2 * Propeller::PI * Propeller::diameter_meters * 0.75 * 0.5) / 60.0f);
	float pitch_b = ((std::atanf(Propeller::air_speed_meters / pitch_w)) + (Propeller::a * (Propeller::PI / 180.0f)));
	Propeller::pitch_meters = (Propeller::PI * Propeller::diameter_meters * 0.75 * std::tanf(pitch_b));

	for (int i{ 0 }; i < Propeller::r_R.size(); ++i)
	{
		float r_R = ((((1 / float(Propeller::steps - 1)) * float(i)) * (1 - Propeller::r0)) + (Propeller::r0));
		Propeller::r_R[i] = r_R;

		float z = ((Propeller::diameter_true * 0.5) * r_R);
		Propeller::z_loc[i] = z;

		float z_meters = ((Propeller::diameter_meters * 0.5) * r_R);
		Propeller::z_loc_meters[i] = z_meters;

		float w = ((Propeller::rpm * 2 * Propeller::PI * z_meters) / 60.0f);
		float b = ((std::atanf(Propeller::air_speed_meters / w)) + (Propeller::a * (Propeller::PI / 180.0f)));
		Propeller::blade_angles[i] = b;

		float c = (pow((float(i) / (Propeller::steps - 1)), Propeller::chord_power) * (Propeller::ce - Propeller::c0)) + Propeller::c0;
		Propeller::chord[i] = c;

		float c_meters = (pow((float(i) / (Propeller::steps - 1)), Propeller::chord_power) * (Propeller::ce_meters - Propeller::c0_meters)) + Propeller::c0_meters;
		Propeller::chord_meters[i] = c_meters;
	}


	if (Propeller::tip_smooth == true)
	{
		float ce_smooth = (Propeller::ce * Propeller::tip_smooth_amt);
		float ce_meters_smooth = (Propeller::ce_meters * Propeller::tip_smooth_amt);

		std::size_t offset = (Propeller::r_R.size() * Propeller::tip_smooth_loc);
		int new_steps = (Propeller::r_R.size() - offset);
		std::vector<float> c_copy = Propeller::chord;
		std::vector<float> c_meters_copy = Propeller::chord_meters;

		for (int i{ 0 }; i < new_steps; ++i)
		{
			float c = ((pow((float(i) / (new_steps - 1)), Propeller::tip_smooth_power) * (ce_smooth - c_copy[i + offset])) + c_copy[i + offset]);			
			Propeller::chord[i + offset] = c;

			float c_meters = ((pow((float(i) / (new_steps - 1)), Propeller::tip_smooth_power) * (ce_meters_smooth - c_meters_copy[i + offset])) + c_meters_copy[i + offset]);
			Propeller::chord_meters[i + offset] = c_meters;
		}

	}


	for (int i{ 0 }; i < Propeller::r_R.size(); ++i)
	{
		float cl_x = ((pow((float(i) / (Propeller::steps - 1)), Propeller::center_line_power)) * Propeller::center_line_x_end);
		float cl_y = ((pow((float(i) / (Propeller::steps - 1)), Propeller::center_line_power)) * Propeller::center_line_y_end);

		if (Propeller::hub_blade == true)
		{
			cl_x += Propeller::hub_x_offset;
			cl_y += Propeller::hub_y_offset;
		}

		Propeller::center_line[i] = std::vector<float>{ cl_x, cl_y, Propeller::z_loc[i] };

		float le_x = ((0.5 * Propeller::chord[i] * std::cosf((Propeller::PI - Propeller::blade_angles[i]))) + cl_x);
		float le_y = ((0.5 * Propeller::chord[i] * std::sinf((Propeller::PI - Propeller::blade_angles[i]))) + cl_y);
		Propeller::leading_edge[i] = std::vector<float>{le_x, le_y, Propeller::z_loc[i]};

		float te_x = ((-0.5 * Propeller::chord[i] * std::cosf((Propeller::PI - Propeller::blade_angles[i]))) + cl_x);
		float te_y = ((-0.5 * Propeller::chord[i] * std::sinf((Propeller::PI - Propeller::blade_angles[i]))) + cl_y);
		Propeller::trailing_edge[i] = std::vector<float>{te_x, te_y, Propeller::z_loc[i]};
	}


	if (Propeller::rake == true)
	{
		std::size_t offset_rake = (Propeller::r_R.size() * Propeller::rake_loc);
		int new_steps_r = (Propeller::r_R.size() - offset_rake);

		std::vector<std::vector<float>> centerline_copy = Propeller::center_line;

		for (int i{ 0 }; i < new_steps_r; ++i)
		{
			float cl_xr = (pow((float(i) / (new_steps_r - 1)), Propeller::rake_power) * ((Propeller::rake_offset + centerline_copy[Propeller::steps - 1][0]) - centerline_copy[offset_rake][0])) + centerline_copy[i + offset_rake][0];
			Propeller::center_line[i + offset_rake][0] = cl_xr;
		}


		for (int i{ 0 }; i < Propeller::r_R.size(); ++i)
		{
			float le_x = ((0.5 * Propeller::chord[i] * std::cosf((Propeller::PI - Propeller::blade_angles[i]))) + Propeller::center_line[i][0]);
			float le_y = ((0.5 * Propeller::chord[i] * std::sinf((Propeller::PI - Propeller::blade_angles[i]))) + Propeller::center_line[i][1]);
			Propeller::leading_edge[i] = std::vector<float>{ le_x, le_y, Propeller::center_line[i][2]};

			float te_x = ((-0.5 * Propeller::chord[i] * std::cosf((Propeller::PI - Propeller::blade_angles[i]))) + Propeller::center_line[i][0]);
			float te_y = ((-0.5 * Propeller::chord[i] * std::sinf((Propeller::PI - Propeller::blade_angles[i]))) + Propeller::center_line[i][1]);
			Propeller::trailing_edge[i] = std::vector<float>{ te_x, te_y, Propeller::center_line[i][2] };
		}
	}

}


void generateHub()
{
	Propeller::hub_steps = (Propeller::steps * Propeller::r0);

	Propeller::r_R_hub.resize(Propeller::hub_steps);
	Propeller::z_loc_hub.resize(Propeller::hub_steps);
	Propeller::blade_angles_hub.resize(Propeller::hub_steps);
	Propeller::chord_hub.resize(Propeller::hub_steps);
	Propeller::center_line_hub.resize(Propeller::hub_steps);
	Propeller::leading_edge_hub.resize(Propeller::hub_steps);
	Propeller::trailing_edge_hub.resize(Propeller::hub_steps);

	std::vector<std::vector<float>> te_copy = Propeller::trailing_edge_hub;
	std::vector<std::vector<float>> le_copy = Propeller::leading_edge_hub;
	std::vector<std::vector<float>> cl_copy = Propeller::center_line_hub;



	for (int i{0}; i < Propeller::hub_steps; ++ i)
	{
		float r_R = ((((float(i + 1) / float(Propeller::hub_steps + 1))) * (-Propeller::r_R[0])) + Propeller::r_R[0]);
		Propeller::r_R_hub[i] = r_R;

		float z = ((Propeller::diameter_true * 0.5) * r_R);
		Propeller::z_loc_hub[i] = z;

		float z_meters = ((Propeller::diameter_meters * 0.5) * r_R);

		float w = ((Propeller::rpm * 2 * Propeller::PI * z_meters) / 60.0f);
		float b = ((std::atanf(Propeller::air_speed_meters / w)) + (Propeller::a * (Propeller::PI / 180.0f)));
		Propeller::blade_angles_hub[i] = b;

		Propeller::chord_hub[i] = Propeller::c0;
	}


	for (int i{ 0 }; i < Propeller::hub_steps; ++i)
	{
		float cl_x = ((((pow((float(i + 1) / (Propeller::hub_steps + 1)), Propeller::center_line_power)) * (Propeller::center_line[0][0] - Propeller::hub_x_offset))) + Propeller::hub_x_offset);
		float cl_y = ((((pow((float(i + 1) / (Propeller::hub_steps + 1)), Propeller::center_line_power)) * (Propeller::center_line[0][1] - Propeller::hub_y_offset))) + Propeller::hub_y_offset);
		cl_copy[i] = std::vector<float>{ cl_x, cl_y, Propeller::z_loc_hub[i] };

		float le_x = ((0.5 * Propeller::chord_hub[i] * std::cosf((Propeller::PI - Propeller::blade_angles_hub[i]))) + cl_x);
		float le_y = ((0.5 * Propeller::chord_hub[i] * std::sinf((Propeller::PI - Propeller::blade_angles_hub[i]))) + cl_y);
		le_copy[i] = std::vector<float>{ le_x, le_y, Propeller::z_loc_hub[i] };

		float te_x = ((-0.5 * Propeller::chord_hub[i] * std::cosf((Propeller::PI - Propeller::blade_angles_hub[i]))) + cl_x);
		float te_y = ((-0.5 * Propeller::chord_hub[i] * std::sinf((Propeller::PI - Propeller::blade_angles_hub[i]))) + cl_y);
		te_copy[i] = std::vector<float>{ te_x, te_y, Propeller::z_loc_hub[i] };
	}

	const int control_pt_steps{ 10 };
	std::vector < std::vector<float>> control_pts_le;
	std::vector < std::vector<float>> control_pts_te;
	std::vector < std::vector<float>> control_pts_center;



	for (int i{0}; i < control_pt_steps; ++i)
	{
		int n = ((float(i) / float(control_pt_steps)) * float(Propeller::hub_steps + 1));
		control_pts_le.push_back(le_copy[n]);
		control_pts_te.push_back(te_copy[n]);
		control_pts_center.push_back(cl_copy[n]);

	}

	control_pts_center[control_pt_steps - 1] = { 0, 0, 0 }; control_pts_center[control_pt_steps - 2] = { 0, 0, float(Propeller::diameter * 0.5 * Propeller::hub_curve_start) };
	control_pts_le[control_pt_steps - 1] = { -Propeller::c_hub0 * 0.5f, 0, 0 }; control_pts_le[control_pt_steps - 2] = { -Propeller::c_hub0 * 0.5f, 0, float(Propeller::diameter * 0.5 * Propeller::hub_curve_start) };
	control_pts_te[control_pt_steps - 1] = { Propeller::c_hub0 * 0.5f, 0, 0 }; control_pts_te[control_pt_steps - 2] = { Propeller::c_hub0 * 0.5f, 0, float(Propeller::diameter * 0.5 * Propeller::hub_curve_start) };


	Propeller::leading_edge_hub = generateBezier(control_pts_le, Propeller::hub_steps - 1);
	Propeller::trailing_edge_hub = generateBezier(control_pts_te, Propeller::hub_steps - 1);
	Propeller::center_line_hub = generateBezier(control_pts_center, Propeller::hub_steps - 1);


	for (int i{ 0 }; i < Propeller::hub_steps; ++i)
	{
		float width = std::fabs(Propeller::leading_edge_hub[i][0] - Propeller::trailing_edge_hub[i][0]);
		float height = std::fabs(Propeller::leading_edge_hub[i][1] - Propeller::trailing_edge_hub[i][1]);
		float b = std::atanf((height / width));

		Propeller::blade_angles_hub[i] = b;
		Propeller::chord_hub[i] = sqrtf(powf(width, 2) + powf(height, 2));
	}

	std::reverse(Propeller::r_R_hub.begin(), Propeller::r_R_hub.end());
	std::reverse(Propeller::z_loc_hub.begin(), Propeller::z_loc_hub.end());
	std::reverse(Propeller::blade_angles_hub.begin(), Propeller::blade_angles_hub.end());
	std::reverse(Propeller::chord_hub.begin(), Propeller::chord_hub.end());
	std::reverse(Propeller::leading_edge_hub.begin(), Propeller::leading_edge_hub.end());
	std::reverse(Propeller::trailing_edge_hub.begin(), Propeller::trailing_edge_hub.end());
	std::reverse(Propeller::center_line_hub.begin(), Propeller::center_line_hub.end());

}


std::vector<std::vector<float>> generateCircularPattern(std::vector<std::vector<float>>& vec, int n)
{
	int steps = (vec.size());
	std::vector<std::vector<float>> vec_cyl;

	for (int i{ 0 }; i < n; ++i)
	{
		for (int j{ 0 }; j < steps; ++j)
		{
			float x = vec[j][0];
			float y = vec[j][1];
			float z = vec[j][2];

			float r = sqrtf(powf(x, 2) + powf(z, 2));
			float spacing = (2 * Propeller::PI * (float(i) / float(n)));
			float t = (std::atan2f(z, x) + spacing);

			float x_new = (r * std::cos(t));
			float z_new = (r * std::sin(t));

			vec_cyl.push_back({ x_new, y, z_new });
		}
	}

	pointCuller(vec_cyl);

	return vec_cyl;
}


void generatePropPointCloud()
{
	Propeller::prop_point_cloud.clear();

	std::vector<std::vector<float>> le = generateCircularPattern(Propeller::leading_edge, Propeller::num_blades);
	std::vector<std::vector<float>> te = generateCircularPattern(Propeller::trailing_edge, Propeller::num_blades);
	std::vector<std::vector<float>> le_hub = generateCircularPattern(Propeller::leading_edge_hub, Propeller::num_blades);
	std::vector<std::vector<float>> te_hub = generateCircularPattern(Propeller::trailing_edge_hub, Propeller::num_blades);
	std::vector<std::vector<float>> af = generateCircularPattern(Propeller::airfoils, Propeller::num_blades);

	Propeller::prop_point_cloud.insert(Propeller::prop_point_cloud.end(), le.begin(), le.end());
	Propeller::prop_point_cloud.insert(Propeller::prop_point_cloud.end(), te.begin(), te.end());
	Propeller::prop_point_cloud.insert(Propeller::prop_point_cloud.end(), le_hub.begin(), le_hub.end());
	Propeller::prop_point_cloud.insert(Propeller::prop_point_cloud.end(), te_hub.begin(), te_hub.end());
	Propeller::prop_point_cloud.insert(Propeller::prop_point_cloud.end(), Propeller::hub_cyl.begin(), Propeller::hub_cyl.end());
	Propeller::prop_point_cloud.insert(Propeller::prop_point_cloud.end(), af.begin(), af.end());
}

