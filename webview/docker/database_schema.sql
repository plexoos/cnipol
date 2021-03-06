CREATE DATABASE `cnipol`;
USE `cnipol`;

CREATE TABLE IF NOT EXISTS `fill_polar` (
  `fill` int(10) unsigned NOT NULL DEFAULT '0',
  `start_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `type` smallint(6) NOT NULL DEFAULT '-1',
  `beam_energy` float NOT NULL DEFAULT '0',
  `polar_blue_p0` decimal(18,9) NOT NULL DEFAULT '-1.000000000',
  `polar_blue_p0_err` float NOT NULL DEFAULT '-1',
  `polar_blue_decay` float NOT NULL DEFAULT '-1',
  `polar_blue_decay_err` float NOT NULL DEFAULT '-1',
  `polar_yellow_p0` float NOT NULL DEFAULT '-1',
  `polar_yellow_p0_err` float NOT NULL DEFAULT '-1',
  `polar_yellow_decay` float NOT NULL DEFAULT '-1',
  `polar_yellow_decay_err` float NOT NULL DEFAULT '-1',
  `polar_blue_hjet` float NOT NULL DEFAULT '-1',
  `polar_blue_hjet_err` float NOT NULL DEFAULT '-1',
  `polar_yellow_hjet` float NOT NULL DEFAULT '-1',
  `polar_yellow_hjet_err` float NOT NULL DEFAULT '-1',
  `polar_blue_1` float NOT NULL DEFAULT '-1',
  `polar_blue_1_err` float NOT NULL DEFAULT '-1',
  `polar_blue_2` float NOT NULL DEFAULT '-1',
  `polar_blue_2_err` float NOT NULL DEFAULT '-1',
  `polar_yellow_1` float NOT NULL DEFAULT '-1',
  `polar_yellow_1_err` float NOT NULL DEFAULT '-1',
  `polar_yellow_2` float NOT NULL DEFAULT '-1',
  `polar_yellow_2_err` float NOT NULL DEFAULT '-1',
  PRIMARY KEY (`fill`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `fill_polar_new` (
  `fill` int(10) unsigned NOT NULL DEFAULT '0',
  `polarimeter_id` tinyint(4) NOT NULL DEFAULT '-1',
  `ring_id` tinyint(4) NOT NULL DEFAULT '-1',
  `start_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `stop_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `type` smallint(6) NOT NULL DEFAULT '-1',
  `beam_energy` float NOT NULL DEFAULT '0',
  `polar` float NOT NULL DEFAULT '-1',
  `polar_err` float NOT NULL DEFAULT '-1',
  `polar_p0` float NOT NULL DEFAULT '-1',
  `polar_p0_err` float NOT NULL DEFAULT '-1',
  `polar_slope` float NOT NULL DEFAULT '-1',
  `polar_slope_err` float NOT NULL DEFAULT '-1',
  PRIMARY KEY (`fill`,`polarimeter_id`,`ring_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `fill_profile` (
  `fill` int(10) unsigned NOT NULL DEFAULT '0',
  `profile_blue_h` float NOT NULL DEFAULT '-1',
  `profile_blue_h_err` float NOT NULL DEFAULT '-1',
  `profile_blue_v` float NOT NULL DEFAULT '-1',
  `profile_blue_v_err` float NOT NULL DEFAULT '-1',
  `profile_yellow_h` float NOT NULL DEFAULT '-1',
  `profile_yellow_h_err` float NOT NULL DEFAULT '-1',
  `profile_yellow_v` float NOT NULL DEFAULT '-1',
  `profile_yellow_v_err` float NOT NULL DEFAULT '-1',
  `pmax_blue_h` float NOT NULL DEFAULT '-1',
  `pmax_blue_h_err` float NOT NULL DEFAULT '-1',
  `pmax_blue_v` float NOT NULL DEFAULT '-1',
  `pmax_blue_v_err` float NOT NULL DEFAULT '-1',
  `pmax_yellow_h` float NOT NULL DEFAULT '-1',
  `pmax_yellow_h_err` float NOT NULL DEFAULT '-1',
  `pmax_yellow_v` float NOT NULL DEFAULT '-1',
  `pmax_yellow_v_err` float NOT NULL DEFAULT '-1',
  PRIMARY KEY (`fill`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `fill_profile_new` (
  `fill` int(10) unsigned NOT NULL DEFAULT '0',
  `polarimeter_id` tinyint(4) NOT NULL DEFAULT '-1',
  `target_orient` tinyint(4) NOT NULL DEFAULT '-1',
  `target_id` tinyint(4) NOT NULL DEFAULT '0',
  `prof_r` float NOT NULL DEFAULT '-1',
  `prof_r_err` float NOT NULL DEFAULT '-1',
  `prof_r0` float NOT NULL DEFAULT '-1',
  `prof_r0_err` float NOT NULL DEFAULT '-1',
  `prof_rslope` float NOT NULL DEFAULT '-1',
  `prof_rslope_err` float NOT NULL DEFAULT '-1',
  `prof_pmax` float NOT NULL DEFAULT '-1',
  `prof_pmax_err` float NOT NULL DEFAULT '-1',
  PRIMARY KEY (`fill`,`polarimeter_id`,`target_orient`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `run_info` (
  `run_name` varchar(63) NOT NULL DEFAULT '',
  `polarimeter_id` smallint(6) NOT NULL DEFAULT '-1',
  `start_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `stop_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `beam_energy` float NOT NULL DEFAULT '0',
  `nevents_total` int(10) unsigned NOT NULL DEFAULT '0',
  `nevents_processed` int(10) unsigned NOT NULL DEFAULT '0',
  `nevents_silicon` int(10) unsigned,
  `polarization` float NOT NULL DEFAULT '0',
  `polarization_error` float NOT NULL DEFAULT '0',
  `phase` float NOT NULL DEFAULT '0',
  `phase_error` float NOT NULL DEFAULT '0',
  `profile_ratio` float NOT NULL DEFAULT '0',
  `profile_ratio_error` float NOT NULL DEFAULT '0',
  `target_orient` char(1) NOT NULL DEFAULT '-',
  `target_id` smallint(6) NOT NULL DEFAULT '-1',
  `asym_version` varchar(60) NOT NULL DEFAULT '',
  `alpha_calib_run_name` varchar(100) NOT NULL DEFAULT '',
  `dl_calib_run_name` varchar(100) NOT NULL DEFAULT '',
  `measurement_type` smallint(6) NOT NULL DEFAULT '-1',
  `disabled_channels` varchar(300) NOT NULL DEFAULT '',
  `disabled_bunches` varchar(400) NOT NULL DEFAULT '',
  `ana_start_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `ana_duration` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`run_name`),
  KEY `run_name` (`run_name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `run_period` (
  `start_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `polarimeter_id` smallint(6) NOT NULL DEFAULT '-1',
  `alpha_calib_run_name` varchar(100) NOT NULL DEFAULT '',
  `alpha_source_count` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `dl_calib_run_name` varchar(100) NOT NULL DEFAULT '',
  `disabled_channels` varchar(300) NOT NULL DEFAULT '',
  `disabled_bunches` varchar(400) NOT NULL DEFAULT '',
  `cut_proto_slope` float NOT NULL DEFAULT '0',
  `cut_proto_offset` float NOT NULL DEFAULT '0',
  `cut_proto_width` tinyint(3) unsigned NOT NULL DEFAULT '20',
  `cut_proto_adc_min` float NOT NULL DEFAULT '0',
  `cut_proto_adc_max` float NOT NULL DEFAULT '255',
  `cut_proto_tdc_min` float NOT NULL DEFAULT '0',
  `cut_proto_tdc_max` float NOT NULL DEFAULT '255',
  `cut_pulser_adc_min` float NOT NULL DEFAULT '255',
  `cut_pulser_adc_max` float NOT NULL DEFAULT '0',
  `cut_pulser_tdc_min` float NOT NULL DEFAULT '255',
  `cut_pulser_tdc_max` float NOT NULL DEFAULT '0',
  UNIQUE KEY `start_time_2` (`start_time`,`polarimeter_id`),
  KEY `start_time` (`start_time`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
