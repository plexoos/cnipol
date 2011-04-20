<?php

define("DATA_DIR", "/usr/local/polarim/root");

$POLARIMETER_ID = array(0 => "B1U", 1 => "Y1D", 2 => "B2D", 3 => "Y2U");
$RHIC_BEAM      = array("1" => "Yellow", "2" => "Blue");
$RHIC_STREAM    = array("1" => "Upstream", "2" => "Downstream");
$MEASTYPE       = array(0x00 => "undef", 0x01 => "alpha", 0x02 => "sweep", 0x04 => "fixed", 0x08 => "ramp", 0x10 => "emit");
$TARGET_ORIENT  = array("H" => "Horizontal", "V" => "Vertical");
$TARGET_ID      = range(1, 6);

?>
