#ifndef POINT_RENDERER_H
#define POINT_RENDERER_H

#include<iostream>
#include<cmath>
#include<vector>
#include<string> 
#include<filesystem>
#include<deque>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include"propeller.h"


// ── Camera state ─────────────────────────────────────────────────
namespace Camera 
{
	float  rotX = 20.0f, rotY = -45.0f;
	double lastX = 0.0, lastY = 0.0;
	bool   dragging = false;
	bool   panning = false;
	float  panX = 0.0f, panY = 0.0f;
	static bool in_render_win{ true };
	static bool file_expl{ false };
	float  zoom = -3.0f;
	float speed = (fabsf(zoom) * 0.0015f);
	constexpr float PI = 3.14159265358979323846f;

}


// ── GLFW callbacks ───────────────────────────────────────────────
namespace Callbacks {
	void mouseButton(GLFWwindow*, int btn, int action, int)
	{
		if (btn == GLFW_MOUSE_BUTTON_LEFT)
			Camera::dragging = (action == GLFW_PRESS);

		if (btn == GLFW_MOUSE_BUTTON_RIGHT)  
			Camera::panning = (action == GLFW_PRESS);

		if (btn == GLFW_MOUSE_BUTTON_MIDDLE)
			Camera::panning = (action == GLFW_PRESS);
	}

	void cursorPos(GLFWwindow*, double x, double y)
	{
		if (Camera::dragging && Camera::in_render_win == true && Camera::file_expl == false)
		{
			Camera::rotY += (x - Camera::lastX) * 0.4f;
			Camera::rotX += (y - Camera::lastY) * 0.4f;
		}

		if (Camera::panning && Camera::in_render_win == true && Camera::file_expl == false)
		{
			// Scale pan speed relative to zoom depth so it feels
			// consistent regardless of how far in/out the camera is
			Camera::speed = fabsf(Camera::zoom) * 0.0015f;
			Camera::panX += (float)(x - Camera::lastX) * Camera::speed;
			Camera::panY -= (float)(y - Camera::lastY) * Camera::speed; // flip Y (screen vs world)
		}

		Camera::lastX = x;
		Camera::lastY = y;
	}

	void scroll(GLFWwindow*, double, double dy)
	{
		if (Camera::in_render_win == true && Camera::file_expl == false)
			Camera::zoom += (float)dy * 0.5f * Propeller::diameter * 0.2f;
	}

}


// ── Rendering ────────────────────────────────────────────────────
namespace Render 
{
	void setProjection(int w, int h) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float aspect = (float)w / h;
		float f = 1.0f / tanf(45.0f * Camera::PI / 180.0f / 2.0f);
		float near0 = 0.1f;
		float far0 = 10000.0f;
		float pm[16] = {
			f / aspect, 0,  0,                        0,
			0,        f,  0,                        0,
			0,        0,  (far0 + near0) / (near0 - far0),   -1,
			0,        0,  (2 * far0 * near0) / (near0 - far0),  0
		};
		glLoadMatrixf(pm);
	}

	void setView(float r) 
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(Camera::panX, Camera::panY, Camera::zoom); 
		glRotatef(Camera::rotX, 1, 0, 0);
		glRotatef(Camera::rotY, 0, 1, 0);
	}


	void drawPoints(std::vector<std::vector<float>>& points, float color[])
	{
		glBegin(GL_POINTS);
		for (auto& p : points) 
		{
			glColor4f(color[0], color[1], color[2], color[3]);
			glVertex3f(p[0], p[1], p[2]);
		}
		glEnd();
	}

	void drawLine(std::vector<std::vector<float>>& points, float color[])
	{
		glLineWidth(1.0f);
		glBegin(GL_LINE_STRIP);

		for (auto& i : points)
		{
			glColor4f(color[0], color[1], color[2], color[3]);
			glVertex3f(i[0], i[1], i[2]);
		}

		glEnd();
	}

	void drawCircularPattern(std::vector<std::vector<float>> vec, int n, float color[], bool points)
	{
		if (Propeller::hub == true)
			pointCuller(vec);

		int steps = (vec.size());

		for (int i{ 0 }; i < n; ++i)
		{
			std::vector<std::vector<float>> vec_cyl;

			for (int j{ 0 }; j < steps; ++j)
			{
				float x = vec[j][0];
				float y = vec[j][1];
				float z = vec[j][2];

				float r = sqrtf(powf(x, 2) + powf(z, 2));
				float spacing = (2 * Camera::PI * (float(i) / float(n)));
				float t = (std::atan2f(z, x) + spacing);

				float x_new = (r * std::cos(t));
				float z_new = (r * std::sin(t));

				vec_cyl.push_back({ x_new, y, z_new });
			}

			if (points == true)
				drawPoints(vec_cyl, color);
			else
				drawLine(vec_cyl, color);
		}

	}


	void drawGrid(float spacing = 1.0f, int halfLines = 50, float y_offset = 0, bool axis = false)
	{
		float extent = spacing * halfLines;
		float y = y_offset * -1.1;

		float R = 0.6;
		float G = 0.6;
		float B = 0.6;
		float A = 0.4;

		glLineWidth(1.0f);
		glBegin(GL_LINES);

		for (int i = -halfLines; i <= halfLines; ++i)
		{
			float t = i * spacing;

			// Highlight the origin axes
			bool isOrigin = (i == 0);

			// Lines parallel to X axis (varying Z)
			glColor4f(isOrigin ? R : R * 0.5,
				isOrigin ? G : G * 0.5,
				isOrigin ? B : B * 0.5,
				isOrigin ? A : A * 0.95);
			glVertex3f(-extent, y, t);
			glVertex3f(extent, y, t);

			// Lines parallel to Z axis (varying X)
			glColor4f(isOrigin ? R : R * 0.5,
				isOrigin ? G : G * 0.5,
				isOrigin ? B : B * 0.5,
				isOrigin ? A : A * 0.95);
			glVertex3f(t, y, -extent);
			glVertex3f(t, y, extent);
		}

		glEnd();


		if (axis == true)
		{
			// Colored axis rays extending to the horizon
			glLineWidth(1.5f);
			glBegin(GL_LINES);

			// X axis — red
			glColor4f(0.8f, 0.2f, 0.2f, 0.3f);
			glVertex3f(-extent, 0.0f, 0.0f);
			glVertex3f(extent, 0.0f, 0.0f);

			// Y axis — green
			glColor4f(0.2f, 0.8f, 0.2f, 0.3f);
			glVertex3f(0.0f, -extent, 0.0f);
			glVertex3f(0.0f, extent, 0.0f);

			// Z axis — blue
			glColor4f(0.2f, 0.2f, 0.8f, 0.3f);
			glVertex3f(0.0f, 0.0f, -extent);
			glVertex3f(0.0f, 0.0f, extent);

			glEnd();
		}

		glLineWidth(1.0f);
	}

	void drawAxisTriad(int windowWidth, int windowHeight, int offset_x, int size = 120)
	{
		// ── Carve out a small viewport in the top-left corner ────────
		int x = offset_x + 30;
		int y = windowHeight - size - 30;

		glViewport(x, y, size, size);
		glScissor(x, y, size, size);
		glEnable(GL_SCISSOR_TEST);

		glClear(GL_DEPTH_BUFFER_BIT);

		// ── Square orthographic projection ───────────────────────────
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		float hs = 2.3f;
		glOrtho(-hs, hs, -hs, hs, -10, 10);

		// ── Inherit camera rotation only (no translation/zoom) ───────
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glRotatef(Camera::rotX, 1, 0, 0);
		glRotatef(Camera::rotY, 0, 1, 0);

		// ── Draw axes ────────────────────────────────────────────────
		const float shaft = 1.0f;
		const float head = 0.22f;
		const float tip = 1.25f;

		glLineWidth(2.0f);

		struct Axis {
			float dx, dy, dz;
			float px, py, pz;
			float qx, qy, qz;
			float r, g, b;
		};

		Axis axes[3] = {
			{ 1,0,0,  0,1,0,  0,0,1,  0.85f, 0.2f,  0.2f  },
			{ 0,1,0,  1,0,0,  0,0,1,  0.2f,  0.85f, 0.2f  },
			{ 0,0,1,  1,0,0,  0,1,0,  0.2f,  0.4f, 0.85f },
		};

		for (auto& a : axes)
		{
			// Shaft
			glBegin(GL_LINES);
			glColor4f(a.r, a.g, a.b, 0.6f);
			glVertex3f(0, 0, 0);
			glVertex3f(a.dx * shaft, a.dy * shaft, a.dz * shaft);
			glEnd();

			// Arrowhead
			glBegin(GL_TRIANGLE_FAN);
			glColor4f(a.r, a.g, a.b, 0.6f);
			glVertex3f(a.dx * tip, a.dy * tip, a.dz * tip);
			glVertex3f(a.dx * shaft + a.px * head, a.dy * shaft + a.py * head, a.dz * shaft + a.pz * head);
			glVertex3f(a.dx * shaft + a.qx * head, a.dy * shaft + a.qy * head, a.dz * shaft + a.qz * head);
			glVertex3f(a.dx * shaft - a.px * head, a.dy * shaft - a.py * head, a.dz * shaft - a.pz * head);
			glVertex3f(a.dx * shaft - a.qx * head, a.dy * shaft - a.qy * head, a.dz * shaft - a.qz * head);
			glVertex3f(a.dx * shaft + a.px * head, a.dy * shaft + a.py * head, a.dz * shaft + a.pz * head);
			glEnd();
		}

		// ── Billboard labels ─────────────────────────────────────────
		struct Label {
			float wx, wy, wz;
			float r, g, b;
			int   axis;
		};

		float label_offset = 0.5f;
		Label labels[3] = {
			{ tip + label_offset, 0,           0,           0.85f, 0.2f,  0.2f,  0 },
			{ 0,           tip + label_offset, 0,           0.2f,  0.85f, 0.2f,  1 },
			{ 0,           0,           tip + label_offset, 0.2f,  0.2f, 0.85f, 2 },
		};

		const float s = 0.2f;

		for (auto& lb : labels)
		{
			glPushMatrix();
			glTranslatef(lb.wx, lb.wy, lb.wz);

			// Undo camera rotation so the glyph always faces the screen
			glRotatef(-Camera::rotY, 0, 1, 0);
			glRotatef(-Camera::rotX, 1, 0, 0);

			float color_offset = 2.5;
			glColor4f(lb.r * color_offset, lb.g * color_offset, lb.b * color_offset, 1.0f);
			glLineWidth(1.5f);
			glBegin(GL_LINES);

			if (lb.axis == 0)       // X  ╲ + ╱
			{
				glVertex3f(-s, s , 0);  glVertex3f(s, -s, 0);
				glVertex3f(-s, -s, 0);  glVertex3f(s, s, 0);
			}
			else if (lb.axis == 1)  // Y  ╲╱ then │
			{
				glVertex3f(-s * 0.7, s, 0);  glVertex3f(0, 0, 0);
				glVertex3f(s * 0.7, s, 0);  glVertex3f(0, 0, 0);
				glVertex3f(0, 0, 0);  glVertex3f(0, -s, 0);
			}
			else                    // Z  ── ╱ ──
			{
				glVertex3f(-s, s, 0);  glVertex3f(s, s, 0);
				glVertex3f(s, s, 0);  glVertex3f(-s, -s, 0);
				glVertex3f(-s, -s, 0);  glVertex3f(s, -s, 0);
			}

			glEnd();
			glPopMatrix();
		}

		// ── Restore ──────────────────────────────────────────────────
		glDisable(GL_SCISSOR_TEST);
		glViewport(0, 0, windowWidth, windowHeight);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glLineWidth(1.0f);
	}

	void drawCircularArrow(
		float cx, float cy, float cz,
		float diameter,
		float r, float g, float b, float a,
		int   segments = 60,
		float gapAngle = 30.0f,
		float lineWidth = 2.0f)
	{
		if (segments == NULL)
			segments = 60;
		if (gapAngle == NULL)
			gapAngle = 30;
		if (lineWidth == NULL)
			lineWidth = 2.0;

		float radius = diameter * 0.5f;
		float gapRad = gapAngle * Camera::PI / 180.0f;
		float startAngle = (3.0f * Camera::PI) - gapRad;  // start before the gap
		float endAngle = (1.0f * Camera::PI);                         // end after the gap

		// ── Arc (clockwise — angle decreases) ────────────────────────
		glLineWidth(lineWidth);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= segments; ++i)
		{
			// lerp from startAngle down to endAngle for clockwise motion
			float t = startAngle + ((float)i / (float)segments) * (endAngle - startAngle);
			glColor4f(r, g, b, a);
			glVertex3f(cx + radius * cosf(t), cy, cz + radius * sinf(t));
		}
		glEnd();

		// ── Arrowhead at the end of the arc ──────────────────────────
		float headAngle = startAngle;

		// Tangent for clockwise motion is the negative of the CCW tangent
		float tx = -sinf(headAngle);
		float tz = cosf(headAngle);

		float tipX = cx + radius * cosf(headAngle);
		float tipZ = cz + radius * sinf(headAngle);

		float headLen = radius * 0.3f;
		float headWidth = radius * 0.3f;

		// Inward normal along the radius
		float nx = cosf(headAngle);
		float nz = sinf(headAngle);

		// Arrowhead triangle — tip points in the clockwise tangent direction
		float ax = tipX + tx * headLen;
		float az = tipZ + tz * headLen;

		float bx = tipX + nx * headWidth;
		float bz = tipZ + nz * headWidth;

		float dx = tipX - nx * headWidth;
		float dz = tipZ - nz * headWidth;

		glBegin(GL_TRIANGLES);
		glColor4f(r, g, b, a);
		glVertex3f(ax, cy, az);
		glVertex3f(bx, cy, bz);
		glVertex3f(dx, cy, dz);
		glEnd();

		glLineWidth(1.0f);
	}


	struct LegendItem 
	{
		float* color;
		const char* label;
	};

	void drawLegend(int windowWidth, int windowHeight, int offset_x, LegendItem* items, int itemCount, float scale)
	{


		//int itemCount = sizeof(items) / sizeof(items[0]);

		// ── Layout constants ─────────────────────────────────────────
		int swatchW = 12 * scale;
		int swatchH = 12 * scale;
		int padding = 10 * scale;
		int rowHeight = 20 * scale;
		int labelGap = 6 * scale;

		int maxLabelW = 0;
		for (int i = 0; i < itemCount; ++i)
		{
			int w = (int)(strlen(items[i].label) * 7 * scale);
			if (w > maxLabelW) maxLabelW = w;
		}

		int boxW = padding + swatchW + labelGap + maxLabelW + padding;
		int boxH = padding + itemCount * rowHeight + padding;

		// Pin to upper right — ImGui Y is top-down so origin is top-left
		int boxX = offset_x - boxW - 20;
		int boxY = 20;                               // distance from top of screen

		ImDrawList* dl = ImGui::GetBackgroundDrawList();

		// ── Background box ───────────────────────────────────────────
		dl->AddRectFilled(
			ImVec2(boxX, boxY),
			ImVec2(boxX + boxW, boxY + boxH),
			IM_COL32(12, 12, 12, 165));

		// Background outline
		dl->AddRect(
			ImVec2(boxX, boxY),
			ImVec2(boxX + boxW, boxY + boxH),
			IM_COL32(100, 100, 100, 200));

		// ── Draw each item ───────────────────────────────────────────
		for (int i = 0; i < itemCount; ++i)
		{
			int rowY = boxY + padding + i * rowHeight + (rowHeight - swatchH) / 2;
			int swatchX = boxX + padding;

			// Colored box
			dl->AddRectFilled(
				ImVec2(swatchX, rowY),
				ImVec2(swatchX + swatchW, rowY + swatchH),
				IM_COL32(
					(int)(items[i].color[0] * 255),
					(int)(items[i].color[1] * 255),
					(int)(items[i].color[2] * 255),
					(int)(items[i].color[3] * 255)));

			// Colored box outline
			dl->AddRect(
				ImVec2(swatchX, rowY),
				ImVec2(swatchX + swatchW, rowY + swatchH),
				IM_COL32(180, 180, 180, 200));

			// White label
			int labelX = swatchX + swatchW + labelGap;
			int labelY = rowY + (swatchH - 13) / 2;

			dl->AddText(
				ImVec2((float)labelX, (float)labelY),
				IM_COL32(255, 255, 255, 255),
				items[i].label);
		}
	}


	std::vector<std::vector<float>> generateCylinder(int pts_h, int pts_c, float d, float h, float d_in)
	{
		float r = (d * 0.5);
		float r_in = (d_in * 0.5);
		std::vector<std::vector<float>> pts;

		float t{ 0 };
		int y_h{ 0 };
		float x{ 0 };
		float y{ 0 };
		float z{ 0 };
		float r_new{ 0 };

		//outside cylinder
		for (int i{ 0 }; i < pts_h; ++i)
		{
			for (int j{ 0 }; j < pts_c; ++j)
			{
				t = (((1 / float(pts_c)) * float(j)) * (2.0f * Camera::PI));
				x = (r * std::cos(t));
				z = (r * std::sin(t));
				y = (((1 / float(pts_h - 1)) * float(i)) * h) - (h * 0.5);
				pts.push_back({ x, y, z });
			}

		}


		//inside cylinder
		for (int i{ 0 }; i < pts_h; ++i)
		{
			for (int j{ 0 }; j < pts_c; ++j)
			{
				t = (((1 / float(pts_c)) * float(j)) * (2.0f * Camera::PI));
				x = (r_in * std::cos(t));
				z = (r_in * std::sin(t));
				y = (((1 / float(pts_h - 1)) * float(i)) * h) - (h * 0.5);
				pts.push_back({ x, y, z });
			}

		}


		//top cylinder
		for (int i{ 0 }; i < pts_h; ++i)
		{
			for (int j{ 0 }; j < pts_c; ++j)
			{
				r_new = (((1 / float(pts_h - 1)) * float(i)) * (r - r_in)) + r_in;
				t = (((1 / float(pts_c)) * float(j)) * (2.0f * Camera::PI));
				x = (r_new * std::cos(t));
				z = (r_new * std::sin(t));
				y = (h * 0.5);
				pts.push_back({ x, y, z });
			}
		}

		//bottom cylinder
		for (int i{ 0 }; i < pts_h; ++i)
		{
			for (int j{ 0 }; j < pts_c; ++j)
			{
				r_new = (((1 / float(pts_h - 1)) * float(i)) * (r - r_in)) + r_in;
				t = (((1 / float(pts_c)) * float(j)) * (2.0f * Camera::PI));
				x = (r_new * std::cos(t));
				z = (r_new * std::sin(t));
				y = (-h * 0.5);
				pts.push_back({ x, y, z });
			}
		}


		return pts;
	}

}

#endif // !POINT_RENDERER.H
