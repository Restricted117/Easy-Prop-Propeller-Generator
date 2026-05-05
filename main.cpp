// Requires C++20 

#include<iostream>
#include<fstream>
#include<sstream>
#include<cmath>
#include<vector>
#include<string> 
#include<filesystem>
#include<Windows.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include<imgui.h>
#include "imfilebrowser.h"
#include"implot/implot.h"
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>
#include<imgui_internal.h>
#include<imconfig.h>
#include "imgui_log_window.h"
#include<imstb_rectpack.h>
#include<imstb_textedit.h>
#include<imstb_truetype.h>

#include"point_renderer.h"
#include"propeller.h"


//for adding imgui style help markers/tooltips 
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}



int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//glfw setup
	if (!glfwInit()) return -1;

	const GLint WIDTH = 1200; 
	const GLint HEIGHT = 900;   //startup window size

	glfwWindowHint(GLFW_SCALE_TO_MONITOR, 1);
	GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "Easy Prop", nullptr, nullptr);

	if (!win) { glfwTerminate(); return -1; }
	glfwMakeContextCurrent(win);
	glfwSetMouseButtonCallback(win, Callbacks::mouseButton);

	int icon_w;
	int icon_h;
	int icon_chan;
	unsigned char* pixels = stbi_load("propeller_clipart.png", &icon_w, &icon_h, &icon_chan, 4);

	GLFWimage images[1];
	images[0].width = icon_w;
	images[0].height = icon_h;
	images[0].pixels = pixels;

	glfwSetWindowIcon(win, 1, images);

	glfwSetCursorPosCallback(win, Callbacks::cursorPos);
	glfwSetScrollCallback(win, Callbacks::scroll);

	// ImGui setup
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // docking branch	
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
	//io.ConfigDpiScaleViewports = true;
	//io.ConfigDpiScaleFonts = true;
	ImGui::StyleColorsDark();
	ImPlot::StyleColorsDark();
	
	
	ImGui_ImplGlfw_InitForOpenGL(win, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGuiFileBrowserFlags file_flags = ImGuiFileBrowserFlags_EditPathString;
	file_flags |= ImGuiFileBrowserFlags_ConfirmOnEnter;
	file_flags |= ImGuiFileBrowserFlags_CloseOnEsc;
	file_flags |= ImGuiFileBrowserFlags_EnterNewFilename;
	file_flags |= ImGuiFileBrowserFlags_CreateNewDir;

	ImGuiFileBrowserFlags af_file_flags{ file_flags };
	af_file_flags |= ImGuiFileBrowserFlags_Open;
	file_flags |= ImGuiFileBrowserFlags_SaveAs;

	ImGui::FileBrowser AirFoilFileDialog{ af_file_flags };
	AirFoilFileDialog.SetTitle("Open airfoil");
	AirFoilFileDialog.SetTypeFilters({ ".txt" });
	AirFoilFileDialog.SetDirectory("./airfoils");

	ImGui::FileBrowser AirFoilPolarFileDialog{ af_file_flags };
	AirFoilPolarFileDialog.SetTitle("Open airfoil polar");
	AirFoilPolarFileDialog.SetTypeFilters({ ".csv"});
	AirFoilPolarFileDialog.SetDirectory("./polars");

	ImGui::FileBrowser OpenConfigFileDialog{ af_file_flags };
	OpenConfigFileDialog.SetTitle("Open");
	OpenConfigFileDialog.SetTypeFilters({ ".toml" });

	ImGui::FileBrowser SaveConfigFileDialog{ file_flags };
	SaveConfigFileDialog.SetTitle("Save Config as");
	SaveConfigFileDialog.SetTypeFilters({ ".toml" });
	SaveConfigFileDialog.SetInputName(".toml");

	ImGui::FileBrowser SaveAllFileDialog{ file_flags };
	SaveAllFileDialog.SetTitle("Save all as");
	SaveAllFileDialog.SetTypeFilters({ ".txt", ".toml"});

	ImGui::FileBrowser BladeFileDialog{ file_flags };
	BladeFileDialog.SetTitle("Save blade as");
	BladeFileDialog.SetTypeFilters({ ".txt" });
	BladeFileDialog.SetInputName(".txt");

	ImGui::FileBrowser PointCloudFileDialog{ file_flags };
	PointCloudFileDialog.SetTitle("Save point cloud as");
	PointCloudFileDialog.SetTypeFilters({ ".txt" });
	PointCloudFileDialog.SetInputName(".txt");

	ImGuiLog log; //for adding text to window log

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(3.0f);

	//init glew
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "glew failed to init\n";
		glfwDestroyWindow(win);
		glfwTerminate();
		return 1;
	}

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 base_pos = viewport->Pos;
	static float background_color[3]{ 0.1, 0.1, 0.1 };
	static float linecolor[4]{ 1, 0, 0, 1 };
	static float def_color[]{ 0, 1, 0, 1 };

	static float center_color[4]{ 1, 0, 0, 1 };
	static float le_color[4]{ 0, 1, 0, 1 };
	static float te_color[4]{ 0, 0, 1, 1 };
	static float af_color[4]{ 1, 1, 1, 0.33 };
	static float hub_color[4]{ 0, 1, 1, 1 };
	static float hub_center_color[4]{ 1, 0.7, 0.7, 1 };
	static float hub_le_color[4]{ 0.7, 1, 0.7, 1 };
	static float hub_te_color[4]{ 0.7, 0.7, 1, 1 };

	static int shape{ 2 };
	static int num_points{ 200 };
	static int h_pts{ 10 };
	static int c_pts{ 50 };
	static float rad_cyl_in{ 0 };

	ImVec2 vMin1{ 0, 0 };
	ImVec2 vMax1{ 0, 0 };

	ImVec2 vMin2{ 0, 0 };
	ImVec2 vMax2{ 0, 0 };

	ImVec2 vMin3{0, 0};
	ImVec2 vMax3{0, 0};

	ImVec2 topLeft{0, 0};
	ImVec2 topRight{ 0, 0 };
	ImVec2 bottomRight{0, 0};

	int vp_width{ 0 };
	int vp_height{ 0 };

	static int x_r{ 0 };
	static int y_r{ 0 };

	static bool hide_grid{ false };
	static bool hide_rot_dir{ false };
	static bool homogenize_colors{ false };
	static bool popup{ false };
	static bool popup_save_all{ false };
	static bool include_hub{ false };
	static int num_af_to_save{ 5 };
	static bool render_prop{ true };

	static double fps{ 0 };
	static int frames{ 0 };

	Propeller::unitSelection();
	Propeller::hub_cyl.clear();
	Propeller::hub_cyl = Render::generateCylinder((Propeller::steps * 0.005) + 5, (Propeller::steps * 0.02) + 15, Propeller::hub_diameter, Propeller::hub_height, Propeller::hub_diamter_inner);
	generateBladeGeom();
	generateHub();
	getAirfoil(Propeller::airfoil_file, Propeller::subdivisions);
	generateAirfoils();
	generatePropPointCloud();
	Propeller::pitchUnitSelection();
	getPolarTable(Propeller::polar_file);
	generateThrustCurve();
	Camera::zoom = -3.0 * Propeller::diameter * 0.5;


	while (!glfwWindowShouldClose(win)) 
	{
		float xscale;
		float yscale;
		glfwGetWindowContentScale(win, &xscale, &yscale);
		ImGui::GetStyle().FontScaleMain = xscale;
		ImGui::GetStyle().FontScaleDpi = xscale;
		double t0 = glfwGetTime();
		++frames;

		glfwPollEvents();

		//disable camera movements when cursor not in render window
		if (io.MousePos.x >= topLeft.x && io.MousePos.x <= bottomRight.x && io.MousePos.y >= topLeft.y && io.MousePos.y <= bottomRight.y)
			Camera::in_render_win = true;
		else
			Camera::in_render_win = false;

		if (AirFoilFileDialog.IsOpened() == true || SaveAllFileDialog.IsOpened() == true || BladeFileDialog.IsOpened() == true || PointCloudFileDialog.IsOpened() == true || popup == true || popup_save_all == true)
			Camera::file_expl = true;
		else
			Camera::file_expl = false;

		int w, h;
		glfwGetFramebufferSize(win, &w, &h);
		glClearColor(background_color[0], background_color[1], background_color[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//implent imgui stuff here

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiViewportFlags vp_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		vp_flags |= ImGuiDockNodeFlags_NoUndocking;

		ImGuiSliderFlags slider_flags = ImGuiSliderFlags_AlwaysClamp;
		ImGuiWindowFlags popup_flags = ImGuiWindowFlags_NoDocking;
		popup_flags |= ImGuiWindowFlags_NoSavedSettings;
		popup_flags |= ImGuiWindowFlags_NoMove;
		popup_flags |= ImGuiWindowFlags_NoResize;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(0, viewport, vp_flags);

		if (AirFoilFileDialog.IsOpened() == true || SaveAllFileDialog.IsOpened() == true || BladeFileDialog.IsOpened() == true || PointCloudFileDialog.IsOpened() == true || popup == true || popup_save_all == true)
			Camera::file_expl = true;
		else
			Camera::file_expl = false;


		//save options pop up window
		if (popup == true || popup_save_all == true)
		{
			int popup_w;
			int popup_h;
			glfwGetWindowSize(win, &popup_w, &popup_h);

			int popup_x = ((popup_w * 0.5) - (popup_w * 0.25 * 0.5));
			int popup_y = ((popup_h * 0.5) - (popup_h * 0.25 * 0.5));

			ImGui::SetNextWindowSize(ImVec2(popup_w * 0.25, popup_h * 0.25));
			ImGui::SetNextWindowPos(ImVec2(popup_x, popup_y));
			ImGui::Begin("Save Blade as", NULL, popup_flags);
			ImGui::NewLine();
			ImGui::Text("Include hub section");
			ImGui::Text("of blade in save file?");
			ImGui::NewLine();

			ImGui::PushItemWidth(50 * xscale);
			ImGui::Checkbox("Include hub section", &include_hub);
			ImGui::DragInt("Number of Airfoils to Save", &num_af_to_save, NULL, 3, 10, NULL, slider_flags);
			ImGui::NewLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 0.4));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 1, 0, 0.8));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 1, 0, 0.25));

			if (ImGui::Button("  confirm  "))
			{
				if (popup == true)
				{
					popup = false;
					BladeFileDialog.Open();
				}
				else if (popup_save_all == true)
				{
					popup_save_all = false;
					SaveAllFileDialog.Open();
				}
			}

			ImGui::PopStyleColor(3);
			ImGui::End();
		}
		//save options end



		//window 1-----------------------------------------
		ImGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("Propeller Parameters", NULL, ImGuiWindowFlags_MenuBar);

		ImGui::TextColored(ImVec4(0, 1, 0, 0.6), "FPS: %.2f", fps);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File  "))
			{
				if (ImGui::BeginMenu("   Open   "))
				{
					if (ImGui::MenuItem("Open Config"))
					{
						OpenConfigFileDialog.Open();
					}

					if (ImGui::MenuItem("Open Airfoil file"))
					{
						AirFoilFileDialog.Open();
					}

					if (ImGui::MenuItem("Open Airfoil Polar file"))
					{
						AirFoilPolarFileDialog.Open();
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("   Save   "))
				{
					if (ImGui::MenuItem("Save Config"))
					{
						SaveConfigFileDialog.Open();
					}

					if (ImGui::MenuItem("Save All"))
					{
						popup_save_all = true;
					}

					if (ImGui::MenuItem("Save Blade"))
					{
						popup = true;
					}

					if (ImGui::MenuItem("Save Point Cloud"))
					{
						PointCloudFileDialog.Open();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings"))
			{
				if (ImGui::BeginMenu("Input Parameter and Graph Units"))
				{
					ImGui::RadioButton("meters", &Propeller::unit_selection, Propeller::Units::m); 
					ImGui::RadioButton("milimeters", &Propeller::unit_selection, Propeller::Units::mm); 
					ImGui::RadioButton("feet", &Propeller::unit_selection, Propeller::Units::ft); 
					ImGui::RadioButton("inches", &Propeller::unit_selection, Propeller::Units::in);
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Pitch Display Units"))
				{
					ImGui::RadioButton("meters", &Propeller::pitch_unit_selection, Propeller::Units::m);
					ImGui::RadioButton("milimeters", &Propeller::pitch_unit_selection, Propeller::Units::mm);
					ImGui::RadioButton("feet", &Propeller::pitch_unit_selection, Propeller::Units::ft);
					ImGui::RadioButton("inches", &Propeller::pitch_unit_selection, Propeller::Units::in);
					ImGui::EndMenu();
				}


				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	
		std::string display_len;
		std::string pitch_display_len;
		std::string display_vel;
		int max_d{};
		int max_v{};

		switch (Propeller::unit_selection)
		{

		case Propeller::Units::m:
		{
			display_len = "[m]";
			display_vel = "[km/h]";
			max_d = 10;
			max_v = 1200;
			break;
		}

		case Propeller::Units::mm:
		{
			display_len = "[mm]";
			display_vel = "[km/h]";
			max_d = 1000;
			max_v = 1200;
			break;
		}

		case Propeller::Units::ft:
		{
			display_len = "[ft]";
			display_vel = "[miles/h]";
			max_d = 50;
			max_v = 750;
			break;
		}

		case Propeller::Units::in:
		{
			display_len = "[in]";
			display_vel = "[miles/h]";
			max_d = 100;
			max_v = 750;
			break;
		}
		default:
			break;
		}

		switch (Propeller::pitch_unit_selection)
		{

		case Propeller::Units::m:
		{
			pitch_display_len = "m";
			break;
		}

		case Propeller::Units::mm:
		{
			pitch_display_len = "mm";
			break;
		}

		case Propeller::Units::ft:
		{
			pitch_display_len = "ft";
			break;
		}

		case Propeller::Units::in:
		{
			pitch_display_len = "in";
			break;
		}
		default:
			break;
		}


		if (ImGui::BeginTabBar("tab bar1"))
		{
			if (ImGui::BeginTabItem("Propeller blade"))
			{
				ImGui::PushItemWidth(125 * xscale);

				ImGui::Checkbox("Render Prop", &render_prop); ImGui::SameLine();
				HelpMarker("Recomend to uncheck for large step sizes or large amounts of airfoils to reduce lag. Prop files will still calculated if you chose to save.");

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.5));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 0, 0, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 0, 0, 0.25));

				if (ImGui::Button("Reset Camera"))
				{
					Camera::zoom = -3.0 * Propeller::diameter * 0.5;
					Camera::panX = 0.0f;
					Camera::panY = 0.0f;
					Camera::rotX = 20.0f;
					Camera::rotY = -45.0f;
				}
				ImGui::PopStyleColor(3);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::DragInt("Steps", &Propeller::steps, NULL, 200, Propeller::max_steps, NULL, slider_flags);
				ImGui::DragFloat("Diameter", &Propeller::diameter, NULL, 0.1, max_d, NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Airspeed", &Propeller::air_speed, NULL, 0.1, 1200, NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_vel.c_str());
				ImGui::DragFloat("RPM", &Propeller::rpm, NULL, 0.1, 50000, NULL, slider_flags);
				ImGui::SliderInt("Blade Count", &Propeller::num_blades, 1, 12);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::DragFloat("r/R Start", &Propeller::r0, NULL, 0.05, 1, NULL, slider_flags); ImGui::SameLine(); HelpMarker("r0/R");
				ImGui::DragFloat("Design AOA [deg]", &Propeller::a, NULL, 0, 45, NULL, slider_flags);
				ImGui::DragInt("Number of Airfoils", &Propeller::num_af, NULL, 3, 1000, NULL, slider_flags);
				ImGui::SliderInt("Airfoil Sub Divisions", &Propeller::subdivisions, 1, 10);
				ImGui::Separator();
				ImGui::NewLine();


				ImGui::DragFloat("Chord Start", &Propeller::c0, NULL, (Propeller::diameter * 0.02), (Propeller::diameter * 0.4), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Chord End", &Propeller::ce, NULL, (Propeller::diameter * 0.02), (Propeller::diameter * 0.4), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Chord Power", &Propeller::chord_power, NULL, 1, 6, NULL, slider_flags);
				ImGui::Separator();
				ImGui::NewLine();
				
				ImGui::DragFloat("Tip Offset x", &Propeller::center_line_x_end, NULL, -(Propeller::diameter * 0.1), (Propeller::diameter * 0.1), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Tip Offset y", &Propeller::center_line_y_end, NULL, -(Propeller::diameter * 0.1), (Propeller::diameter * 0.1), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Centerline Power", &Propeller::center_line_power, NULL, 1, 6, NULL, slider_flags);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::Checkbox("Tip Smooth", &Propeller::tip_smooth);
				ImGui::DragFloat("Tip Smooth Start [r/R]", &Propeller::tip_smooth_loc, NULL, Propeller::r0, 0.95, NULL, slider_flags);
				ImGui::DragFloat("Tip Smoothing", &Propeller::tip_smooth_amt, NULL, 0.1, 1, NULL, slider_flags);
				ImGui::DragFloat("Tip Smooth Strength", &Propeller::tip_smooth_power, NULL, 1, 6, NULL, slider_flags);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::Checkbox("Rake", &Propeller::rake);
				ImGui::DragFloat("Rake Location [r/R]", &Propeller::rake_loc, NULL, 0.9, 0.98, NULL, slider_flags);
				ImGui::DragFloat("Rake Offset", &Propeller::rake_offset, NULL, -(Propeller::diameter * 0.1), (Propeller::diameter * 0.1), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Rake Strength", &Propeller::rake_power, NULL, 1, 6, NULL, slider_flags);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::EndTabItem();

			}

			if (ImGui::BeginTabItem("Propeller hub"))
			{
				ImGui::PushItemWidth(125 * xscale);

				ImGui::Checkbox("Render Prop", &render_prop); ImGui::SameLine();
				HelpMarker("Recomend to uncheck for large step sizes or large amounts of airfoils to reduce lag. Prop files will still calculated if you chose to save.");
				
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.5));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 0, 0, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 0, 0, 0.25));

				if (ImGui::Button("Reset Camera"))
				{
					Camera::zoom = -3.0 * Propeller::diameter * 0.5;
					Camera::panX = 0.0f;
					Camera::panY = 0.0f;
					Camera::rotX = 20.0f;
					Camera::rotY = -45.0f;
				}
				ImGui::PopStyleColor(3);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::Checkbox("Generate Hub", &Propeller::hub);
				ImGui::Checkbox("Generate Hub Blade", &Propeller::hub_blade);
				ImGui::DragFloat("Hub Diameter", &Propeller::hub_diameter, NULL, (Propeller::diameter * 0.001), (Propeller::diameter * 0.2), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Hub Inner Diameter", &Propeller::hub_diamter_inner, NULL, (Propeller::diameter * 0.0005), (Propeller::hub_diameter * 0.9), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Hub Height", &Propeller::hub_height, NULL, (Propeller::diameter * 0.001), (Propeller::diameter * 0.2), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Hub Chord Start", &Propeller::c_hub0, NULL, (Propeller::diameter * 0.01), (Propeller::diameter * 0.2), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Hub Curve Start", &Propeller::hub_curve_start, NULL, 0.005, Propeller::r0 * 0.95, NULL, slider_flags);
				ImGui::DragFloat("Hub x offset", &Propeller::hub_x_offset, NULL, -(Propeller::diameter * 0.05), (Propeller::diameter * 0.05), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::DragFloat("Hub y offset", &Propeller::hub_y_offset, NULL, -(Propeller::diameter * 0.05), (Propeller::diameter * 0.05), NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_len.c_str());
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();


		ImVec2 vMin1 = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax1 = ImGui::GetWindowContentRegionMax();
		vMin1.x += ImGui::GetWindowPos().x;
		vMin1.y += ImGui::GetWindowPos().y;
		vMax1.x += ImGui::GetWindowPos().x;
		vMax1.y += ImGui::GetWindowPos().y;

		ImGui::End();
		//window 1 end---------------------------------



		//window2---------------------------------------
		ImGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 300), ImGuiCond_FirstUseEver);
		ImGui::Begin("log");

		ImVec2 vMin2 = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax2 = ImGui::GetWindowContentRegionMax();
		vMin2.x += ImGui::GetWindowPos().x;
		vMin2.y += ImGui::GetWindowPos().y;
		vMax2.x += ImGui::GetWindowPos().x;
		vMax2.y += ImGui::GetWindowPos().y;

		log.Draw("log");
		ImGui::End();
		//window2 end-----------------------------------------



		//window3
		ImGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("View");

		if (ImGui::BeginTabBar("tab bar2"))
		{
			if (ImGui::BeginTabItem("Propeller stats"))
			{
				std::string graph_display_units_thrust;
				std::string graph_display_units_torque;
				const char* graph_display_units_speed;


				if (Propeller::unit_selection == Propeller::Units::m || Propeller::unit_selection == Propeller::Units::mm)
				{
					graph_display_units_thrust = "[N]";
					graph_display_units_torque = "[Nm]";
					graph_display_units_speed = "[km/h]";

				}
				else
				{
					graph_display_units_thrust = "[lbf]";
					graph_display_units_torque = "[ft lb]";
					graph_display_units_speed = "[mph]";

				}

				char stats_text[255];

				ImGui::Checkbox("Generate Graphs", &Propeller::graphs); ImGui::SameLine();
				HelpMarker("Uncheck if you don't have polar files");
				ImGui::Separator();

				ImGui::Text("Propeller Pitch:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f %s", Propeller::pitch, pitch_display_len.c_str());
				ImGui::SameLine(); HelpMarker("units can be changed under propeller parameters -> settings");

				ImGui::Text("Blade Angle at r0/R:"); ImGui::SameLine();
				snprintf(stats_text, 255, "%.2f [deg]", (Propeller::blade_angles[0] * (180.0f / Propeller::PI)));
				ImGui::TextColored(ImVec4(0, 1, 0, 1), stats_text);

				ImGui::Text("Blade Angle at tip:"); ImGui::SameLine();
				snprintf(stats_text, 255, "%.2f [deg]", (Propeller::blade_angles.back() * (180.0f / Propeller::PI)));
				ImGui::TextColored(ImVec4(0, 1, 0, 1), stats_text);

				snprintf(stats_text, 255, "Max Reynolds Number at %.0f RPM and %.1f", Propeller::rpm, Propeller::air_speed);
				ImGui::Text(stats_text); ImGui::SameLine();
				ImGui::Text("%s:", display_vel.c_str()); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.1f", Propeller::max_re);

				snprintf(stats_text, 255, "Min Reynolds Number at %.0f RPM and %.1f", Propeller::rpm, Propeller::air_speed);
				ImGui::Text(stats_text); ImGui::SameLine();
				ImGui::Text("%s:", display_vel.c_str()); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.1f", Propeller::min_re);

				snprintf(stats_text, 255, "Thrust at %.0f RPM and %.1f", Propeller::rpm, Propeller::air_speed);
				ImGui::Text(stats_text); ImGui::SameLine();
				ImGui::Text("%s:", display_vel.c_str()); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f", Propeller::thrust); ImGui::SameLine(); ImGui::TextColored(ImVec4(0, 1, 0, 1), graph_display_units_thrust.c_str());

				snprintf(stats_text, 255, "Torque at %.0f RPM and %.1f", Propeller::rpm, Propeller::air_speed);
				ImGui::Text(stats_text); ImGui::SameLine();
				ImGui::Text("%s:", display_vel.c_str()); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f", Propeller::torque); ImGui::SameLine(); ImGui::TextColored(ImVec4(0, 1, 0, 1), graph_display_units_torque.c_str());

				snprintf(stats_text, 255, "Blade Efficiency at %.0f RPM %.1f", Propeller::rpm, Propeller::air_speed);
				ImGui::Text(stats_text); ImGui::SameLine();
				ImGui::Text("%s:", display_vel.c_str()); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.4f", Propeller::eff);

				ImGui::Separator();
				ImGui::NewLine();

				ImPlotFlags plot_flags = ImPlotFlags_Crosshairs;
				ImPlotAxisFlags axis_flags = ImPlotAxisFlags_AutoFit;

				std::unique_ptr<VecToCarray> r_R				= std::make_unique<VecToCarray>(Propeller::r_R);
				std::unique_ptr<VecToCarray> re					= std::make_unique<VecToCarray>(Propeller::reynolds);
				std::unique_ptr<VecToCarray> re_min				= std::make_unique<VecToCarray>(Propeller::reynolds_min);
				std::unique_ptr<VecToCarray> re_max				= std::make_unique<VecToCarray>(Propeller::reynolds_max);
				std::unique_ptr<VecToCarray> chord				= std::make_unique<VecToCarray>(Propeller::chord);
				std::unique_ptr<VecToCarray> rpm_curve			= std::make_unique<VecToCarray>(Propeller::rpm_curve);
				std::unique_ptr<VecToCarray> thrust_curve		= std::make_unique<VecToCarray>(Propeller::thrust_curve);
				std::unique_ptr<VecToCarray> thrust_curve_min	= std::make_unique<VecToCarray>(Propeller::thrust_curve_min);
				std::unique_ptr<VecToCarray> thrust_curve_max	= std::make_unique<VecToCarray>(Propeller::thrust_curve_max);
				std::unique_ptr<VecToCarray> torque_curve		= std::make_unique<VecToCarray>(Propeller::torque_curve);
				std::unique_ptr<VecToCarray> torque_curve_min	= std::make_unique<VecToCarray>(Propeller::torque_curve_min);
				std::unique_ptr<VecToCarray> torque_curve_max	= std::make_unique<VecToCarray>(Propeller::torque_curve_max);
				std::unique_ptr<VecToCarray> eff_curve			= std::make_unique<VecToCarray>(Propeller::eff_curve);
				std::unique_ptr<VecToCarray> eff_curve_min		= std::make_unique<VecToCarray>(Propeller::eff_curve_min);
				std::unique_ptr<VecToCarray> eff_curve_max		= std::make_unique<VecToCarray>(Propeller::eff_curve_max);

				ImGui::PushItemWidth(100 * xscale);
				ImGui::DragFloat("Min RPM", &Propeller::min_rpm, NULL, 1, 100000, NULL, slider_flags); 
				ImGui::SameLine();
				ImGui::DragFloat("Min Airspeed", &Propeller::min_v, NULL, 0.1, 1200, NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_vel.c_str());
				ImGui::DragFloat("Max RPM", &Propeller::max_rpm, NULL, 1, 100000, NULL, slider_flags);
				ImGui::SameLine();
				ImGui::DragFloat("Max Airspeed", &Propeller::max_v, NULL, 0.1, 1200, NULL, slider_flags); ImGui::SameLine(); ImGui::Text(display_vel.c_str());
				ImGui::NewLine();

				std::string plot_title_thrust = "Thrust " + graph_display_units_thrust;


				if (ImPlot::BeginPlot(plot_title_thrust.c_str(), ImVec2(-1, 0), plot_flags))
				{
					char plot_label[255];
					snprintf(plot_label, 255, "%.1f %s", Propeller::air_speed, graph_display_units_speed);
					ImPlot::SetupAxes("RPM", plot_title_thrust.c_str(), axis_flags, axis_flags);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, thrust_curve->arr, Propeller::thrust_curve_steps, {ImPlotProp_LineColor, ImVec4(1, 0.2, 0.2, 1)});
					snprintf(plot_label, 255, "%.1f %s", Propeller::min_v, graph_display_units_speed);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, thrust_curve_min->arr, Propeller::thrust_curve_steps, { ImPlotProp_LineColor, ImVec4(0.1, 1, 0.3, 1) });
					snprintf(plot_label, 255, "%.1f %s", Propeller::max_v, graph_display_units_speed);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, thrust_curve_max->arr, Propeller::thrust_curve_steps, { ImPlotProp_LineColor, ImVec4(0, 0.5, 1, 1) });
					ImPlot::EndPlot();
				}
				ImGui::NewLine();

				std::string plot_title_torque = "Torque " + graph_display_units_torque;

				if (ImPlot::BeginPlot(plot_title_torque.c_str(), ImVec2(-1, 0), plot_flags))
				{
					char plot_label[255];
					snprintf(plot_label, 255, "%.1f %s", Propeller::air_speed, graph_display_units_speed);
					ImPlot::SetupAxes("RPM", plot_title_torque.c_str(), axis_flags, axis_flags);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, torque_curve->arr, Propeller::thrust_curve_steps, { ImPlotProp_LineColor, ImVec4(1, 0.2, 0.2, 1) });
					snprintf(plot_label, 255, "%.1f %s", Propeller::min_v, graph_display_units_speed);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, torque_curve_min->arr, Propeller::thrust_curve_steps, { ImPlotProp_LineColor, ImVec4(0.1, 1, 0.3, 1) });
					snprintf(plot_label, 255, "%.1f %s", Propeller::max_v, graph_display_units_speed);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, torque_curve_max->arr, Propeller::thrust_curve_steps, { ImPlotProp_LineColor, ImVec4(0, 0.5, 1, 1) });
					ImPlot::EndPlot();
				}
				ImGui::NewLine();

				if (ImPlot::BeginPlot("Efficiency", ImVec2(-1, 0), plot_flags))
				{
					ImPlotLocation loc = ImPlotLocation_East;
					loc |= ImPlotLocation_North;
					ImPlot::SetupLegend(loc, 0);
					char plot_label[255];
					snprintf(plot_label, 255, "%.1f %s", Propeller::air_speed, graph_display_units_speed);
					ImPlot::SetupAxes("RPM", "Efficiency", axis_flags, axis_flags);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, eff_curve->arr, Propeller::thrust_curve_steps, { ImPlotProp_LineColor, ImVec4(1, 0.2, 0.2, 1) });
					snprintf(plot_label, 255, "%.1f %s", Propeller::min_v, graph_display_units_speed);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, eff_curve_min->arr, Propeller::thrust_curve_steps, { ImPlotProp_LineColor, ImVec4(0.1, 1, 0.3, 1) });
					snprintf(plot_label, 255, "%.1f %s", Propeller::max_v, graph_display_units_speed);
					ImPlot::PlotLine(plot_label, rpm_curve->arr, eff_curve_max->arr, Propeller::thrust_curve_steps, { ImPlotProp_LineColor, ImVec4(0, 0.5, 1, 1) });
					ImPlot::EndPlot();
				}
				ImGui::NewLine();

				if (ImPlot::BeginPlot("Reynolds Number", ImVec2(-1, 0), plot_flags))
				{
					char plot_label[255];
					snprintf(plot_label, 255, "%.0f RPM", Propeller::rpm);
					ImPlot::SetupAxes("r/R", "Reynolds Number", axis_flags, axis_flags);
					ImPlot::PlotLine(plot_label, r_R->arr, re->arr, Propeller::steps, { ImPlotProp_LineColor, ImVec4(1, 0.2, 0.2, 1) });
					snprintf(plot_label, 255, "%.0f RPM", Propeller::min_rpm);
					ImPlot::PlotLine(plot_label, r_R->arr, re_min->arr, Propeller::steps, { ImPlotProp_LineColor, ImVec4(0.1, 1, 0.3, 1) });
					snprintf(plot_label, 255, "%.0f RPM", Propeller::max_rpm);
					ImPlot::PlotLine(plot_label, r_R->arr, re_max->arr, Propeller::steps, { ImPlotProp_LineColor, ImVec4(0, 0.5, 1, 1) });
					ImPlot::EndPlot();
				}
				ImGui::NewLine();

				std::string plot_title_chord = "Chord " + display_len;

				if (ImPlot::BeginPlot(plot_title_chord.c_str(), ImVec2(-1, 0), plot_flags))
				{
					ImPlot::SetupAxes("r/R", plot_title_chord.c_str(), axis_flags, axis_flags);
					ImPlot::PlotLine("##data", r_R->arr, chord->arr, Propeller::steps, { ImPlotProp_LineColor, ImVec4(0.1, 1, 0.3, 1) });
					ImPlot::EndPlot();
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Appearance"))
			{
				ImGui::Checkbox("Render Prop", &render_prop); ImGui::SameLine();
				HelpMarker("Recomend to uncheck for large step sizes or large amounts of airfoils to reduce lag. Prop files will still calculated if you chose to save.");

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.5));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 0, 0, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 0, 0, 0.25));

				if (ImGui::Button("Reset Camera"))
				{
					Camera::zoom = -4.0 * Propeller::diameter * 0.5;
					Camera::panX = 0.0f;
					Camera::panY = 0.0f;
					Camera::rotX = 20.0f;
					Camera::rotY = -45.0f;
				}
				ImGui::PopStyleColor(3);

				if (ImGui::Button("Reset Prop to Default"))
				{
					Propeller::reset();

					Camera::zoom = -3.0 * Propeller::diameter * 0.5;
					Camera::panX = 0.0f;
					Camera::panY = 0.0f;
					Camera::rotX = 20.0f;
					Camera::rotY = -45.0f;
				}

				ImGui::Checkbox("Hide Grid", &hide_grid);
				ImGui::Checkbox("Hide Rotation Direction", &hide_rot_dir);
				ImGui::NewLine();


				if (ImGui::Button("Reset Colors"))
				{
					def_color[0] = 0; def_color[1] = 1; def_color[2] = 0; def_color[3] = 1;
					center_color[0] = 1;    center_color[1] = 0;    center_color[2] = 0;    center_color[3] = 1;
					le_color[0] = 0;        le_color[1] = 1;        le_color[2] = 0;        le_color[3] = 1;
					te_color[0] = 0;        te_color[1] = 0;        te_color[2] = 1;        te_color[3] = 1;
					af_color[0] = 1;        af_color[1] = 1;      af_color[2] = 1;      af_color[3] = 0.3;
					hub_color[0] = 0;       hub_color[1] = 1;       hub_color[2] = 1;       hub_color[3] = 1;
					hub_center_color[0] = 1; hub_center_color[1] = 0.7; hub_center_color[2] = 0.7; hub_center_color[3] = 1;
					hub_le_color[0] = 0.7;  hub_le_color[1] = 1;    hub_le_color[2] = 0.7;  hub_le_color[3] = 1;
					hub_te_color[0] = 0.7;  hub_te_color[1] = 0.7;  hub_te_color[2] = 1;    hub_te_color[3] = 1;

				}

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(def_color[0], def_color[1], def_color[2], 0.4));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(def_color[0], def_color[1], def_color[2], 0.8));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(def_color[0], def_color[1], def_color[2], 0.25));

				if (ImGui::Button("Homogenize colors"))
				{
					center_color[0] = def_color[0];      center_color[1] = def_color[1];      center_color[2] = def_color[2];      center_color[3] = def_color[3];
					le_color[0] = def_color[0];          le_color[1] = def_color[1];          le_color[2] = def_color[2];          le_color[3] = def_color[3];
					te_color[0] = def_color[0];          te_color[1] = def_color[1];          te_color[2] = def_color[2];          te_color[3] = def_color[3];
					af_color[0] = def_color[0];          af_color[1] = def_color[1];          af_color[2] = def_color[2];          af_color[3] = def_color[3];
					hub_color[0] = def_color[0];         hub_color[1] = def_color[1];         hub_color[2] = def_color[2];         hub_color[3] = def_color[3];
					hub_center_color[0] = def_color[0];  hub_center_color[1] = def_color[1];  hub_center_color[2] = def_color[2];  hub_center_color[3] = def_color[3];
					hub_le_color[0] = def_color[0];      hub_le_color[1] = def_color[1];      hub_le_color[2] = def_color[2];      hub_le_color[3] = def_color[3];
					hub_te_color[0] = def_color[0];      hub_te_color[1] = def_color[1];      hub_te_color[2] = def_color[2];      hub_te_color[3] = def_color[3];

				}

				ImGui::PopStyleColor(3);

				ImGui::NewLine();

				ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_NoSidePreview;
				color_flags += ImGuiColorEditFlags_NoSmallPreview;
				color_flags += ImGuiColorEditFlags_NoLabel;

				if (ImGui::TreeNode("default color"))
				{
					ImGui::Text("default color");
					ImGui::ColorPicker4("default color", def_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				if (ImGui::TreeNode("leading edge color"))
				{
					ImGui::Text("leading edge color");
					ImGui::ColorPicker4("leading edge color", le_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				if (ImGui::TreeNode("centerline color"))
				{
					ImGui::Text("centerline color");
					ImGui::ColorPicker4("centerline color", center_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				if (ImGui::TreeNode("trailing edge color"))
				{
					ImGui::Text("trailing edge color");
					ImGui::ColorPicker4("trailing edge color", te_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				if (ImGui::TreeNode("airfoil color"))
				{
					ImGui::Text("airfoil color");
					ImGui::ColorPicker4("airfoil color", af_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				if (ImGui::TreeNode("hub color"))
				{
					ImGui::Text("hub color");
					ImGui::ColorPicker4("hub color", hub_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				if (ImGui::TreeNode("hub leading edge color"))
				{
					ImGui::Text("hub leading edge color");
					ImGui::ColorPicker4("hub leading edge color", hub_le_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				if (ImGui::TreeNode("hub centerline color"))
				{
					ImGui::Text("hub centerline color");
					ImGui::ColorPicker4("hub centerline color", hub_center_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				if (ImGui::TreeNode("hub trailing edge color"))
				{
					ImGui::Text("hub trailing edge color");
					ImGui::ColorPicker4("hub trailing edge color", hub_te_color, color_flags);
					ImGui::NewLine();
					ImGui::TreePop();
					ImGui::Spacing();
				}

				ImGui::SeparatorText("");
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();


		ImVec2 vMin3 = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax3 = ImGui::GetWindowContentRegionMax();
		vMin3.x += ImGui::GetWindowPos().x;
		vMin3.y += ImGui::GetWindowPos().y;
		vMax3.x += ImGui::GetWindowPos().x;
		vMax3.y += ImGui::GetWindowPos().y;

		topLeft.x = (vMax1.x + 20);
		topLeft.y = (vMin1.y - 35);
		bottomRight.x = (vMin3.x - 20);
		bottomRight.y = (vMin2.y - 45);
		topRight.x = (vMin3.x - 20);
		topRight.y = topLeft.y;

		vp_width = std::abs(topLeft.x + bottomRight.x);
		vp_height = std::abs(topLeft.y + bottomRight.y);


		x_r = static_cast<int>((std::abs(topLeft.x + bottomRight.x) * 0.5) - (w * 0.5));
		y_r = static_cast<int>((-std::abs(topLeft.y + bottomRight.y) * 0.5) + (h * 0.5));

		glViewport(x_r, y_r, w, h); 

		ImGui::End();
		//window3 end-----------------------------------------



		//prop rendering
		Render::setProjection(w, h);
		Render::setView(1.0f);

		Propeller::unitsChanged();
		if (Propeller::units_changed == true)
		{
			Camera::zoom = -3.0 * Propeller::diameter * 0.5;
			Camera::panX = 0.0f;
			Camera::panY = 0.0f;
			Camera::rotX = 20.0f;
			Camera::rotY = -45.0f;
		}

		if (Propeller::paramsChanged() == true)
		{
			Propeller::unitSelection();
			Propeller::hub_cyl.clear();
			Propeller::hub_cyl = Render::generateCylinder((Propeller::steps * 0.005) + 5, (Propeller::steps * 0.02) + 15, Propeller::hub_diameter, Propeller::hub_height, Propeller::hub_diamter_inner);
			generateBladeGeom();
			generateHub();
			Propeller::pitchUnitSelection();
			getAirfoil(Propeller::airfoil_file, Propeller::subdivisions);
			generateAirfoils();
			generatePropPointCloud();

			if (Propeller::graphs == true)
			{
				getPolarTable(Propeller::polar_file);
				generateThrustCurve();
			}

		}

		if (render_prop == true)
		{
			if (Propeller::hub_blade == true)
			{
				Render::drawCircularPattern(Propeller::center_line, Propeller::num_blades, center_color, true);
				Render::drawCircularPattern(Propeller::leading_edge, Propeller::num_blades, le_color, true);
				Render::drawCircularPattern(Propeller::trailing_edge, Propeller::num_blades, te_color, true);
				Render::drawCircularPattern(Propeller::airfoils, Propeller::num_blades, af_color, true);

				Render::drawCircularPattern(Propeller::center_line_hub, Propeller::num_blades, center_color, true);
				Render::drawCircularPattern(Propeller::leading_edge_hub, Propeller::num_blades, le_color, true);
				Render::drawCircularPattern(Propeller::trailing_edge_hub, Propeller::num_blades, te_color, true);
			}
			else if (Propeller::hub_blade == false)
			{
				Render::drawCircularPattern(Propeller::center_line, 1, center_color, true);
				Render::drawCircularPattern(Propeller::leading_edge, 1, le_color, true);
				Render::drawCircularPattern(Propeller::trailing_edge, 1, te_color, true);
				Render::drawCircularPattern(Propeller::airfoils, 1, af_color, true);
			}


			if (Propeller::hub == true && Propeller::hub_blade == true)
			{
				Render::drawPoints(Propeller::hub_cyl, hub_color);
			}
		}
		//prop rendering end



		if (hide_rot_dir == false)
			Render::drawCircularArrow(0, -Propeller::diameter * 0.1f, 0, Propeller::diameter * 0.2, 0, 1, 1, 0.5, NULL, 135);

		if (hide_grid == false)
			Render::drawGrid(Propeller::diameter * 0.25f, 5000, 0, false);


		Render::LegendItem legend[] = {
			{ le_color, "Leading edge" },
		    { te_color, "Trailing edge" },
			{ center_color, "Center line" },
			{ af_color, "Airfoils" },
			{ hub_color, "Hub" },
		};

		Render::drawLegend(w, h, topRight.x, legend, sizeof(legend) / sizeof(legend[0]), xscale);
		Render::drawAxisTriad(w, h, topLeft.x, 120 * xscale);


		OpenConfigFileDialog.SetWindowSize(w * 0.5, h * 0.5);
		OpenConfigFileDialog.SetWindowPos(w * 0.25, h * 0.25);
		OpenConfigFileDialog.Display();
		if (OpenConfigFileDialog.HasSelected())
		{
			std::string config = OpenConfigFileDialog.GetSelected().string();
			Propeller::openConfig(config);

			Camera::zoom = -3.0 * Propeller::diameter * 0.5;
			Camera::panX = 0.0f;
			Camera::panY = 0.0f;
			Camera::rotX = 20.0f;
			Camera::rotY = -45.0f;

			char text[255];
			strncpy_s(text, config.c_str(), 255);
			log.AddLog("Loaded Config: ");
			log.AddLog(text);
			log.AddLog("\n");
			OpenConfigFileDialog.ClearSelected();
		}


		SaveConfigFileDialog.SetWindowSize(w * 0.5, h * 0.5);
		SaveConfigFileDialog.SetWindowPos(w * 0.25, h * 0.25);
		SaveConfigFileDialog.Display();
		if (SaveConfigFileDialog.HasSelected())
		{
			std::filesystem::path name = SaveConfigFileDialog.GetSelected().string();

			if (name.extension() == ".toml")
			{
				name.replace_extension("");
			}

			std::string name_cfg = (name.string() + "_config");

			Propeller::saveConfig(name_cfg);

			char text[255];
			strncpy_s(text, name_cfg.c_str(), 255);
			log.AddLog("Saved Config at: ");
			log.AddLog(text);
			log.AddLog("\n");
			SaveConfigFileDialog.ClearSelected();
		}


		AirFoilFileDialog.SetWindowSize(w * 0.5, h * 0.5);
		AirFoilFileDialog.SetWindowPos(w * 0.25, h * 0.25);
		AirFoilFileDialog.Display();
		if (AirFoilFileDialog.HasSelected())
		{
			std::string airfoil = AirFoilFileDialog.GetSelected().string();
			Propeller::airfoil_file = airfoil;
			char text[255];
			strncpy_s(text, airfoil.c_str(), 255);
			log.AddLog("Loaded airfoil: ");
			log.AddLog(text);
			log.AddLog("\n");
			AirFoilFileDialog.ClearSelected();
		}


		AirFoilPolarFileDialog.SetWindowSize(w * 0.5, h * 0.5);
		AirFoilPolarFileDialog.SetWindowPos(w * 0.25, h * 0.25);
		AirFoilPolarFileDialog.Display();
		if (AirFoilPolarFileDialog.HasSelected())
		{
			std::string polar = AirFoilPolarFileDialog.GetSelected().string();
			Propeller::polar_file = polar;
			getPolarTable(Propeller::polar_file);
			char text[255];
			strncpy_s(text, polar.c_str(), 255);
			log.AddLog("Loaded Polar Table: ");
			log.AddLog(text);
			log.AddLog("\n");
			AirFoilPolarFileDialog.ClearSelected();
		}


		SaveAllFileDialog.SetWindowSize(w * 0.5, h * 0.5);
		SaveAllFileDialog.SetWindowPos(w * 0.25, h * 0.25);
		SaveAllFileDialog.Display();
		if (SaveAllFileDialog.HasSelected())
		{
			std::filesystem::path name = SaveAllFileDialog.GetSelected().string();

			if (name.extension() == ".txt" || name.extension() == ".toml")
			{
				name.replace_extension("");
			}

			std::string name_le = (name.string() + "_le");
			std::string name_te = (name.string() + "_te");
			std::string name_pc = (name.string() + "_point_cloud");
			std::string name_cfg = (name.string() + "_config");


			if (Propeller::hub_blade == true)
			{
				save2D_Vec(name_le, append2D_Vec(Propeller::leading_edge_hub, Propeller::leading_edge));
				save2D_Vec(name_te, append2D_Vec(Propeller::trailing_edge_hub, Propeller::trailing_edge));
				saveAirfoils(num_af_to_save, true, name.string());
			}

			if (Propeller::hub_blade == false)
			{
				save2D_Vec(name_le, Propeller::leading_edge);
				save2D_Vec(name_te, Propeller::trailing_edge);
				saveAirfoils(num_af_to_save, false, name.string());
			}

			save2D_Vec(name_pc, Propeller::prop_point_cloud);
			Propeller::saveConfig(name_cfg);


			char text[255];
			strncpy_s(text, name_le.c_str(), 255);
			log.AddLog("Leading edge Saved at: ");
			log.AddLog(text);
			log.AddLog("\n");

			strncpy_s(text, name_te.c_str(), 255);
			log.AddLog("Trailing edge Saved at: ");
			log.AddLog(text);
			log.AddLog("\n");

			strncpy_s(text, name_pc.c_str(), 255);
			log.AddLog("Point Cloud Saved at: ");
			log.AddLog(text);
			log.AddLog("\n");

			snprintf(text, 255, "%i Airfoils Saved at: ", num_af_to_save);
			log.AddLog(text);
			strncpy_s(text, name_te.c_str(), 255);
			log.AddLog(text);
			log.AddLog("\n");

			strncpy_s(text, name_cfg.c_str(), 255);
			log.AddLog("Config Saved at: ");
			log.AddLog(text);
			log.AddLog("\n");

			SaveAllFileDialog.ClearSelected();
		}


		BladeFileDialog.SetWindowSize(w * 0.5, h * 0.5);
		BladeFileDialog.SetWindowPos(w * 0.25, h * 0.25);
		BladeFileDialog.Display();
		if (BladeFileDialog.HasSelected())
		{
			std::filesystem::path name = BladeFileDialog.GetSelected().string();

			if (name.extension() == ".txt")
			{
				name.replace_extension("");
			}

			std::string name_le = (name.string() + "_le");
			std::string name_te = (name.string() + "_te");


			if (include_hub == true)
			{
				save2D_Vec(name_le, append2D_Vec(Propeller::leading_edge_hub, Propeller::leading_edge));
				save2D_Vec(name_te, append2D_Vec(Propeller::trailing_edge_hub, Propeller::trailing_edge));
				saveAirfoils(num_af_to_save, true, name.string());
			}

			if (include_hub == false)
			{
				save2D_Vec(name_le, Propeller::leading_edge);
				save2D_Vec(name_te, Propeller::trailing_edge);
				saveAirfoils(num_af_to_save, false, name.string());

			}


			char text[255];
			strncpy_s(text, name_le.c_str(), 255);
			log.AddLog("Leading edge Saved at: ");
			log.AddLog(text);
			log.AddLog("\n");

			strncpy_s(text, name_te.c_str(), 255);
			log.AddLog("Trailing edge Saved at: ");
			log.AddLog(text);
			log.AddLog("\n");

			snprintf(text, 255, "%i Airfoils Saved at: ", num_af_to_save);
			log.AddLog(text);
			strncpy_s(text, name_te.c_str(), 255);
			log.AddLog(text);
			log.AddLog("\n");

			BladeFileDialog.ClearSelected();
		}


		PointCloudFileDialog.SetWindowSize(w * 0.5, h * 0.5);
		PointCloudFileDialog.SetWindowPos(w * 0.25, h * 0.25);
		PointCloudFileDialog.Display();
		if (PointCloudFileDialog.HasSelected())
		{
			std::filesystem::path name = PointCloudFileDialog.GetSelected().string();

			if (name.extension() == ".txt")
			{
				name.replace_extension("");
			}

			std::string name_pc = (name.string() + "_point_cloud");

			save2D_Vec(name_pc, Propeller::prop_point_cloud);

			char text[255];
			strncpy_s(text, name_pc.c_str(), 255);
			log.AddLog("Point Cloud Saved at: ");
			log.AddLog(text);
			log.AddLog("\n");

			PointCloudFileDialog.ClearSelected();
		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(win);

		double t1 = glfwGetTime();

		if (frames % 5 == 0)
			fps = (1 / (t1 - t0));
	}

	glfwDestroyWindow(win);
	glfwTerminate();

	return 0;
}
