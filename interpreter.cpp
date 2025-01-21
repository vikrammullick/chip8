#include "interpreter.hpp"
#include "sdl_helpers.hpp"

using namespace std;

interpreter_t::interpreter_t(const vector<char> &rom_bytes)
    : m_address_decoder(m_bus),
      m_memory(rom_bytes, m_bus),
      m_ppu(m_bus, m_memory),
      m_keyboard(m_bus),
      m_delay_timer(m_bus, constants::DELAY_TIMER_CHIP_SELECT),
      m_sound_timer(m_bus, constants::SOUND_TIMER_CHIP_SELECT),
      m_rng(m_bus),
      m_cpu(m_bus) {
    sdl_init_window();
}

interpreter_t::~interpreter_t() { sdl_destroy_window(); }

uint64_t now() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

void interpreter_t::run() {
    uint64_t last_ts = now();
    while (g_running) {
        uint64_t current_ts = now();
        if ((current_ts - last_ts) <
            (constants::MICROS_PER_SEC / constants::INTERPRETER_CLOCK_RATE)) {
            continue;
        }
        last_ts = current_ts;

        // combinational logic
        m_address_decoder.select_chip();
        m_memory.service_request();
        m_ppu.service_request();
        m_keyboard.service_request();
        m_delay_timer.service_request();
        m_sound_timer.service_request();
        m_rng.service_request();

        // sequential logic
        m_cpu.tick();
        m_ppu.tick();
        m_delay_timer.tick();
        m_sound_timer.tick();

        sdl_poll_keyboard();
    }
}
