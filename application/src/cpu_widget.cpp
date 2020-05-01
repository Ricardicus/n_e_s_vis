#include "cpu_widget.h"

#include "control.h"
#include "nes/core/immu.h"
#include "nes/core/imos6502.h"
#include "nes/core/opcode.h"
#include "nes/nes.h"

#include "imgui-SFML.h"
#include "imgui.h"

#include <cinttypes>

namespace nesvis {

CpuWidget::CpuWidget(n_e_s::nes::Nes *nes, Control *control)
        : nes_{nes}, control_{control} {}

void CpuWidget::draw() {
    ImGui::Begin("Nes cpu");

    if (control_->is_running()) {
        if (ImGui::Button("Pause", {50.f, 0.f})) {
            control_->pause();
        }
    } else {
        if (ImGui::Button("Run", {50.f, 0.f})) {
            control_->run();
        }
    }
    ImGui::SameLine();

    if (ImGui::Button("Reset", {50.f, 0.f})) {
        nes_->reset();
    }

    ImGui::Spacing();

    ImVec2 button_size{100.f, 0.f};
    if (ImGui::Button("Stepi", button_size)) {
        control_->stepi();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stepi 1000", button_size)) {
        for (int i = 0; i < 1000; ++i) {
            control_->stepi();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stepi 10000", button_size)) {
        for (int i = 0; i < 10000; ++i) {
            control_->stepi();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stepi 100000", button_size)) {
        for (int i = 0; i < 100000; ++i) {
            control_->stepi();
        }
    }

    if (ImGui::Button("Step", button_size)) {
        control_->step();
    }
    ImGui::SameLine();
    if (ImGui::Button("Step 10", button_size)) {
        for (int i = 0; i < 10; ++i) {
            control_->step();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Step 1000", button_size)) {
        for (int i = 0; i < 1000; ++i) {
            control_->step();
        }
    }

    ImGui::Spacing();

    if (ImGui::Button("Jump to addr", button_size)) {
        nes_->reset();
        nes_->cpu_registers().pc = jump_to_address_;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(button_size.x);
    ImGui::InputScalar("",
            ImGuiDataType_U16,
            &jump_to_address_,
            nullptr,
            nullptr,
            "%04hx",
            ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::Spacing();

    ImGui::Text("Curr cycle: %" PRIu64, nes_->current_cycle());

    const auto reg = nes_->cpu_registers();

    ImGui::Text("%s: %04hX", "pc", reg.pc);
    ImGui::SameLine();
    ImGui::Text("%s: %02hhX", "a", reg.a);
    ImGui::SameLine();
    ImGui::Text("%s: %02hhX", "x", reg.x);
    ImGui::SameLine();
    ImGui::Text("%s: %02hhX", "y", reg.y);
    ImGui::SameLine();
    ImGui::Text("%s: %02hhX", "p", reg.p);
    ImGui::SameLine();
    ImGui::Text("%s: %02hhX", "sp", reg.sp);

    ImGui::Spacing();

    const uint16_t pc_start = nes_->cpu().state().start_pc;
    ImGui::Text("Mem at PC: %02hhX %02hhX %02hhX",
            nes_->mmu().read_byte(pc_start),
            nes_->mmu().read_byte(pc_start + 1),
            nes_->mmu().read_byte(pc_start + 2));

    const auto opcode = n_e_s::core::decode(nes_->mmu().read_byte(pc_start));
    const auto family = std::string(n_e_s::core::to_string(opcode.family));
    ImGui::Text("Instruction: %s", family.c_str());

    ImGui::Separator();

    ImGui::Text("Stack contents:");
    if (ImGui::BeginChild("stack_content", {0, 200})) {
        for (int i = reg.sp + 1; i <= 0xFF; ++i) {
            ImGui::Text("%02X: %02hhX", i, nes_->mmu().read_byte(0x0100 + i));
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

} // namespace nesvis
