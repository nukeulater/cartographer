#include "stdafx.h"

#include "Blam\Engine\IceCreamFlavor\IceCreamFlavor.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\CustomMenu\CustomLanguage.h"
#include "H2MOD\Modules\GamePhysics\Patches\MeleeFix.h"
#include "H2MOD\Modules\HudElements\HudElements.h"
#include "H2MOD\Modules\Input\Mouseinput.h"
#include "H2MOD\Modules\RenderHooks\RenderHooks.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\SpecialEvents\SpecialEvents.h"
#include "Util\Hooks\Hook.h"

#ifndef NDEBUG
#include "H2MOD\Modules\DirectorHooks\DirectorHooks.h"
#include "H2MOD\Modules\ObserverMode\ObserverMode.h"
#include "H2MOD\Utils\Utils.h"
#endif

#include "imgui.h"
#include "imgui_handler.h"

namespace ImGuiHandler {
	namespace ImAdvancedSettings {
		std::string windowName = "advanced_settings";
		namespace
		{
			float crosshairSize = 1.0f;
			bool g_showHud = true;
			bool g_showFP = true;
			bool g_UncappedFPS = false;
			int g_fpsLimit = 60;
			bool g_hitfix = true;
			int g_deadzone = 0;
			int g_aiming = 0;
			int g_shadows = 0;
			int g_water = 0;
			int g_experimental = 0;
			bool g_init = false;
			int g_language_code = -1;

			const char* button_items[] = { "Dpad Up","Dpad Down","Dpad Left","Dpad Right","Start","Back","Left Thumb","Right Thumb","Left Bumper","Right Bumper","A","B","X","Y" };
			const char* action_items[] = { "Dpad Up","Dpad Down","Dpad Left","Dpad Right","Start","Back","Crouch","Zoom","Flashlight","Switch Grenades","Jump","Melee","Reload","Switch Weapons" };
			const WORD button_values[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 4096, 8192, 16384, 32768 };

			int button_placeholders[14];
			std::map<int, std::map<e_advanced_string, const char*>> string_table;
			//Used for controls that use the same string, A identifier has to be appended to them
			//I.E Reset##1... Reset##20
			std::map<std::string, std::string> string_cache;
			void DrawDeadzones()
			{
				ImDrawList* draw_list = ImGui::GetForegroundDrawList();
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImVec2 Center(
					viewport->WorkSize.x - 200,
					viewport->WorkSize.y - 200
				);

				draw_list->AddRectFilled(ImVec2(Center.x - 100, Center.y - 100), ImVec2(Center.x + 100, Center.y + 100), ImColor(20, 20, 20));
				draw_list->AddCircleFilled(Center, 100, ImColor(255, 255, 255), 120);
				if (H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_axial
					|| H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_both) {
					int X_Axis = (int)(100 * (H2Config_Deadzone_A_X / 100));
					int Y_Axis = (int)(100 * (H2Config_Deadzone_A_Y / 100));
					ImVec2 Y_TopLeft(
						Center.x - 100,
						Center.y - Y_Axis);
					ImVec2 Y_BottomRight(
						Center.x + 100,
						Center.y + Y_Axis);
					draw_list->AddRectFilled(Y_TopLeft, Y_BottomRight, ImColor(20, 20, 20, 125));
					ImVec2 X_TopLeft(Center.x - X_Axis, Center.y - 100);
					ImVec2 X_BottomRight(Center.x + X_Axis, Center.y + 100);
					draw_list->AddRectFilled(X_TopLeft, X_BottomRight, ImColor(20, 20, 20, 125));
				}
				if (H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_radial 
					|| H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_both) {
					draw_list->AddCircleFilled(Center, H2Config_Deadzone_Radial, ImColor(20, 20, 20, 125), 120);
				}

				short* C_Input = (short*)ControllerInput::get_controller_input(0);
				ImVec2 Thumb_Pos(
					Center.x + (100 * (C_Input[28] / (float)MAXSHORT)),
					Center.y - (100 * (C_Input[29] / (float)MAXSHORT)));
				int axial_invalid = 0;
				if (abs(C_Input[28]) <= ((float)MAXSHORT * (H2Config_Deadzone_A_X / 100)))
					axial_invalid++;
				if (abs(C_Input[29]) <= ((float)MAXSHORT * (H2Config_Deadzone_A_Y / 100)))
					axial_invalid++;
				bool radial_invalid = false;
				unsigned int ar = pow((short)((float)MAXSHORT * (H2Config_Deadzone_Radial / 100)), 2);
				unsigned int arx = pow(C_Input[28], 2);
				unsigned int ary = pow(C_Input[29], 2);
				unsigned int rh = arx + ary;
				if (rh <= ar)
				{
					radial_invalid = true;
				}
				bool valid = true;
				if (H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_axial 
					|| H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_both)
					if (axial_invalid == 2)
						valid = false;
				if (H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_radial 
					|| H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_both)
					if (radial_invalid)
						valid = false;

				if (valid)
				{
					draw_list->AddCircleFilled(Thumb_Pos, 5, ImColor(0, 255, 0), 60);
				}
				else
				{
					draw_list->AddCircleFilled(Thumb_Pos, 5, ImColor(255, 0, 0), 60);
				}
			}
			void HudSettings()
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(GetString(hud_title)))
				{
					ImVec2 b2_size = ImVec2(WidthPercentage(10), item_size.y);
					//Player FOV
					ImGui::Text(GetString(player_field_of_view));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderInt("##PlayerFOV1", &H2Config_field_of_view, 45, 110, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						HudElements::setFOV();

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputInt("##PlayerFOV2", &H2Config_field_of_view, 0, 110, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_field_of_view > 110)
							H2Config_field_of_view = 110;
						if (H2Config_field_of_view < 45)
							H2Config_field_of_view = 45;

						HudElements::setFOV();
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "PlayerFov3"), b2_size))
					{
						H2Config_field_of_view = 78.0f;
						HudElements::setFOV();
					}
					ImGui::PopItemWidth();


					//Vehicle FOV
					ImGui::Text(GetString(vehicle_field_of_view));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderInt("##VehicleFOV1", &H2Config_vehicle_field_of_view, 45, 110, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						HudElements::setVehicleFOV();

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputInt("##VehicleFOV2", &H2Config_vehicle_field_of_view, 0, 110, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_vehicle_field_of_view > 110)
							H2Config_vehicle_field_of_view = 110;
						if (H2Config_vehicle_field_of_view < 45)
							H2Config_vehicle_field_of_view = 45;

						HudElements::setVehicleFOV();
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "VehicleFOV3"), b2_size))
					{
						H2Config_vehicle_field_of_view = 78.0f;
						HudElements::setVehicleFOV();
					}
					ImGui::PopItemWidth();

					//Crosshair Offset
					ImGui::Text(GetString(crosshair_offset));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##Crosshair1", &H2Config_crosshair_offset, 0.0f, 0.5f, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						HudElements::setCrosshairPos();
					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##Crosshair2", &H2Config_crosshair_offset, 0, 110, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_crosshair_offset > 0.5)
							H2Config_crosshair_offset = 0.5;
						if (H2Config_crosshair_offset < 0)
							H2Config_crosshair_offset = 0;

						HudElements::setCrosshairPos();
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "Crosshair3"), b2_size))
					{
						H2Config_crosshair_offset = 0.138f;
						HudElements::setCrosshairPos();
					}
					ImGui::PopItemWidth();

					//Crosshair Size
					ImGui::Text(GetString(crosshair_size));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##CrosshairSize1", &H2Config_crosshair_scale, 0.0f, 2.0f, "");  ImGui::SameLine();
					if (ImGui::IsItemEdited())
						HudElements::setCrosshairSize();
					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##CrosshairSize2", &H2Config_crosshair_scale, 0, 110, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_crosshair_scale > 2)
							H2Config_crosshair_scale = 2;
						if (H2Config_crosshair_scale < 0)
							H2Config_crosshair_scale = 0;
						HudElements::setCrosshairSize();
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "CrosshairSize3"), b2_size))
					{
						H2Config_crosshair_scale = 1;
						HudElements::setCrosshairSize();
					}
					ImGui::PopItemWidth();

					ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
					ImGui::NewLine();
					//Ingame Change Display
					if (ImGui::Button(GetString(weaponoffsets, "WeaponOffsets"), b3_size))
					{
						ImGuiHandler::ToggleWindow("Weapon Offsets");
					}

					ImGui::Columns(2, NULL, false);

					ImGui::Checkbox(GetString(hide_ingame_chat), &H2Config_hide_ingame_chat);
					ImGui::NextColumn();
					ImGui::Checkbox(GetString(static_fp), &H2Config_static_first_person);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(static_fp_tooltip));
					ImGui::NextColumn();
					ImGui::Checkbox(GetString(show_hud), &g_showHud);
					if (ImGui::IsItemEdited())
						HudElements::ToggleHUD(g_showHud);
					ImGui::NextColumn();
					ImGui::Checkbox(GetString(show_first_person), &g_showFP);
					if (ImGui::IsItemEdited())
						HudElements::ToggleFirstPerson(g_showFP);
					ImGui::Columns(1);
					ImGui::NewLine();
				}
			}
			void VideoSettings()
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(GetString(video_title)))
				{
					CHRONO_DEFINE_TIME_AND_CLOCK();

					ImVec2 LargestText = ImGui::CalcTextSize(GetString(hires_fix), NULL, true);
					float float_offset = ImGui::GetCursorPosX() + LargestText.x + (LargestText.x * 0.075);
					//FPS Limit
					ImGui::Columns(2, NULL, false);
					ImGui::Text(GetString(fps_limit));
					ImGui::PushItemWidth(WidthPercentage(50));
					ImGui::InputInt("##FPS1", &H2Config_fps_limit, 0, 110);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(fps_limit_tooltip));
					if (ImGui::IsItemEdited()) {
						if (H2Config_fps_limit < 10 && H2Config_fps_limit != 0)
							H2Config_fps_limit = 10;
						if (H2Config_fps_limit > 2048)
							H2Config_fps_limit = 2048;
					}

					ImGui::SameLine();
					if (ImGui::Button(GetString(reset, "FPS2"), ImVec2(WidthPercentage(50), item_size.y)))
					{
						H2Config_fps_limit = 60;
					}
					ImGui::NextColumn();
					ImGui::PopItemWidth();
					ImGui::Text(GetString(refresh_rate));
					ImGui::PushItemWidth(WidthPercentage(100));
					int gRefresh = H2Config_refresh_rate;
					ImGui::InputInt("##Refresh1", &gRefresh, 0, 110, ImGuiInputTextFlags_AlwaysOverwrite);
					if (ImGui::IsItemEdited())
						H2Config_refresh_rate = gRefresh;
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(refresh_rate_tooltip));

					ImGui::NextColumn();
					//LOD
					ImGui::Text(GetString(lod));
					const char* items[] = { GetString(e_default), GetString(lod_1), GetString(lod_2), GetString(lod_3), GetString(lod_4), GetString(lod_5), GetString(lod_6) };
					ImGui::PushItemWidth(WidthPercentage(100));
					ImGui::Combo("##LOD", &H2Config_static_lod_state, items, 7);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(lod_tooltip));

					ImGui::NextColumn();
					ImGui::Text(GetString(shadow_title));
					const char* s_items[] = { GetString(tex_L1), GetString(e_default), GetString(tex_L2), GetString(tex_L3) };
					ImGui::PushItemWidth(WidthPercentage(100));
					if (ImGui::Combo("##Shadows", &g_shadows, s_items, 4))
					{
						H2Config_Override_Shadows = (e_override_texture_resolution)g_shadows;
						RenderHooks::ResetDevice();
					}
					ImGui::NextColumn();
					ImGui::Text(GetString(water_title));
					ImGui::PushItemWidth(WidthPercentage(100));
					if (ImGui::Combo("##Water", &g_water, s_items, 4))
					{
						H2Config_Override_Water = (e_override_texture_resolution)g_water;
						RenderHooks::ResetDevice();
					}
					ImGui::NextColumn();
					ImGui::Text(GetString(experimental_rendering_changes));
					const char* r_items[] = { GetString(render_none), GetString(render_cinematic), GetString(render_engine), GetString(render_patch) };
					ImGui::PushItemWidth(WidthPercentage(100));
					if (ImGui::Combo("##ExpRend", &g_experimental, r_items, 4))
					{
						H2Config_experimental_fps = (H2Config_Experimental_Rendering_Mode)g_experimental;
					}
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(experimental_rendering_tooltip));
					ImGui::Columns(1);

					//Force max shader LOD

					ImGui::Checkbox(GetString(shader_lod_max), &H2Config_shader_lod_max);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(shader_lod_max_tooltip));

					//Disable Light Suppressor

					ImGui::Checkbox(GetString(light_suppressor), &H2Config_light_suppressor);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(light_suppressor_tooltip));

					//Hires Fix

					ImGui::Checkbox(GetString(hires_fix), &H2Config_hiresfix);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(hires_fix_tooltip));


					//ImGui::Checkbox(GetString(experimental_rendering_changes), &H2Config_experimental_fps);
					//if (ImGui::IsItemHovered())
					//	ImGui::SetTooltip(GetString(experimental_rendering_tooltip));

				}
			}
			void MouseKeyboardSettings()
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(GetString(m_k_title)))
				{
					ImGui::Columns(2, NULL, false);

					//Raw Input
					TextVerticalPad(GetString(raw_mouse));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##RawMouse", &H2Config_raw_input);
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(GetString(raw_mouse_tooltip));
					}
					//Uniform Sensitivity
					ImGui::NextColumn();
					TextVerticalPad(GetString(uniform_sensitivity));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##MK_Sep", &H2Config_mouse_uniform);
					if (ImGui::IsItemEdited())
					{
						MouseInput::SetSensitivity(H2Config_mouse_sens);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(GetString(uniform_sensitivity_tooltip));
					}
					ImGui::Columns(1);
					if (H2Config_raw_input) {
						ImGui::Text(GetString(raw_mouse_sensitivity));
						ImGui::PushItemWidth(WidthPercentage(75));
						int g_raw_scale = (int)H2Config_raw_mouse_scale;
						ImGui::SliderInt("##RawMouseScale1", &g_raw_scale, 1, 100, ""); ImGui::SameLine();
						if (ImGui::IsItemEdited())
						{
							H2Config_raw_mouse_scale = (float)g_raw_scale;
						}
						ImGui::PushItemWidth(WidthPercentage(15));
						ImGui::InputFloat("##RawMouseScale2", &H2Config_raw_mouse_scale, 0, 110, "%.5f", ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
						if (ImGui::IsItemEdited()) {
							if (g_raw_scale > 100)
								g_raw_scale = 100;
							if (g_raw_scale < 1)
								g_raw_scale = 1;
							g_raw_scale = (int)H2Config_raw_mouse_scale;
						}
						ImGui::PushItemWidth(WidthPercentage(10));
						if (ImGui::Button(GetString(reset, "RawMouseScale2"), ImVec2(WidthPercentage(10), item_size.y)))
						{
							g_raw_scale = 25;
							H2Config_raw_mouse_scale = 25.0f;
						}
					}
					else
					{
						ImGui::Text(GetString(mouse_sensitivity));
						ImGui::PushItemWidth(WidthPercentage(75));
						int g_mouse_sens = (int)H2Config_mouse_sens;
						ImGui::SliderInt("##Mousesens1", &g_mouse_sens, 1, 100, ""); ImGui::SameLine();
						if (ImGui::IsItemEdited())
						{
							H2Config_mouse_sens = (float)g_mouse_sens;
							MouseInput::SetSensitivity(H2Config_mouse_sens);
						}
						ImGui::PushItemWidth(WidthPercentage(15));
						ImGui::InputFloat("##Mousesens2", &H2Config_mouse_sens, 0, 110, "%.5f", ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
						if (ImGui::IsItemEdited()) {
							if (g_mouse_sens > 100)
								g_mouse_sens = 100;
							if (g_mouse_sens < 1)
								g_mouse_sens = 1;
							g_mouse_sens = (int)H2Config_mouse_sens;
							MouseInput::SetSensitivity(H2Config_mouse_sens);
						}
						ImGui::PushItemWidth(WidthPercentage(10));
						if (ImGui::Button(GetString(reset, "Mousesens3"), ImVec2(WidthPercentage(10), item_size.y)))
						{
							g_mouse_sens = 3;
							H2Config_mouse_sens = 3.0f;
							MouseInput::SetSensitivity(H2Config_mouse_sens);
						}
					}
				}
			}
			void ControllerSettings()
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(GetString(controller_title)))
				{
					DrawDeadzones();
					ImGui::Columns(2, NULL, false);
					//Uniform Sensitivity
					TextVerticalPad(GetString(uniform_sensitivity));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##C_Sep", &H2Config_mouse_uniform);
					if (ImGui::IsItemEdited())
					{
						MouseInput::SetSensitivity(H2Config_mouse_sens);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(GetString(uniform_sensitivity_tooltip));
					}
					ImGui::Columns(1);


					ImGui::Text(GetString(controller_sensitivity));
					ImGui::PushItemWidth(WidthPercentage(75));
					int g_controller_sens = (int)H2Config_controller_sens;
					ImGui::SliderInt("##Controllersens1", &g_controller_sens, 1, 100, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
					{
						H2Config_controller_sens = (float)g_controller_sens;
						ControllerInput::SetSensitiviy(H2Config_controller_sens);
					}
					ImGui::PushItemWidth(WidthPercentage(15));
					ImGui::InputFloat("##Controllersens2", &H2Config_controller_sens, 0, 110, "%.5f", ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (g_controller_sens > 100)
							g_controller_sens = 100;
						if (g_controller_sens < 1)
							g_controller_sens = 1;
						g_controller_sens = (int)H2Config_controller_sens;
						ControllerInput::SetSensitiviy(H2Config_controller_sens);
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "Controllersens3"), ImVec2(WidthPercentage(10), item_size.y)))
					{
						g_controller_sens = 3;
						H2Config_controller_sens = 3.0f;
						ControllerInput::SetSensitiviy(H2Config_controller_sens);
					}
					ImGui::PopItemWidth();

					ImGui::Columns(2, NULL, false);
					ImGui::Text(GetString(aiming_type));
					const char* a_items[] = { GetString(e_default), GetString(modern) };
					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					if (ImGui::Combo("##C_Aiming_Style", &g_aiming, a_items, 2))
					{
						H2Config_controller_modern = g_aiming != 0;
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(GetString(aiming_type_tooltip));
					}
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					ImGui::Text(GetString(deadzone_type));
					const char* items[] = { GetString(axial), GetString(radial), GetString(both) };
					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					if (ImGui::Combo("##C_Deadzone_Type", &g_deadzone, items, 3))
					{
						H2Config_Controller_Deadzone = g_deadzone;
						ControllerInput::SetDeadzones();
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(GetString(deadzone_type_tooltip));
					}
					ImGui::PopItemWidth();
					ImGui::Columns(1);

					if (H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_axial 
						|| H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_both) {
						ImGui::Text(GetString(axial_deadzone_X));
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_A_X_1", &H2Config_Deadzone_A_X, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(13));
						ImGui::InputFloat("##C_Deadzone_A_X_2", &H2Config_Deadzone_A_X, 0, 3);
						if (ImGui::IsItemEdited())
						{
							if (H2Config_Deadzone_A_X < 0)
								H2Config_Deadzone_A_X = 0;
							if (H2Config_Deadzone_A_X > 100)
								H2Config_Deadzone_A_X = 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(15));
						if (ImGui::Button(GetString(e_default, "C_Deadzone_A_X_3"), ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_A_X = (8689.0f / (float)MAXSHORT) * 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::PopItemWidth();
						ImGui::Text(GetString(axial_deadzone_Y));
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_A_Y_1", &H2Config_Deadzone_A_Y, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(13));
						ImGui::InputFloat("##C_Deadzone_A_Y_2", &H2Config_Deadzone_A_Y, 0, 3);
						if (ImGui::IsItemEdited())
						{
							if (H2Config_Deadzone_A_Y < 0)
								H2Config_Deadzone_A_Y = 0;
							if (H2Config_Deadzone_A_Y > 100)
								H2Config_Deadzone_A_Y = 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(12));
						if (ImGui::Button(GetString(e_default, "C_Deadzone_A_Y_3"), ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_A_Y = (8689.0f / (float)MAXSHORT) * 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::PopItemWidth();
					}
					if (H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_radial 
						|| H2Config_Controller_Deadzone == _controller_input_thumbstick_deadzone_both) {
						ImGui::Text(GetString(radial_deadzone_radius));
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_R_1", &H2Config_Deadzone_Radial, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(13));
						ImGui::InputFloat("##C_Deadzone_R_2", &H2Config_Deadzone_Radial, 0, 3);
						if (ImGui::IsItemEdited())
						{
							if (H2Config_Deadzone_Radial < 0)
								H2Config_Deadzone_Radial = 0;
							if (H2Config_Deadzone_Radial > 100)
								H2Config_Deadzone_Radial = 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(12));
						if (ImGui::Button(GetString(e_default, "C_Deadzone_R_R_3"), ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_Radial = (8689.0f / (float)MAXSHORT) * 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::PopItemWidth();
					}
					ImGui::NewLine();
					ImGui::Text("Controller Layout");
					ImGui::NewLine();
					ImGui::TextWrapped("To use this you must have your games controller layout SET TO DEFAULT. Changing the drop down for the specific action will remap the button to the new one");
					ImGui::NewLine();
					ImGui::Columns(3, NULL, false);
					for (auto i = 0; i < 14; i++)
					{
						ImGui::Text(button_items[i]);
						ImGui::PushItemWidth(ImGui::GetColumnWidth());
						std::string Id = "##C_L" + std::to_string(i);
						if (ImGui::Combo(Id.c_str(), &button_placeholders[i], action_items, 14))
						{
							switch (button_values[i])
							{
							case XINPUT_GAMEPAD_DPAD_UP:
								H2Config_CustomLayout.DPAD_UP = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_DPAD_DOWN:
								H2Config_CustomLayout.DPAD_DOWN = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_DPAD_LEFT:
								H2Config_CustomLayout.DPAD_LEFT = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_DPAD_RIGHT:
								H2Config_CustomLayout.DPAD_RIGHT = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_START:
								H2Config_CustomLayout.START = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_BACK:
								H2Config_CustomLayout.BACK = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_LEFT_THUMB:
								H2Config_CustomLayout.LEFT_THUMB = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_RIGHT_THUMB:
								H2Config_CustomLayout.RIGHT_THUMB = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_LEFT_SHOULDER:
								H2Config_CustomLayout.LEFT_SHOULDER = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_RIGHT_SHOULDER:
								H2Config_CustomLayout.RIGHT_SHOULDER = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_A:
								H2Config_CustomLayout.A = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_B:
								H2Config_CustomLayout.B = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_X:
								H2Config_CustomLayout.X = button_values[button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_Y:
								H2Config_CustomLayout.Y = button_values[button_placeholders[i]];
								break;
							}
						}
						ImGui::PopItemWidth();
						ImGui::NextColumn();
					}
					ImGui::Columns(1);
				}
			}
			void HostSettings()
			{
				if (NetworkSession::LocalPeerIsSessionHost() || h2mod->GetEngineType() == e_engine_type::_single_player) {
					if (ImGui::CollapsingHeader(GetString(host_campagin_settings)))
					{
						ImGui::Columns(2, NULL, false);
						TextVerticalPad(GetString(anti_cheat));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						if (ImGui::Checkbox("##Anti-Cheat", &H2Config_anti_cheat_enabled))
						{
							for (int i = 0; i < NetworkSession::GetPeerCount(); i++)
							{
								NetworkMessage::SendAntiCheat(i);
							}
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip(GetString(anti_cheat_tooltip));
						}

						ImGui::NextColumn();

						//XDelay
						TextVerticalPad(GetString(disable_x_delay));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						if (ImGui::Checkbox("##XDelay", &H2Config_xDelay))
						{
							H2Tweaks::RefreshTogglexDelay();
						}
						ImGui::Columns(1);
						ImGui::Separator();
						bool* skulls = ice_cream_flavor_state();
						ImGui::Columns(3, NULL, false);

						TextVerticalPad(GetString(skull_anger));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullAnger", &skulls[_anger]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_anger_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_assassins));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullAssassins", &skulls[_assassians]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_assassins_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_black_eye));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullBlackEye", &skulls[_black_eye]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_black_eye_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_blind));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullBlind", &skulls[_blind]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_blind_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_catch));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullCatch", &skulls[_catch]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_catch_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_envy));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullEnvy", &skulls[_envy]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_envy_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_famine));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullFamine", &skulls[_famine]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_famine_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_ghost));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullGhost", &skulls[_ghost]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_ghost_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_grunt));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullGBP", &skulls[_grunt_birthday_party]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_grunt_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_iron));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullIron", &skulls[_iron]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_iron_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_iwbyd));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullIWHBYD", &skulls[_IWHBYD]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_iwbyd_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_mythic));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullMythic", &skulls[_mythic]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_mythic_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_sputnik));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullSputnik", &skulls[_sputnik]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_sputnik_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_thunderstorm));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullThunderstorm", &skulls[_thunderstorm]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_thunderstorm_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(GetString(skull_whuppopotamus));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullWhuppopatamus", &skulls[_whuppopotamus]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skull_whuppopotamus_tooltip));

						ImGui::Columns(1);
					}
				}
			}
			void GameSettings()
			{
				if (ImGui::CollapsingHeader(GetString(game_title)))
				{
					ImGui::Columns(2, NULL, false);

					TextVerticalPad(GetString(discord_presence));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##DRP", &H2Config_discord_enable);

					TextVerticalPad(GetString(upnp_title));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##upnp", &H2Config_upnp_enable);
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(GetString(upnp_tooltip));
					}

					TextVerticalPad(GetString(no_events_title));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##no_events", &H2Config_no_events);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(no_events_tooltip));

					if (SpecialEvents::getCurrentEvent() == SpecialEvents::_halloween) {
						TextVerticalPad(GetString(skeleton_biped));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##spooky_scary", &H2Config_spooky_boy);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetString(skeleton_biped_tooltip));
					}

					// next column elements now
					//Skip Intro
					ImGui::NextColumn();
					TextVerticalPad(GetString(disable_intro_videos));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##Intro", &H2Config_skip_intro);

#if 0
					ImGui::NextColumn();
					TextVerticalPad(GetString(melee_fix_title));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##melee_fix", &H2Config_melee_fix);
					if (ImGui::IsItemEdited())
						MeleeFix::MeleeCollisionPatch();
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(GetString(melee_fix_tooltip));
#endif

					ImGui::Columns(1);

					ImGui::Text(GetString(language));
					const char* l_items[]{ GetString(lang_english), GetString(lang_japanese), GetString(lang_german), GetString(lang_french), GetString(lang_spanish), GetString(lang_italian), GetString(lang_korean), GetString(lang_chinese), GetString(lang_native) };
					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					if (ImGui::Combo("##Language_Selection", &g_language_code, l_items, 9))
					{
						if (g_language_code == 8)
							H2Config_language.code_main = -1;
						else
							H2Config_language.code_main = g_language_code;
						setCustomLanguage(H2Config_language.code_main, H2Config_language.code_variant);
					}
					ImGui::PopItemWidth();


					ImGui::Columns(1);
					ImGui::NewLine();
				}
			}
		}
		const char* GetString(e_advanced_string string, const std::string& id)
		{
			if (string_table.count(H2Config_language.code_main))
			{
				if (string_table.at(H2Config_language.code_main).count(string)) {
					if (id.empty()) {
						return const_cast<char*>(string_table.at(H2Config_language.code_main).at(string));
					}

					if (!string_cache.count(id))
					{
						std::string temp_str(const_cast<char*>(string_table.at(H2Config_language.code_main).at(string)));
						temp_str.append("##");
						temp_str.append(id);
						string_cache[id] = temp_str;
					}
					return (char*)string_cache[id].c_str();
				}
			}
			if (id.empty()) {
				return const_cast<char*>(string_table.at(0).at(string));
			}
			if (!string_cache.count(id))
			{
				std::string temp_str(const_cast<char*>(string_table.at(0).at(string)));
				temp_str.append("##");
				temp_str.append(id);
				string_cache[id] = temp_str;
			}
			return (char*)string_cache[id].c_str();
		}
		void Render(bool* p_open)
		{
			if (!g_init)
			{
				g_deadzone = (int)H2Config_Controller_Deadzone;
				g_aiming = (int)H2Config_controller_modern;
				g_language_code = H2Config_language.code_main;
				g_shadows = (int)H2Config_Override_Shadows;
				g_water = (int)H2Config_Override_Water;
				g_experimental = (int)H2Config_experimental_fps;
				if (g_language_code == -1)
					g_language_code = 8;
				g_init = true;
			}

			bool open = *p_open;
			ImGuiIO& io = ImGui::GetIO();
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
			//window_flags |= ImGuiWindowFlags_MenuBar;
			ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x * 0.5f, viewport->WorkSize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			//ImGui::PushFont(font2);
			ImGui::SetNextWindowSize(ImVec2(650, 530), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(610, 530), ImVec2(1920, 1080));
			if (h2mod->GetEngineType() == _main_menu)
				ImGui::SetNextWindowBgAlpha(1);
			if (ImGui::Begin(GetString(e_advanced_string::title), &open, window_flags))
			{
				HudSettings();
				VideoSettings();
				MouseKeyboardSettings();
				ControllerSettings();
				HostSettings();
				GameSettings();


#ifndef NDEBUG
				if (ImGui::CollapsingHeader("Dev Testing"))
				{
					/*
					if(ImGui::CollapsingHeader("Misc"))
					{
						if(ImGui::Button("Log Player Unit Objects"))
						{
							PlayerIterator playerIt;
							s_data_array* Objects = *Memory::GetAddress<s_data_array**>(0x4E461C);

							while(playerIt.get_next_active_player())
							{
								auto player = playerIt.get_current_player_data();
								int object = *(int*)&Objects->datum[12 * player->unit_index.Index + 8];
								LOG_INFO_GAME(L"[DevDebug]: {} {} {}", playerIt.get_current_player_name(), IntToWString<int>(player->unit_index.ToInt(), std::hex), IntToWString<int>(object, std::hex));
							}
						}
					}
					*/
					ImGui::Indent();
					if (ImGui::CollapsingHeader("Director Mode"))
					{
						if (ImGui::Button("Set Observer Team"))
						{
							WriteValue(Memory::GetAddress(0x51A6B4), 255);
						}
						if (ImGui::Button("Game"))
						{
							DirectorHooks::SetDirectorMode(DirectorHooks::e_game);
							ObserverMode::SwitchObserverMode(ObserverMode::observer_none);
						}
						if (ImGui::Button("Editor"))
						{
							ObserverMode::SwitchObserverMode(ObserverMode::observer_freecam);
						}
						ImGui::SameLine();
						if (ImGui::Button("Editor Follow"))
						{
							ObserverMode::NextPlayer();
							ObserverMode::SwitchObserverMode(ObserverMode::observer_followcam);
						}
						ImGui::SameLine();
						if (ImGui::Button("Editor First Person"))
						{
							ObserverMode::NextPlayer();
							ObserverMode::SwitchObserverMode(ObserverMode::observer_firstperson);
						}
						if (ImGui::Button("Player"))
						{
							DirectorHooks::SetDirectorMode(DirectorHooks::e_game);
							ObserverMode::SwitchObserverMode(ObserverMode::observer_none);
						}
						if (ImGui::Button("N"))
						{
							ObserverMode::NextPlayer();
						}
					}
					if (ImGui::CollapsingHeader("Raster Layers")) {
						ImGui::Columns(4, NULL, false);
						for (auto i = 0; i < 25; i++)
						{
							if (ImGui::Checkbox(IntToString<int>(i).c_str(), &ras_layer_overrides[i]))
							{
								RenderHooks::ResetDevice();
							}
							ImGui::NextColumn();
						}
						ImGui::Columns(1);
					}
					if (ImGui::CollapsingHeader("Render Geometries"))
					{
						ImGui::Columns(4, NULL, false);
						for (auto i = 0; i < 24; i++)
						{
							ImGui::Checkbox(IntToString<int>(i).c_str(), &geo_render_overrides[i]);
							ImGui::NextColumn();
						}
						ImGui::Columns(1);
					}
					static int event_type = H2Config_forced_event;
					if (ImGui::CollapsingHeader("Events"))
					{
						if (ImGui::RadioButton("None", &event_type, SpecialEvents::e_event_type::_no_event))
						{
							H2Config_forced_event = (int)SpecialEvents::e_event_type::_no_event;
						} ImGui::SameLine();
						if (ImGui::RadioButton("Christmas", &event_type, SpecialEvents::e_event_type::_christmas))
						{
							H2Config_forced_event = (int)SpecialEvents::e_event_type::_christmas;
						} ImGui::SameLine();
						if (ImGui::RadioButton("St Paddys", &event_type, SpecialEvents::e_event_type::_st_paddys))
						{
							H2Config_forced_event = (int)SpecialEvents::e_event_type::_st_paddys;
						} ImGui::SameLine();
						if (ImGui::RadioButton("Mook Madness", &event_type, SpecialEvents::e_event_type::_mook_maddness))
						{
							H2Config_forced_event = (int)SpecialEvents::e_event_type::_mook_maddness;
						}

						if (ImGui::RadioButton("Halloween", &event_type, SpecialEvents::e_event_type::_halloween))
						{
							H2Config_forced_event = (int)SpecialEvents::e_event_type::_halloween;
						}ImGui::SameLine();
						if (ImGui::RadioButton("Birthday", &event_type, SpecialEvents::e_event_type::_birthday))
						{
							H2Config_forced_event = (int)SpecialEvents::e_event_type::_birthday;
						}

					}
					if (ImGui::CollapsingHeader("WGIT Testing"))
					{
						if (ImGui::Button("Custom Languages"))
						{
							GSCustomMenuCall_Language();
						}
					}
				}
#endif
			}

			ImGui::PopStyleVar();
			ImGui::End();

			if (!open)
			{
				ImGuiHandler::ToggleWindow(ImGuiHandler::ImAdvancedSettings::windowName);
			}
		}
		void Open()
		{
			WORD button_layout[_controller_input_end];
			H2Config_CustomLayout.GetLayout(button_layout);
			for (int i = 0; i < _controller_input_end; i++)
			{
				for (int j = 0; j < _controller_input_end; j++)
				{
					if (button_values[j] == button_layout[i])
						button_placeholders[i] = j;
				}
			}
		}
		void Close()
		{
			SaveH2Config();
		}
		void BuildStringsTable()
		{
			string_table[0][e_advanced_string::title] = "        Advanced Settings";
			string_table[0][e_advanced_string::hud_title] = "HUD Settings";
			string_table[0][e_advanced_string::player_field_of_view] = "Player Field of View";
			string_table[0][e_advanced_string::reset] = "Reset";
			string_table[0][e_advanced_string::vehicle_field_of_view] = "Vehicle Field of View";
			string_table[0][e_advanced_string::crosshair_offset] = "Crosshair Offset";
			string_table[0][e_advanced_string::crosshair_size] = "Crosshair Size";
			string_table[0][e_advanced_string::hide_ingame_chat] = "Hide Ingame Chat";
			string_table[0][e_advanced_string::show_hud] = "Show HUD";
			string_table[0][e_advanced_string::show_first_person] = "Show First Person";
			string_table[0][e_advanced_string::weaponoffsets] = "Adjust Weapon Offsets";
			string_table[0][e_advanced_string::video_title] = "Video Settings";
			string_table[0][e_advanced_string::fps_limit] = "FPS Limit";
			string_table[0][e_advanced_string::fps_limit_tooltip] =
				"Setting this to 0 will uncap your games frame rate."
				"\nAnything over 60 may cause performance issues"
				"\nUse the Experimental Rendering Changes to resolve them";
			string_table[0][e_advanced_string::experimental_rendering_changes] = "Experimental Rendering Mode";
			string_table[0][e_advanced_string::experimental_rendering_tooltip] =
				"This will change how the game handles rendering, requires a restart to take effect."
				"\n\nNone: Default behavior of the game, will not work past 60FPS"
				"\n\nCinematic: Tricks the game into rending in cinematic mode"
				"\n\nEngine: Forces the unused native engine interpolation"
				"\n\nOriginal: Forces the original framerate limiter used in the original game, tied to tickrate";
			string_table[0][e_advanced_string::render_none] = "None";
			string_table[0][e_advanced_string::render_cinematic] = "Cinematic Force";
			string_table[0][e_advanced_string::render_engine] = "Engine Force";
			string_table[0][e_advanced_string::refresh_rate] = "Refresh Rate";
			string_table[0][e_advanced_string::refresh_rate_tooltip] = "This settings requires a restart to take effect";
			string_table[0][e_advanced_string::lod] = "Level of Detail";
			string_table[0][e_advanced_string::e_default] = "Default";
			string_table[0][e_advanced_string::lod_1] = "L1 - Very Low";
			string_table[0][e_advanced_string::lod_2] = "L2 - Low";
			string_table[0][e_advanced_string::lod_3] = "L3 - Medium";
			string_table[0][e_advanced_string::lod_4] = "L4 - High";
			string_table[0][e_advanced_string::lod_5] = "L5 - Very High";
			string_table[0][e_advanced_string::lod_6] = "L6 - Cinematic";
			string_table[0][e_advanced_string::shadow_title] = "Shadow Quality";
			string_table[0][e_advanced_string::water_title] = "Water Quality";
			string_table[0][e_advanced_string::tex_L1] = "Low";
			string_table[0][e_advanced_string::tex_L2] = "High";
			string_table[0][e_advanced_string::tex_L3] = "Ultra";
			string_table[0][e_advanced_string::lod_tooltip] =
				"Changing this will force the game to use the set Level of Detail for models that have them"
				"\nLeaving it at default makes it dynamic which is the games default behaviour.";
			string_table[0][e_advanced_string::shader_lod_max] = "Force Max Shader LOD";
			string_table[0][e_advanced_string::shader_lod_max_tooltip] =
				"This will force shaders to use the highest LODS regardless of their distance away from the player"
				"\nA restart is required for these changes to take effect.";
			string_table[0][e_advanced_string::light_suppressor] = "Disable Light Suppression";
			string_table[0][e_advanced_string::light_suppressor_tooltip] =
				"This will force lights to not fade out when there's multiple of them onscreen"
				"\nA restart is required for these changes to take effect.";
			string_table[0][e_advanced_string::hires_fix] = "High Resolution Fix";
			string_table[0][e_advanced_string::hires_fix_tooltip] =
				"This will enable fixes for high resolution monitors that will fix text clipping"
				"\nA restart is required for these changes to take effect.";
			string_table[0][e_advanced_string::m_k_title] = "Mouse and Keyboard Input";
			string_table[0][e_advanced_string::raw_mouse] = "Raw Mouse Input";
			string_table[0][e_advanced_string::raw_mouse_tooltip] =
				"This will remove the game's default mouse acceleration."
				"\n\nNOTE: This setting does not work if you have Modern Aiming turned on for your controller.";
			string_table[0][e_advanced_string::uniform_sensitivity] = "Uniform Sensitivity";
			string_table[0][e_advanced_string::uniform_sensitivity_tooltip] =
				"By default the game has the vertical sensitivity half of the horizontal."
				"\nEnabling this option will make these match.";
			string_table[0][e_advanced_string::raw_mouse_sensitivity] = "Raw Mouse Sensitivity";
			string_table[0][e_advanced_string::mouse_sensitivity] = "Mouse Sensitivity";
			string_table[0][e_advanced_string::controller_title] = "Controller Input";
			string_table[0][e_advanced_string::controller_sensitivity] = "Controller Sensitivity";
			string_table[0][e_advanced_string::aiming_type] = "Aiming Type";
			string_table[0][e_advanced_string::modern] = "Modern";
			string_table[0][e_advanced_string::aiming_type_tooltip] =
				"Modern Aiming will remove the native acceleration zones from a controller while aiming, allowing for a more precise aim."
				"\n\nNOTE: Selecting Modern Aiming will cause Raw Mouse input to not work.";
			string_table[0][e_advanced_string::deadzone_type] = "Deadzone Type";
			string_table[0][e_advanced_string::axial] = "Axial";
			string_table[0][e_advanced_string::radial] = "Radial";
			string_table[0][e_advanced_string::both] = "Both";
			string_table[0][e_advanced_string::deadzone_type_tooltip] = "Halo 2 by default uses axial deadzones, radial deadzones have been added as another option for players.";
			string_table[0][e_advanced_string::axial_deadzone_X] = "Axial Deadzone X";
			string_table[0][e_advanced_string::axial_deadzone_Y] = "Axial Deadzone Y";
			string_table[0][e_advanced_string::radial_deadzone_radius] = "Radial Deadzone Radius";
			string_table[0][e_advanced_string::host_campagin_settings] = "Host & Campaign Settings";
			string_table[0][e_advanced_string::anti_cheat] = "Anti-Cheat";
			string_table[0][e_advanced_string::anti_cheat_tooltip] = "Allows you to disable the Anti-Cheat for your lobby.";
			string_table[0][e_advanced_string::disable_x_delay] = "Enable X to Delay";
			string_table[0][e_advanced_string::skull_anger] = "Anger";
			string_table[0][e_advanced_string::skull_anger_tooltip] = "Enemies and allies fire their weapons faster and more frequently.";
			string_table[0][e_advanced_string::skull_assassins] = "Assassins";
			string_table[0][e_advanced_string::skull_assassins_tooltip] =
				"All enemies in game are permanently cloaked. Allies can sometimes"
				"\nsee them but mostly they can't, so they can't help much.";
			string_table[0][e_advanced_string::skull_black_eye] = "Black Eye";
			string_table[0][e_advanced_string::skull_black_eye_tooltip] =
				"Your shield does not charge normally. To charge your shields you"
				"\nmust kill something with a melee attack";
			string_table[0][e_advanced_string::skull_blind] = "Blind";
			string_table[0][e_advanced_string::skull_blind_tooltip] =
				"Your heads-up display becomes invisible. In other words, you cannot"
				"\nsee your weapon, body, shields, ammunition, motion tracker,"
				"\n or use your flashlight.";
			string_table[0][e_advanced_string::skull_catch] = "Catch";
			string_table[0][e_advanced_string::skull_catch_tooltip] =
				"A.I. will throw more grenades. Also] = everybody will drop two grenades"
				"\n of their kind Flood will drop grenades depending on whether"
				"\n they're human or Covenant.";
			string_table[0][e_advanced_string::skull_envy] = "Envy";
			string_table[0][e_advanced_string::skull_envy_tooltip] =
				"The Master Chief now has an Active camouflage just like the Arbiter's."
				"\nHowever, there is no visible timer, so remember: five second"
				"\n cloak with ten second recharge on Legendary";
			string_table[0][e_advanced_string::skull_famine] = "Famine";
			string_table[0][e_advanced_string::skull_famine_tooltip] =
				"All dropped weapons have half ammo. Weapons that spawned on the floor or"
				"\nspawned with are unaffected.";
			string_table[0][e_advanced_string::skull_ghost] = "Ghost";
			string_table[0][e_advanced_string::skull_ghost_tooltip] = "A.I. characters will not flinch from attacks, melee or otherwise.";
			string_table[0][e_advanced_string::skull_grunt] = "Grunt Birthday";
			string_table[0][e_advanced_string::skull_grunt_tooltip] = "Headshots turn into Plasma Grenade explosions.";
			string_table[0][e_advanced_string::skull_iron] = "Iron";
			string_table[0][e_advanced_string::skull_iron_tooltip] =
				"When playing co-op, if either player dies the game restarts you at your"
				"\nlast checkpoint.";
			string_table[0][e_advanced_string::skull_iwbyd] = "IWBYD";
			string_table[0][e_advanced_string::skull_iwbyd_tooltip] =
				"The rarity of combat dialog is changed, rare lines become far more common"
				"\nbut common lines are still present at their normal rate";
			string_table[0][e_advanced_string::skull_mythic] = "Mythic";
			string_table[0][e_advanced_string::skull_mythic_tooltip] = "Enemies have more health and shielding, and are therefore harder to kill.";
			string_table[0][e_advanced_string::skull_sputnik] = "Sputnik";
			string_table[0][e_advanced_string::skull_sputnik_tooltip] =
				"The mass of certain objects is severely reduced, making them fly further"
				"\nwhen smacked with a melee hit, or when they are near an explosion";
			string_table[0][e_advanced_string::skull_thunderstorm] = "Thunderstorm";
			string_table[0][e_advanced_string::skull_thunderstorm_tooltip] = "Causes most enemy and ally units to be their highest rank.";
			string_table[0][e_advanced_string::skull_whuppopotamus] = "Whuppopotamus";
			string_table[0][e_advanced_string::skull_whuppopotamus_tooltip] = "Strengthens the hearing of both allies and enemies";
			string_table[0][e_advanced_string::game_title] = "Game Settings";
			string_table[0][e_advanced_string::discord_presence] = "Discord Rich Presence";
			string_table[0][e_advanced_string::disable_intro_videos] = "Disable Intro videos";
			string_table[0][e_advanced_string::language] = "Language";
			string_table[0][e_advanced_string::lang_english] = "English";
			string_table[0][e_advanced_string::lang_japanese] = "Japanese";
			string_table[0][e_advanced_string::lang_german] = "German";
			string_table[0][e_advanced_string::lang_french] = "French";
			string_table[0][e_advanced_string::lang_spanish] = "Spanish";
			string_table[0][e_advanced_string::lang_italian] = "Italian";
			string_table[0][e_advanced_string::lang_korean] = "Korean";
			string_table[0][e_advanced_string::lang_chinese] = "Chinese";
			string_table[0][e_advanced_string::lang_native] = "Native";
			string_table[0][e_advanced_string::static_fp] = "Static FP Scale";
			string_table[0][e_advanced_string::static_fp_tooltip] = "This setting will force your First person model to stay the default size independent of FOV.";
			string_table[0][e_advanced_string::upnp_title] = "UPNP Enabled";
			string_table[0][e_advanced_string::upnp_tooltip] = "Enabled UPNP Port forwarding for the project.";
			string_table[0][e_advanced_string::melee_fix_title] = "Melee Patch";
			string_table[0][e_advanced_string::melee_fix_tooltip] = "Allows you to turn off the melee patch";
			string_table[0][e_advanced_string::no_events_title] = "No Events";
			string_table[0][e_advanced_string::no_events_tooltip] = "Opt out of event cosmetics restart required to take effect";
			string_table[0][e_advanced_string::render_patch] = "Original FPS limiter";
			string_table[0][e_advanced_string::skeleton_biped] = "Play as Spooky boy";
			string_table[0][e_advanced_string::skeleton_biped_tooltip] = "Changes your biped to be a Spooky Scary Skeleton for the Halloween event";

			//Spanish.
			string_table[4][e_advanced_string::title] = "      Ajustes avanzados";
			string_table[4][e_advanced_string::hud_title] = "Ajustes de Interfaz";
			string_table[4][e_advanced_string::player_field_of_view] = "Campo de visión (Personaje)";
			string_table[4][e_advanced_string::reset] = "Inicial";
			string_table[4][e_advanced_string::vehicle_field_of_view] = "Campo de visión (Vehículo)";
			string_table[4][e_advanced_string::crosshair_offset] = "Posición de la mira";
			string_table[4][e_advanced_string::crosshair_size] = "Tamaño de la mira";
			string_table[4][e_advanced_string::hide_ingame_chat] = "Ocultar chat en partida";
			string_table[4][e_advanced_string::show_hud] = "Mostrar Interfaz";
			string_table[4][e_advanced_string::show_first_person] = "Mostrar primera persona";
			string_table[4][e_advanced_string::weaponoffsets] = "Ajustar compensaciones de armas";
			string_table[4][e_advanced_string::video_title] = "Ajustes de video";
			string_table[4][e_advanced_string::fps_limit] = "Limitar FPS";
			string_table[4][e_advanced_string::fps_limit_tooltip] =
				"Dejar este ajuste en 0 quitará el límite de fotogramas por segundo."
				"\nCualquier valor mayor a 60 puede causar problemas de rendimiento."
				"\nUsa el Cambio de Renderizado Experimental para solucionarlo.";
			string_table[4][e_advanced_string::experimental_rendering_changes] = "Cambio de Renderizado Experimental";
			string_table[4][e_advanced_string::experimental_rendering_tooltip] =
				"Esto cambiará la forma en que el juego maneja el renderizado, requiere un reinicio para que surta efecto."
				"\n\nNinguno: el comportamiento predeterminado del juego, no funcionará más allá de los 60FPS "
				"\n\nCinematic: Hace que el juego se desgarre en modo cinemático"
				"\n\nEngine: Fuerza la interpolación del motor nativo no utilizado"
				"\n\nOriginal: fuerza el limitador de FPS original utilizado en el juego original, vinculado a la velocidad de tick ";
			string_table[4][e_advanced_string::render_none] = "Ninguno";
			string_table[4][e_advanced_string::render_cinematic] = "Fuerza Cinematográfica";
			string_table[4][e_advanced_string::render_engine] = "Fuerza del motor";
			string_table[4][e_advanced_string::refresh_rate] = "Taza de refresco";
			string_table[4][e_advanced_string::refresh_rate_tooltip] = "Este ajuste requiere reiniciar el juego para que tenga efecto.";
			string_table[4][e_advanced_string::lod] = "Nivel de detalle";
			string_table[4][e_advanced_string::e_default] = "Inicial";
			string_table[4][e_advanced_string::lod_1] = "N1 - Muy bajo";
			string_table[4][e_advanced_string::lod_2] = "N2 - Bajo";
			string_table[4][e_advanced_string::lod_3] = "N3 - Medio";
			string_table[4][e_advanced_string::lod_4] = "N4 - Alto";
			string_table[4][e_advanced_string::lod_5] = "N5 - Muy alto";
			string_table[4][e_advanced_string::lod_6] = "N6 - Cinemático";
			string_table[4][e_advanced_string::shadow_title] = "Calidad sombra";
			string_table[4][e_advanced_string::water_title] = "Calidad del agua";
			string_table[4][e_advanced_string::tex_L1] = "Bajo";
			string_table[4][e_advanced_string::tex_L2] = "Alto";
			string_table[4][e_advanced_string::tex_L3] = "Muy alto";
			string_table[4][e_advanced_string::lod_tooltip] = "Cambiar esto forzará el juego a usar los modelos del nivel de detalle seleccionado si están disponibles.\nDejarlo en Predeterminado hará que el nivel de detalle sea dinámico y controlado por el juego.";
			string_table[4][e_advanced_string::shader_lod_max] = "Máximo nivel de profundidad de sombreado";
			string_table[4][e_advanced_string::shader_lod_max_tooltip] =
				"Esto obligará a los shaders a usar los LODS más altos independientemente de su distancia del jugador."
				"\nEste ajuste requiere reiniciar el juego para que tenga efecto.";
			string_table[4][e_advanced_string::light_suppressor] = "Desactivar supresión de luz";
			string_table[4][e_advanced_string::light_suppressor_tooltip] =
				"Esto hará que las luces no se desvanezcan cuando haya varias en la pantalla"
				"\nEste ajuste requiere reiniciar el juego para que tenga efecto.";
			string_table[4][e_advanced_string::hires_fix] = "Arreglos de alta resolución";
			string_table[4][e_advanced_string::hires_fix_tooltip] = "Esto habilitará arreglos para monitores de alta resolución, solucionará textos recortados.\nEste ajuste requiere reiniciar el juego para que tenga efecto.";
			string_table[4][e_advanced_string::m_k_title] = "Entrada de mouse y teclado";
			string_table[4][e_advanced_string::raw_mouse] = "Entrada de mouse pura";
			string_table[4][e_advanced_string::raw_mouse_tooltip] = "Esto desactivará la aceleración de mouse predeterminada del juego.\n\nNOTA: Este ajuste no funcionará si tienes Apuntado Moderno activado para tu mando.";
			string_table[4][e_advanced_string::uniform_sensitivity] = "Sensibilidad uniforme";
			string_table[4][e_advanced_string::uniform_sensitivity_tooltip] = "Por defecto el juego tiene la sensibilidad vertical a la mitad de la horizontal.\nActivar esta opción igualará estas sensibilidades.";
			string_table[4][e_advanced_string::raw_mouse_sensitivity] = "Sensibilidad de mouse pura";
			string_table[4][e_advanced_string::mouse_sensitivity] = "Sensibilidad de mouse";
			string_table[4][e_advanced_string::controller_title] = "Entrada de mando";
			string_table[4][e_advanced_string::controller_sensitivity] = "Sensibilidad de mando";
			string_table[4][e_advanced_string::aiming_type] = "Tipo de apuntado";
			string_table[4][e_advanced_string::modern] = "Moderno";
			string_table[4][e_advanced_string::aiming_type_tooltip] = "El apuntado Moderno eliminará las zonas de aceleración por defecto del mando al apuntar, lo que permite un apuntado más preciso.\n\nNOTA: Seleccionar apuntado Moderno hará que la Entrada de mouse pura no funcione.";
			string_table[4][e_advanced_string::deadzone_type] = "Tipo de Zona muerta";
			string_table[4][e_advanced_string::axial] = "Por eje";
			string_table[4][e_advanced_string::radial] = "Radial";
			string_table[4][e_advanced_string::both] = "Ambos";
			string_table[4][e_advanced_string::deadzone_type_tooltip] = "Por defecto, Halo 2 usa zonas muertas axiales. Las zonas muertas radiales fueron agregadas como otra opción para los jugadores.";
			string_table[4][e_advanced_string::axial_deadzone_X] = "Zona muerta del Eje X";
			string_table[4][e_advanced_string::axial_deadzone_Y] = "Zona muerta del Eje Y";
			string_table[4][e_advanced_string::radial_deadzone_radius] = "Radio de zona muerta radial";
			string_table[4][e_advanced_string::host_campagin_settings] = "Ajustes de anfitrión y campaña";
			string_table[4][e_advanced_string::anti_cheat] = "Anti-Trampas";
			string_table[4][e_advanced_string::anti_cheat_tooltip] = "Permite desactivar el anti-trampas de tu sala.";
			string_table[4][e_advanced_string::disable_x_delay] = "Desactivar X para retrasar";
			string_table[4][e_advanced_string::skull_anger] = "Ira";
			string_table[4][e_advanced_string::skull_anger_tooltip] = "Los enemigos y aliados disparan sus armas de forma más rápida y frecuente.";
			string_table[4][e_advanced_string::skull_assassins] = "Asesinos";
			string_table[4][e_advanced_string::skull_assassins_tooltip] = "Todos los enemigos en la partida serán camuflados permanentemente. Los aliados algunas veces\nlos verán pero no será muy frecuente, por esto no podrán ayudarte mucho.";
			string_table[4][e_advanced_string::skull_black_eye] = "Ojo Morado";
			string_table[4][e_advanced_string::skull_black_eye_tooltip] = "Tu escudo no se recarga normalmente. Para recargar tu escudo\ndebes matar algo con un ataque cuerpo a cuerpo. ";
			string_table[4][e_advanced_string::skull_blind] = "Ciego";
			string_table[4][e_advanced_string::skull_blind_tooltip] = "Tu interfaz en partida se hace invisible. En otras palabras, no podrás\nver tu arma, cuerpo, escudos, munición, sensor de movimiento,\no usar tu linterna.";
			string_table[4][e_advanced_string::skull_catch] = "Pilla";
			string_table[4][e_advanced_string::skull_catch_tooltip] = "La I.A. arrojará más granadas. Además, todos dejarán 2 granadas\ncorrespondientes. Los Flood dejarán granadas dependiendo\nsi son humanos o Covenant.";
			string_table[4][e_advanced_string::skull_envy] = "Envidia";
			string_table[4][e_advanced_string::skull_envy_tooltip] = "El Jefe Maestro ahora tiene un Camuflaje activo al igual que El Árbitro.\nSin embargo, no verás el cronómetro, así que recuerda: cinco segundos\n de camuflaje con diez segundos de recarga en dificultad Legendario.";
			string_table[4][e_advanced_string::skull_famine] = "Hambruna";
			string_table[4][e_advanced_string::skull_famine_tooltip] = "Todas las armas que dejen al morir tienen la mitad de la munición. Las armas que aparecen en el mapa\n no serán afectadas.";
			string_table[4][e_advanced_string::skull_ghost] = "Fantasma";
			string_table[4][e_advanced_string::skull_ghost_tooltip] = "Los personajes de I.A. no retrocederán a los ataques, ya sean cuerpo a cuerpo u otro tipo.";
			string_table[4][e_advanced_string::skull_grunt] = "Cumpleaños Grunt";
			string_table[4][e_advanced_string::skull_grunt_tooltip] = "Los tiros a la cabeza se convierten en explosiones de Granada de Plasma.";
			string_table[4][e_advanced_string::skull_iron] = "Hierro";
			string_table[4][e_advanced_string::skull_iron_tooltip] = "Al jugar co-operativo, si alguno de los jugadores muere el juego volverá al\núltimo punto de control.";
			string_table[4][e_advanced_string::skull_iwbyd] = "IWBYD";
			string_table[4][e_advanced_string::skull_iwbyd_tooltip] = "La rareza del diálogo de combate cambiará, el diálogo raro será más frecuente\n pero el diálogo común seguirá escuchándose con la misma frecuencia.";
			string_table[4][e_advanced_string::skull_mythic] = "Mítico";
			string_table[4][e_advanced_string::skull_mythic_tooltip] = "Los enemigos tienen más salud y escudo, así que serán más difíciles de matar.";
			string_table[4][e_advanced_string::skull_sputnik] = "Sputnik";
			string_table[4][e_advanced_string::skull_sputnik_tooltip] = "La masa de ciertos objetos será muy reducida, haciendo que vuelen más lejos\nsi son golpeados por un ataque cuerpo a cuerpo o si están cerca de una explosión.";
			string_table[4][e_advanced_string::skull_thunderstorm] = "Tormenta Eléctrica";
			string_table[4][e_advanced_string::skull_thunderstorm_tooltip] = "La mayoría de unidades enemigas y aliadas serán del rango más alto.";
			string_table[4][e_advanced_string::skull_whuppopotamus] = "Whuppopotamus";
			string_table[4][e_advanced_string::skull_whuppopotamus_tooltip] = "Mejora el oído de aliados y enemigos.";
			string_table[4][e_advanced_string::game_title] = "Ajustes del juego";
			string_table[4][e_advanced_string::discord_presence] = "Discord Rich Presence";
			string_table[4][e_advanced_string::disable_intro_videos] = "Desactivar Videos al inicio";
			string_table[4][e_advanced_string::language] = "Idioma";
			string_table[4][e_advanced_string::lang_english] = "Inglés";
			string_table[4][e_advanced_string::lang_japanese] = "Japonés";
			string_table[4][e_advanced_string::lang_german] = "Alemán";
			string_table[4][e_advanced_string::lang_french] = "Francés";
			string_table[4][e_advanced_string::lang_spanish] = "Español";
			string_table[4][e_advanced_string::lang_italian] = "Italiano";
			string_table[4][e_advanced_string::lang_korean] = "Coreano";
			string_table[4][e_advanced_string::lang_chinese] = "Chino";
			string_table[4][e_advanced_string::lang_native] = "Nativo";
			string_table[4][e_advanced_string::static_fp] = "Escala FP estática";
			string_table[4][e_advanced_string::static_fp_tooltip] = "Esta configuración obligará a su modelo en primera persona a mantener el tamaño predeterminado\nindependientemente del campo de visión.";
			string_table[4][e_advanced_string::upnp_title] = "UPNP habilitado";
			string_table[4][e_advanced_string::upnp_tooltip] = "Habilita el reenvío de puertos UPNP para el proyecto.";
			string_table[4][e_advanced_string::melee_fix_title] = "Parche cuerpo a cuerpo";
			string_table[4][e_advanced_string::melee_fix_tooltip] = "Te permite desactivar el parche cuerpo a cuerpo";
			string_table[4][e_advanced_string::no_events_title] = "No hay eventos";
			string_table[4][e_advanced_string::no_events_tooltip] = "Se requiere el reinicio de los cosméticos del evento para que surta efecto";
			string_table[4][e_advanced_string::render_patch] = "Limitador de velocidad de FPS original";
			string_table[4][e_advanced_string::skeleton_biped] = "Juega como Spooky boy";
			string_table[4][e_advanced_string::skeleton_biped_tooltip] = "Cambia a tu bípedo para que sea un esqueleto espeluznante y aterrador para el evento de Halloween.";

		}
	}
}
