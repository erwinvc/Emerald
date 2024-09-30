#pragma once
#include <vector>
#include <string_view>
#include "imgui.h"
#include "imgui_internal.h"
#include "util/utils.h"
#include "propertyDrawerUtils.h"

namespace emerald {

	//Visual divider style
	enum class DividerType {
		TOP,
		MIDDLE,
		BOTTOM,
		SINGLELINE
	};

	class PropertyDrawer {
	public:
		template<typename PropertyType, typename T, typename Q>
		static bool drawFloat(std::string_view name, const std::vector<Q*>& elements, PropertyType T::* member, PropertyType defaultValue, const char** symbols, DividerType dividerType) {
			return drawMultiProperty<PropertyType, T, Q>(name, elements, member, defaultValue, dividerType, [&](PropertyType& value) -> bool {
				return propertyDrawerUtils::DragFloat("", glm::value_ptr(value), symbols, 0.1f);
			});
		}

		template<typename PropertyType, typename T, typename Q>
		static bool drawFloat2(std::string_view name, const std::vector<Q*>& elements, PropertyType T::* member, PropertyType defaultValue, const char** symbols, DividerType dividerType) {
			return drawMultiProperty<PropertyType, T, Q>(name, elements, member, defaultValue, dividerType, [&](PropertyType& value) -> bool {
				return propertyDrawerUtils::DragFloat2("", glm::value_ptr(value), symbols, 0.1f);
			});
		}

		template<typename PropertyType, typename T, typename Q>
		static bool drawFloat3(std::string_view name, const std::vector<Q*>& elements, PropertyType T::* member, PropertyType defaultValue, const char** symbols, DividerType dividerType) {
			return drawMultiProperty<PropertyType, T, Q>(name, elements, member, defaultValue, dividerType, [&](PropertyType& value) -> bool {
				return propertyDrawerUtils::DragFloat3("", glm::value_ptr(value), symbols, 0.1f);
			});
		}

		template<typename PropertyType, typename T, typename Q>
		static bool drawLabel(std::string_view name, const std::vector<Q*>& elements, PropertyType T::* member, PropertyType defaultValue, DividerType dividerType) {
			return drawMultiProperty<PropertyType, T, Q>(name, elements, member, defaultValue, dividerType, [&](PropertyType& value) -> bool {
				return ImGui::InputText("", value.data(), value.capacity());
			});
		}

	private:
		static constexpr float dividerSpacing = 10.0f;

		template<typename PropertyType, typename T, typename Q>
		static bool getCommonPropertyValue(const std::vector<Q*>& components, PropertyType T::* member, PropertyType& outValue, PropertyType defaultValue) {
			if (components.empty()) return false;
			PropertyType firstValue = ((T*)components[0])->*member;
			for (size_t i = 1; i < components.size(); ++i) {
				if (!(((T*)components[i])->*member == firstValue)) {
					outValue = defaultValue;
					return false;
				}
			}
			outValue = firstValue;
			return true;
		}


		// Generalized drawMultiProperty function
		template<typename PropertyType, typename T, typename Q, typename WidgetFunc>
		static bool drawMultiProperty(std::string_view name, const std::vector<Q*>& elements, PropertyType T::* member, PropertyType defaultValue, DividerType dividerType, WidgetFunc widgetFunc) {
			// Setup code (common to all widgets)
			size_t member_hash = utils::hashMemberPointer(member);
			ImGuiID id = static_cast<ImGuiID>(member_hash);
			ImGui::PushID(id);
			auto& style = ImGui::GetStyle();
			ImVec2 startPos = ImGui::GetCursorScreenPos();
			float startY = startPos.y - ((dividerType != DividerType::TOP && dividerType != DividerType::SINGLELINE) ? style.ItemSpacing.y : 0);
			bool toRet = false;
			PropertyType value;
			bool commonValue = getCommonPropertyValue(elements, member, value, defaultValue);
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			if (!commonValue) ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);

			float availWidth = ImGui::GetContentRegionAvail().x;

			ImGui::BeginColumns(name.data(), 3, ImGuiOldColumnFlags_NoBorder);
			ImGui::SetColumnWidth(0, glm::clamp(availWidth - 300.0f, 45.0f, 300.0f));
			ImGui::SetColumnWidth(1, dividerSpacing);
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(name.data());

			// Move to the third column
			ImGui::NextColumn();
			ImGui::NextColumn();

			// Call the widget-specific function
			if (widgetFunc(value)) {
				for (Q* element : elements) {
					((T*)element)->*member = value;
				}
				toRet = true;
			}

			// Teardown code (common to all widgets)
			ImGui::EndColumns();

			ImVec2 endPos = ImGui::GetCursorScreenPos();
			float endY = ImGui::GetCursorScreenPos().y - style.ItemSpacing.y;

			float lineX = startPos.x + glm::clamp(availWidth - 300.0f, 45.0f, 300.0f) + dividerSpacing / 2;

			ImVec2 cp = ImGui::GetCursorPos();
			draw_list->AddLine(ImVec2(lineX, startY), ImVec2(lineX, endY), ImGui::GetColorU32(ImGuiCol_Border), 2.0f);
			ImGui::SetCursorPos(cp);
			if (dividerType == DividerType::BOTTOM || dividerType == DividerType::SINGLELINE) {
				draw_list->AddLine(ImVec2(startPos.x, endY + dividerSpacing / 2), ImVec2(startPos.x + availWidth, endY + dividerSpacing / 2), ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
				ImGui::Dummy(ImVec2(0, dividerSpacing / 2));
			}
			if (!commonValue) ImGui::PopItemFlag();
			ImGui::PopID();

			return toRet;
		}
	};
}
