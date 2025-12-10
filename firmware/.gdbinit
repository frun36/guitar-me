# --- Connection ---
target extended-remote | openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c "gdb_port pipe; log_output openocd.log"
monitor reset halt
load
break main
continue

# --- Dashboard ---
source ~/.gdbinit
dashboard -layout source assembly registers stack variables expressions
