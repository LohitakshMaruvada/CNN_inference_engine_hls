# =========================================================
# Vitis HLS automation script for CNN_Top
# =========================================================

# -------- USER EDITABLE PATHS --------
set proj_name   "PE"
set proj_dir    "./cnn_hls_proj"

# Source files (edit paths/names as needed)
set src_files [list \
    "./src/cnn_top.cpp" \
    "./src/processing_element.cpp" \
    "./src/agu.cpp" \
    "./src/line_memory.cpp" \
    "./src/minmax.cpp" \
]

# Optional testbench (comment out if you don’t have one)
set tb_files [list \
    "./cnn_tb.cpp" \
]

# Target device (ZCU102)
set part_name  "xczu1cg-sbva484-1-e"
set clk_period 5.0   ;# ns (200 MHz) – adjust as needed

# =========================================================
# 1. Create project and add files
# =========================================================
open_project -reset $proj_name
set_top CNN_Top

# Add source files
foreach f $src_files {
    add_files $f
}

# Add testbench files
foreach f $tb_files {
    if {[file exists $f]} {
        add_files -tb $f
    }
}

# Set project directory
set_property project.dir $proj_dir [current_project]

# =========================================================
# 2. Create solution, set device and clock
# =========================================================
open_solution -reset "solution1"
set_part $part_name
create_clock -period $clk_period -name default

# (Optional) set C standard / flow options here if needed
# config_compile -enable_auto_pipeline

# =========================================================
# 3. Run C simulation
# =========================================================
# csim_design -clean

# =========================================================
# 4. Run C synthesis
# =========================================================
csynth_design

# =========================================================
# 5. Export RTL as IP
# =========================================================
set export_dir "./export_ip"
file mkdir $export_dir

export_design \
    -format ip_catalog \
    -rtl verilog \
    -output $export_dir

puts "INFO: HLS flow complete. IP exported to $export_dir"

exit

