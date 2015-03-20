<?php

// constants to workaround runconfig.php contents
define('nan', NAN);
define('inf', INF);

include_once("Pair.php");


/**
 *    Get the ordinal suffix of an int (e.g. th, rd, st, etc.)
 *     @param  int   $n
 *     @return string   $n + $n's ordinal suffix
 */
function ordinal_suffix($n) {
     $n_last = $n % 100;
     if (($n_last > 10 && $n_last < 14) || $n == 0){
          return "{$n}th";
     }
     switch(substr($n, -1)) {
          case '1':    return "{$n}st";
          case '2':    return "{$n}nd";
          case '3':    return "{$n}rd";
          default:     return "{$n}th";
     }
}


function dateTimeDiff($startStamp, $endStamp)
{
   $d = $endStamp - $startStamp;

   if ($d < 0) return "";

   $secondsInMin  = 60;
   $secondsInHour = 3600;
   $secondsInDay  = 86400;

   $fullDays  = floor($d/$secondsInDay);
   $d = $d % $secondsInDay;
   $fullHours = floor($d/$secondsInHour);
   $d = $d % $secondsInHour;
   $fullMins  = floor($d/$secondsInMin);
   $d = $d % $secondsInMin;
   $fullSecs  = $d;

   $diff  = $fullDays ? "{$fullDays}d:" : "";
   $diff .= $fullHours ? "{$fullHours}h:" : "";
   $diff .= $fullMins ? "{$fullMins}m:" : "";
   $diff .= $fullSecs ? "{$fullSecs}s" : "";

   return $diff;
}


/** */
function exportMysqlToCsv($table, $sqlWhere="", $filename='rundb.csv')
{
   $csv_terminated = "\n";
   $csv_separator = ",";
   $csv_enclosed = '"';
   $csv_escaped = "\\";
   $sql_query = "SELECT * FROM $table WHERE $sqlWhere";

   // Gets the data from the database
   $result = mysql_query($sql_query);
   $fields_cnt = mysql_num_fields($result);

   $schema_insert = '';

   for ($i = 0; $i < $fields_cnt; $i++)
   {
      $l = $csv_enclosed . str_replace($csv_enclosed, $csv_escaped . $csv_enclosed,
          stripslashes(mysql_field_name($result, $i))) . $csv_enclosed;
      $schema_insert .= $l;
      $schema_insert .= $csv_separator;
   }

   $out = trim(substr($schema_insert, 0, -1));
   //$out .= "\n".$sql_query."\nttt\n";
   $out .= $csv_terminated;

   // Format the data
   while ($row = mysql_fetch_array($result))
   {
       $schema_insert = '';
       for ($j = 0; $j < $fields_cnt; $j++) {
           if ($row[$j] == '0' || $row[$j] != '') {

               if ($csv_enclosed == '') {
                   $schema_insert .= $row[$j];
               } else {
                   $schema_insert .= $csv_enclosed .
                      str_replace($csv_enclosed, $csv_escaped . $csv_enclosed, $row[$j]) . $csv_enclosed;
               }
           } else {
               $schema_insert .= '';
           }

           if ($j < $fields_cnt - 1) {
               $schema_insert .= $csv_separator;
           }
       }

       $out .= $schema_insert;
       $out .= $csv_terminated;
   }

   header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
   header("Content-Length: " . strlen($out));
   // Output to browser with appropriate mime type, you choose ;)
   header("Content-type: text/x-csv");
   //header("Content-type: text/csv");
   //header("Content-type: application/csv");
   header("Content-Disposition: attachment; filename=$filename");
   echo $out;
   exit;
}


/** */
function exportMysqlToCsv2($table, $sqlWhere="", $filename='rundb.csv')
{
   $csv_terminated = "\n";
   $csv_separator = ",";
   $csv_enclosed = '"';
   $csv_escaped = "\\";
   $sql_query = "SELECT * FROM $table WHERE $sqlWhere";

   // Gets the data from the database
   $result = mysql_query($sql_query);
   $fields_cnt = mysql_num_fields($result);

   $schema_insert = '';

   for ($i = 0; $i < $fields_cnt; $i++)
   {
      $l = $csv_enclosed . str_replace($csv_enclosed, $csv_escaped . $csv_enclosed,
          stripslashes(mysql_field_name($result, $i))) . $csv_enclosed;
      $schema_insert .= $l;
      $schema_insert .= $csv_separator;
   }

   $out = trim(substr($schema_insert, 0, -1));
   //$out .= "\n".$sql_query."\nttt\n";
   $out .= $csv_terminated;

   // Format the data
   while ($row = mysql_fetch_array($result))
   {
       $schema_insert = '';
       for ($j = 0; $j < $fields_cnt; $j++) {
           if ($row[$j] == '0' || $row[$j] != '') {

               if ($csv_enclosed == '') {
                   $schema_insert .= $row[$j];
               } else {
                   $schema_insert .= $csv_enclosed .
                      str_replace($csv_enclosed, $csv_escaped . $csv_enclosed, $row[$j]) . $csv_enclosed;
               }
           } else {
               $schema_insert .= '';
           }

           if ($j < $fields_cnt - 1) {
               $schema_insert .= $csv_separator;
           }
       }

       $out .= $schema_insert;
       $out .= $csv_terminated;
   }

   header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
   header("Content-Length: " . strlen($out));
   // Output to browser with appropriate mime type, you choose ;)
   header("Content-type: text/x-csv");
   //header("Content-type: text/csv");
   //header("Content-type: application/csv");
   header("Content-Disposition: attachment; filename=$filename");
   echo $out;
   exit;
}


/** */
//function floatToString($val, $err)
//{
//   $str = "&nbsp;";
//
//   if ( $val >= 0 && $err >= 0 )
//    $str = sprintf("%5.2f &plusmn; %5.2f", $val*100, $err*100);
//
//   return $str;
//}


/** */
function pairToString($valerr=null, $classes="", $format="%3.2f", $isHtml=true)
{
   if ($valerr == null || $valerr->second < 0) return $isHtml ? "&nbsp;" : "";

   $str = "";

   if ($isHtml)
      $str = sprintf("$format&nbsp;&plusmn;&nbsp;$format", $valerr->first, $valerr->second);
   else
      $str = sprintf("$format +- $format", $valerr->first, $valerr->second);
  
   if (!empty($classes) && $isHtml)
     $str = "<span class='$classes'>$str</span>";
   
   return $str;
}


/** */
//function polarToString($val, $err)
//{
//}


/** */
function polarPairToString($valerr=null, $classes="", $format="%3.2f", $isHtml=true)
{
   if (empty($valerr))
      return "";

   $ve_scaled = $valerr->ClonePair();

   $ve_scaled->val *= 100.;
   $ve_scaled->err *= 100.;

   return pairToString($ve_scaled, $classes, $format, $isHtml);
}


/** */
function calcWeigtedSum($valerrs, $power=0)
{

   if ($power != 0 && $power != 1 && $power != 2) return null;
   if (count($valerrs) <= 0) return null;

   $sum = 0;

   foreach($valerrs as $valerr) {

      if ($valerr == null) continue;
      //if ($valerr->second < 0) continue;

      $val = $valerr->first;
      $err = $valerr->second == 0 ? 1E-50 : $valerr->second;

      $w   = 1./$err/$err;
      //print($w."<br>");

      if ($power == 0)
         $sum += $w ;

      if ($power == 1)
         $sum += $w*$val;

      if ($power == 2)
         $sum += $w*$val*$val;

      //print($sum."<br>");
   }

   return $sum;
}


/** */
function calcWeigtedAvrgErr($valerrs)
{
   $result = new pair(-1, -1);

   //if (count($valerrs) <= 0) return $result;
   if (count($valerrs) <= 0) return null;

   $sum1   = 0;
   $sum2   = 0;

   foreach($valerrs as $valerr) {

      if ($valerr == null) continue;

      if ($valerr->second < 0) continue;

      $val = $valerr->first;
      $err = $valerr->second == 0 ? 1E-50 : $valerr->second;

      $err2  = $err*$err;
      $sum1 += $val/$err2 ;
      $sum2 += 1./$err2 ;
   }

   $result->first  = $sum1/$sum2;
   $result->second = ($sum2 == 0 ? -1 : sqrt(1./$sum2));

   return $result;
}


/** */
function calcWeigtedAvrgErrPairs($ve1, $ve2)
{
   $result = new pair(0, -1);

   if ($ve1->second < 0 && $ve2->second < 0 ) return $result;
   if ($ve1->second < 0 ) return $ve2;
   if ($ve2->second < 0 ) return $ve1;

   $w1 = 1./$ve1->second/$ve1->second;
   $w2 = 1./$ve2->second/$ve2->second;

   $result->first = ($ve1->first*$w1 + $ve2->first*$w2) / ($w1 + $w2);
   $result->second = 1./sqrt($w1 + $w2);

   return $result;
}


/** */
function calcWeigtedStdDev($valerrs)
{
   //if (count($valerrs) <= 0) return null;

   $result = new pair(0, -1);

   $s0 = calcWeigtedSum($valerrs, 0);
   $s1 = calcWeigtedSum($valerrs, 1);
   $s2 = calcWeigtedSum($valerrs, 2);

   if (!$s0) return $result;

   $result->first  = sqrt( ($s0*$s2 - $s1*$s1) / ( $s0*($s0 - 1) ) );
   //$result->first  = 777;
   $result->second = 0;

   return $result;
}


/** */
function calcMaxDifference($avrg=null, $valerrs=null)
{
   $maxDiff = -1;

   if ($avrg == null) return -1;

   //if (count($valerrs) <= 0) return $maxDiff;
   if (count($valerrs) <= 0) return -1;

   foreach($valerrs as $valerr) {

      if ($valerr == null) continue;

      //if ($valerr->second < 0) continue;

      $diff = abs($avrg->first - $valerr->first);

      //if ($diff > 0 && $diff > $maxDiff) $maxDiff = $diff;
      if ($diff > $maxDiff) $maxDiff = $diff;
   }

   $maxDiff = $maxDiff < 0.0000000001 ? -1 : $maxDiff;

   return $maxDiff;
}


/** */
function calcPolarCollisionScale($pairR_H=null, $pairR_V=null)
{
   if ($pairR_H == null || $pairR_V == null) return null;

   // protected for division y zero
   $pairR_V_first = $pairR_V->first == -2 ? 0 : $pairR_V->first;
   $pairR_H_first = $pairR_H->first == -2 ? 0 : $pairR_H->first;

   $collScale = new pair(-1, -1);

   $collScale->first = sqrt((1+$pairR_H_first)/(1+0.5*$pairR_H_first)) * sqrt((1+$pairR_V_first)/(1+0.5*$pairR_V_first));

   $relDelta2  = $pairR_H->second * $pairR_H->second / (2 + $pairR_H_first) / (2 + $pairR_H_first)/ 4.;
   $relDelta2 += $pairR_V->second * $pairR_V->second / (2 + $pairR_V_first) / (2 + $pairR_V_first)/ 4.;

   $collScale->second = $collScale->first * sqrt($relDelta2);

   return $collScale;
}


/** */
function calcDSAPolarCollScale($profRsHorz=array(), $profRsVert=array())
{
   $result = new pair(0, -1);

   //foreach ($profR) {
   //}

   if ($profRsHorz == null || $profRsVert == null) return null;

   // protected for division y zero
   $pairR_V_first = $profRsVert->first == -2 ? 0 : $profRsVert->first;
   $pairR_H_first = $profRsHorz->first == -2 ? 0 : $profRsHorz->first;


   $result->first = sqrt((1+$pairR_H_first)/(1+0.5*$pairR_H_first)) * sqrt((1+$pairR_V_first)/(1+0.5*$pairR_V_first));

   $relDelta2  = $profRsHorz->second * $profRsHorz->second / (2 + $pairR_H_first) / (2 + $pairR_H_first)/ 4.;
   $relDelta2 += $profRsVert->second * $profRsVert->second / (2 + $pairR_V_first) / (2 + $pairR_V_first)/ 4.;

   $result->second = $result->first * sqrt($relDelta2);

   return $result;
}


/** */
function calcPolarCollision($polar=null, $scaleColl=null)
{
   $result = new pair(0, -1);

   if ($polar == null || $scaleColl == null) return $polar->ClonePair();
   if ($polar->second < 0 || $scaleColl->second < 0) return $polar->ClonePair();

   $result->first = $polar->first * $scaleColl->first;

   $relDelta2 = $polar->second * $polar->second / $polar->first / $polar->first + $scaleColl->second * $scaleColl->second / $scaleColl->first / $scaleColl->first;
   $result->second = abs($result->first) * sqrt($relDelta2);

   return $result;
}


/** */
function calcPolarP0Collision($polarP0=null, $profR0Horz=null, $profR0Vert=null)
{
   $result = new pair(0, -1);

   //if ($polarP0 == null || $profR0Horz == null || $profR0Vert == null)             return $polarP0->ClonePair();
   //if ($polarP0->second < 0 || $profR0Horz->second < 0 || $profR0Vert->second < 0) return $polarP0->ClonePair();

   $P0  = $polarP0->first;
   $R_H = $profR0Horz->first; 
   $R_V = $profR0Vert->first;

   $result->first = 2 * $P0 * sqrt( (1+$R_H)/(2+$R_H) ) * sqrt( (1+$R_V)/(2+$R_V) );
 
   return $result;
}


/** */
function calcPolarSlopeCollision($polarP0=null, $polarSlope=null, $profR0=null, $profRSlope=null)
{
   $result = new pair(0, -1);

   //if ($polarP0 == null || $polarSlope == null || $profR0 == null || $profRSlope == null)  return $polarP0->ClonePair();
   //if ($polarP0->second < 0 || $profR0Horz->second < 0 || $profR0Vert->second < 0) return $polarP0->ClonePair();

   $P0       = $polarP0->first;
   $PSlope   = $polarSlope->first;
   $R0_H     = $profR0[0]->first;
   $R0_V     = $profR0[1]->first;
   $RSlope_H = $profRSlope[0]->first;
   $RSlope_V = $profRSlope[1]->first;

   $result->first  = 2 * $PSlope * sqrt( (1+$R_H)/(2+$R_H) ) * sqrt( (1+$R_V)/(2+$R_V) );
   $numer = RSlope_H*(2 + 3*R0_V + R0_V*R0_V) + RSlope_V*(2 + 3*R0_H + R0_H*R0_H);
   $denom = sqrt(1 + R0_H) * sqrt( (2 + R0_H)*(2 + R0_H)*(2 + R0_H) )
          * sqrt(1 + R0_V) * sqrt( (2 + R0_V)*(2 + R0_V)*(2 + R0_V) );
   $result->first += P0*$numer/$denom;

   return $result;
}


/** */
function calcQuantileValue(&$vals=null, $quantile=null)
{

   if ($quantile == null) return end($vals);

   sort($vals);

   $n = count($vals);
   $i = 0;

   foreach ($vals as $val) {

      $i++;

      if ( ( (double)  $i/$n) >= $quantile)
         return $val;
   }

   return end($vals);
}


/** */
function calcProfPolar($profPMax=null, $profR=null)
{
   if ($profPMax == null || $profR == null) return null;

   $profPolar = new pair(-1, -1);

   $profPolar->first = $profPMax->first / sqrt( 1 + $profR->first);

   //$relDelta2 = $profPMax->second * $profPMax->second / $profPMax->first / $profPMax->first + $profR->second * $profR->second / $profR->first / $profR->first;
   //$profPolar->second = $profPolar->first * sqrt($relDelta2);

   $delta2 = $profPMax->first  * $profPMax->first  * $profR->second * $profR->second/4./(1+$profR->first)/(1+$profR->first)/(1+$profR->first) +
             $profPMax->second * $profPMax->second / (1 + $profR->first);
   $profPolar->second = sqrt($delta2);

   return $profPolar;
}


/** */
function calcDivision($ve1, $ve2, $r12=0)
{
   $result = new pair(0, -1);

   if ($ve1->first == 0 || $ve2->first == 0) return $result;

   $result->first  = $ve1->first  / $ve2->first;
   $re1            = $ve1->second / $ve1->first;
   $re2            = $ve2->second / $ve2->first;
   $re             = sqrt($re1*$re1 + $re2*$re2 - 2*$re1*$re2*$r12);
   $result->second = $re * $result->first;

   return $result;
}


/** */
function readOnlinePolar($fileNameFull="")
{
   $values = array();
   $handle = fopen($fileNameFull, "r");

   if ($handle === FALSE) return $values;

   //$row = 1;

   while (($data = fgetcsv($handle, 100, ",")) !== FALSE)
   {
       $nElements = count($data);
       if ($nElements == 3)
          $values[$data[0]] = new pair($data[1], $data[2]);
   }

   fclose($handle);
   return $values;
}


/** */
function getJetCarbonNormalization($normSpecs=null)
{
   global $normJetCarbonByTarget;

   //print "<!--\n";
   //print_r($normJetCarbonByTarget);
   //print "-->\n";

   //$specs = array("runPeriod", "energy", "polId", "tgtOrient", "tgtId");

   $currSpec = $normJetCarbonByTarget;
   if ( !is_array($currSpec) && is_numeric($currSpec) ) return $currSpec;

   foreach ($normSpecs as $specKey => $specVal)
   {
      if ( !array_key_exists($specVal, $currSpec) ) {
         //print("key not found {$specVal}\n");
         return 1; // key not found: may also want to issue a warning
      }

      // select curr spec go deeper
      $currSpec = $currSpec[$specVal];

      //print("$specKey = $specVal\n");
      //print_r($currSpec);

      if ( !is_array($currSpec) && is_numeric($currSpec) ) return $currSpec;
   }

   return 1;
}


/** */
function getHJPCNorm($run, $benergy, $polId, $tgtOrient=null, $tgtId=null)
{
   global $normHJPC;

   //print "<!--\n";
   //print "$run, $benergy, $polId, $tgtOrient, $tgtId\n";
   //print_r($normHJPC);
   //print "-->\n";

   if (is_numeric($tgtOrient) && $tgtOrient == 0)
      $tgtOrient = "H";
   else if (is_numeric($tgtOrient) && $tgtOrient == 1)
      $tgtOrient = "V";
   else 
      $tgtOrient = null;

   //$tgtOrient = $tgtOrient == 0 ? "H" : ($tgtOrient == 1 ? "V" : null);

   $norm = $normHJPC[$run][$benergy][$polId][$tgtOrient][$tgtId];

   if ( isset($norm) && get_class($norm) == "pair" ) return $norm;

   $norm = $normHJPC[$run][$benergy][$polId]['nomi'];

   if ( isset($norm) && get_class($norm) == "pair" ) return $norm;

   return new pair(1, -1);
}


/** */
function getRunPeriod($timestamp=null)
{
   global $RUN_PERIOD_BY_DATE;

   foreach ($RUN_PERIOD_BY_DATE as $runPeriod => $runTimes) {

//print "<!-- runperiod\n";
// $start = strtotime($runTimes["start"]);
// $end   = strtotime($runTimes["end"]) ;
//print("hhh: $timestamp $start $end ");
//print "-->\n";

      if ( $timestamp >= strtotime($runTimes["start"]) && $timestamp <= strtotime($runTimes["end"]) )
         return $runPeriod;
   }

}


/** */
function GetRingId($polId)
{
   switch ($polId) {
   case 0:
      return 1;
   case 1:
      return 2;
   case 2:
      return 1;
   case 3:
      return 2;
   default:
      return -1;
   }
}

function do_offset($level){
    $offset = "";             // offset for subarry 
    for ($i=1; $i<$level;$i++){
    $offset = $offset . "<td></td>";
    }
    return $offset;
}

function show_array($array, $level, $sub){
    if (is_array($array) == 1){          // check if input is an array
       foreach($array as $key_val => $value) {
           $offset = "";
           if (is_array($value) == 1){   // array is multidimensional
           echo "<tr>";
           $offset = do_offset($level);
           echo $offset . "<td>" . $key_val . "</td>";
           show_array($value, $level+1, 1);
           }
           else{                        // (sub)array is not multidim
           if ($sub != 1){          // first entry for subarray
               echo "<tr nosub>";
               $offset = do_offset($level);
           }
           $sub = 0;
           echo $offset . "<td main ".$sub." width=\"120\">" . $key_val . 
               "</td><td width=\"120\">" . pairToString($value, "", "%8.7f") . "</td>"; 
           echo "</tr>\n";
           }
       } //foreach $array
    }  
    else{ // argument $array is not an array
        return;
    }
}


function flatten($array) {
    $return = array();
    array_walk_recursive($array, function($a) use (&$return) { $return[] = $a; });
    return $return;
}


function makeNonNestedRecursive(array &$out, $key, array $in){
    foreach($in as $k=>$v){
    	if(is_array($v)){
    	    makeNonNestedRecursive($out, $key . $k . ',', $v);
    	}else{
            $out[$key . $k] = $v;
    	}
    }
}

function makeNonNested(array $in){
    $out = array();
    makeNonNestedRecursive($out, '', $in);
    return $out;
}

?>
