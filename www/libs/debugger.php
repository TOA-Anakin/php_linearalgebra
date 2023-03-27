<?php
class debugger {
	static private $marks=0;
	static private $time=array();
	static private $queries=array();

	static public $debug = array();
	static public $debugMode = false;

	static function addDebug($name, $value, $max = -1) {
		if(self::$debugMode === false) return;
		if(!isset(self::$debug[$name])) self::$debug[$name] = array();
		if($max > 0 && count(self::$debug[$name]) >= $max) return;
		self::$debug[$name][] = $value;
	}

	static function getDebug($name = false) {
		if(self::$debugMode === false) return;
		$tmp = array();
		foreach(self::$debug as $key => $val) {
			$tmp = self::getDebugTree($tmp, explode(".", $key), $val);
		}
		return $tmp;
	}

	static function getDebugTree($tmp, $tree, $val) {
		if(empty($tree)) {
			return $val;
		}
		foreach($tree as $key => $name) {
			if(!isset($tmp[$name])) $tmp[$name] = array();
			unset($tree[$key]);
			$tmp[$name] = self::getDebugTree($tmp[$name], $tree, $val);
			return $tmp;
		}
	}

	static function mark($name="") {
		self::$marks++;
		if($name=="") $name=$marks;
		self::$time[$name]=microtime(true);
	}
	static function time($mark1="luci_start",$mark2="luci_end") {
		if(!isset($time[$mark2])) $time[$mark2]=microtime(true);
		return (round((self::$time[$mark2]-self::$time[$mark1])*1000,3))." ms";
	}
	static function memory() {
		$unit=array('b','kb','mb','gb','tb','pb');
		$size=memory_get_usage();
    return @round($size/pow(1024,($i=floor(log($size,1024)))),2).' '.$unit[$i];
	}
	static function memory_peak() {
		$unit=array('b','kb','mb','gb','tb','pb');
		$size=memory_get_peak_usage();
    return @round($size/pow(1024,($i=floor(log($size,1024)))),2).' '.$unit[$i];
	}
	static function draw() {
		echo "<div style=\"position:fixed;top:100%;left:100%;margin-left:-300px;margin-top:-25px;padding-left:5px;background:#eeeeee;color:black;font-weight:bold;width:295px;font-size:12px;height:25px;line-height:25px;\">";
		echo "MEM: ",self::memory()," | TIME: ",self::time()," | ";
		echo "<span style=\"display:inline-block;position:relative\"";
		echo " onmouseover=\"document.getElementById('luci_debugger_queries_bar_31').style.display='block'\"";
		echo " onmouseout=\"document.getElementById('luci_debugger_queries_bar_31').style.display='none'\"";
		echo ">Queries:",count(self::getQueries());
		echo "<div style=\"position:absolute;bottom:25px;padding:5px;border:1px black solid;right:0px;font-size:10px;width:300px;background:#fefeee;display:none\" id=\"luci_debugger_queries_bar_31\">";
		$queries = self::getQueries();
		foreach($queries as $query) {
			echo $query."<hr>";
		}
		echo "</div>";
		echo "</span>";
		echo "</div>";
	}
	static function addquery($query) {
		self::$queries[]=$query;
	}
	static function getQueries() {
		return self::$queries;
	}
	static function dump($var="", $compare1 = false, $compare2 = false) {
		echo "<pre style=\"z-index:99999;background-color:#eeeeee;padding:10px;text-align:left;color:black;position:relative;border:1px black solid;\">";
		if($var=="") {
			echo "--------------- POST DATA  ---------------";
			print_r($_POST);
			echo "--------------- SESSION DATA  ---------------";
			print_r($_SESSION);
			echo "--------------- GET DATA  ---------------";
			print_r($_GET);
			echo "--------------- SERVER DATA  ---------------";
			print_r($_SERVER);
		}
		else {
			if ($compare1 !== false || $compare2 !== false) {
				$compares = array($var);
				if ($compare1 !== false) $compares[] = $compare1;
				if ($compare2 !== false) $compares[] = $compare2;

				echo "<table style=\"width:100%\"><tr>";
				foreach ($compares as $compare) {
					echo "<td style=\"width:".(100 / count($compares))."%\">";
					print_r($compare);
					echo "</td>";
				}
				echo "</tr></table>";
			} else {
				print_r($var);
			}
		}
		echo "</pre>";
	}
}
function d($var="") {
	debugger::dump($var);
}
?>