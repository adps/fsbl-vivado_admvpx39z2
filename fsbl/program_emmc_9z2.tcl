connect
exec sleep 4
#jtag frequency 12000000
source D:/Xilinx/SDK/2017.4/scripts/sdk/util/zynqmp_utils.tcl
# #################################
#Load PMU Firmware
# #################################

targets -set 4
disable_pmu_gate
exec sleep 1
targets -set -nocase -filter {name =~"PSU*" && jtag_cable_name =~ "Platform Cable USB II 000013ccfc6f01"} -index 1
exec sleep 1
targets -set 13
exec sleep 1
dow ./sources/emmc_programming/9z2_pmufw.elf
con

# #################################
# Load fsbl+u-boot
# #################################
targets -set -filter {name =~ "PSU"}
# write bootloop and release A53-0 reset
mwr 0xffff0000 0x14000000
mwr 0xFD1A0104 0x380E
exec sleep 1
# download and run FSBL
targets -set -filter {name =~ "Cortex-A53 #0"}
# downloading FSBL
dow ./sources/emmc_programming/9z2_fsbl.elf
con
exec sleep 8
stop
exec sleep 1
mwr 0xFF18031C 0x64406440  
mwr 0xFF180314 0x01150000        
mwr 0xFF180318 0x00450043
exec sleep 1
dow F:/svn_repo/ch_repo/ZynqMPSoC/xsdk/build_bootimg/u-boot.elf
dow F:/svn_repo/ch_repo/ZynqMPSoC/xsdk/build_bootimg/bl31.elf
#dow -data  F:/svn_repo/ch_repo/ZynqMPSoC/xsdk/15484_small_part.bin 0x8000
dow -data  F:/svn_repo/ch_repo/ZynqMPSoC/xsdk/build_bootimg/emmc_base.img 0x8000    
con              