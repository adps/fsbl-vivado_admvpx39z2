--Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2018.1 (win64) Build 2188600 Wed Apr  4 18:40:38 MDT 2018
--Date        : Tue Jun 12 16:29:25 2018
--Host        : chris-pc running 64-bit major release  (build 9200)
--Command     : generate_target simple_9z2_wrapper.bd
--Design      : simple_9z2_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity simple_9z2_wrapper is
  port (
    P2_AUX_EN_L : out STD_LOGIC_VECTOR ( 0 to 0 );
    P2_GP_EN_L : out STD_LOGIC_VECTOR ( 0 to 0 );
    REFCLK_100M1_clk_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    REFCLK_100M1_clk_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    dp_aux_data_in : in STD_LOGIC;
    dp_aux_data_oe : out STD_LOGIC_VECTOR ( 0 to 0 );
    dp_aux_data_out : out STD_LOGIC;
    dp_hot_plug_detect : in STD_LOGIC
  );
end simple_9z2_wrapper;

architecture STRUCTURE of simple_9z2_wrapper is
  component simple_9z2 is
  port (
    REFCLK_100M1_clk_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    REFCLK_100M1_clk_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    dp_aux_data_in : in STD_LOGIC;
    dp_hot_plug_detect : in STD_LOGIC;
    P2_GP_EN_L : out STD_LOGIC_VECTOR ( 0 to 0 );
    P2_AUX_EN_L : out STD_LOGIC_VECTOR ( 0 to 0 );
    dp_aux_data_out : out STD_LOGIC;
    dp_aux_data_oe : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  end component simple_9z2;
begin
simple_9z2_i: component simple_9z2
     port map (
      P2_AUX_EN_L(0) => P2_AUX_EN_L(0),
      P2_GP_EN_L(0) => P2_GP_EN_L(0),
      REFCLK_100M1_clk_n(0) => REFCLK_100M1_clk_n(0),
      REFCLK_100M1_clk_p(0) => REFCLK_100M1_clk_p(0),
      dp_aux_data_in => dp_aux_data_in,
      dp_aux_data_oe(0) => dp_aux_data_oe(0),
      dp_aux_data_out => dp_aux_data_out,
      dp_hot_plug_detect => dp_hot_plug_detect
    );
end STRUCTURE;
