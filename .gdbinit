# --- Connection ---
target extended-remote | openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c "gdb_port pipe; log_output openocd.log"
monitor reset halt
load
break main
continue

# --- Appearance ---
tui enable
tui new-layout sidebyside {-horizontal src 1 asm 1} 2 status 0 cmd 1
layout sidebyside

winheight src 16
winheight asm 16

set print pretty on
