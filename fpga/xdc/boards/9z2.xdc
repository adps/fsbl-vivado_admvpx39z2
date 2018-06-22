set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

set_property PACKAGE_PIN E17 [get_ports [list {REFCLK_100M1_clk_p[0]}]]
set_property IOSTANDARD LVDS_25 [get_ports [list {REFCLK_100M1_clk_p[0]}]]
create_clock -period 10.000 -name {REFCLK_100M1_clk_p[0]} -waveform {0.000 5.000} [get_ports {REFCLK_100M1_clk_p[0]}]

set_property PACKAGE_PIN F12 [get_ports dp_aux_data_in]
set_property PACKAGE_PIN G11 [get_ports dp_aux_data_oe]
set_property PACKAGE_PIN D10 [get_ports dp_aux_data_out]
set_property PACKAGE_PIN H11 [get_ports dp_hot_plug_detect]
set_property IOSTANDARD LVCMOS33 [get_ports dp_aux_data_out]
set_property IOSTANDARD LVCMOS33 [get_ports dp_aux_data_oe]
set_property IOSTANDARD LVCMOS33 [get_ports dp_hot_plug_detect]
set_property IOSTANDARD LVCMOS33 [get_ports dp_aux_data_in]

set_property PACKAGE_PIN AN13 [get_ports P2_AUX_EN_L]
set_property PACKAGE_PIN AN14 [get_ports P2_GP_EN_L]
set_property IOSTANDARD LVCMOS33 [get_ports P2_AUX_EN_L]
set_property IOSTANDARD LVCMOS33 [get_ports P2_GP_EN_L]

set_property OFFCHIP_TERM NONE [get_ports dp_aux_data_oe]
set_property OFFCHIP_TERM NONE [get_ports dp_aux_data_out]
set_property PULLUP true [get_ports dp_aux_data_in]
set_property PULLUP true [get_ports dp_aux_data_oe]
set_property PULLUP true [get_ports dp_aux_data_out]
set_property PULLUP true [get_ports dp_hot_plug_detect]


