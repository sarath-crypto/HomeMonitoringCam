<?php
	header("Refresh: 5");
	$files = array();
	$dir_sz = 0;
	$dir = "data/";
	$f = scandir($dir);
	foreach ($f as $e){
		$fp = $dir . $e;
		if(is_file($fp)){
			$files[] = $fp;
			$dir_sz += filesize($fp);
			//echo $fp ." ". filesize($fp) ." ". $dir_sz .  "<br>";
		}
	}
	$files_sz = sizeof($files);
	$df = disk_free_space("/");
	$df = round($df/1000000,2);
	echo "[Entries:".$files_sz ."] [Directory Size:". round($dir_sz/1000000,2) . "MB] [Disk Free Space:" . $df . "MB]</br>";

	$d = array();
	foreach ($files as $e){
		$dy = substr($e,9,2);
		if(sizeof($d) == 0){
			$d[] = $dy;
		}else if(in_array($dy,$d) == FALSE){
			$d[] = $dy;
		}
	}
	//echo sizeof($d) . "-" . $d[0];
	echo 	'<style>
		table, th, td {
		border: 1px solid black;
		}
		th, td {
  		background-color: #96D4D4;
		}
		</style>';
		
	$nc = 0;	
	echo '<table style="width:400px">';
	foreach ($d as $e){
		$en = 0;
		$fz = 0;
		if( $nc == 0){
			echo '<tr style="height:200px;text-align: center; vertical-align: middle;">';
		}
		foreach ($files as $p){
			$dy = substr($p,9,2);
			if($e == $dy){
				$en += 1;
				$fz += filesize($p);
			}
		}
		$fz = round($fz/1000000,1);
		echo "<td><h1>$e</h1><h3>Records:$en</h3><h3>Size:$fz MB</h3></td>";
		$nc += 1;	
		if( $nc == 4){
			echo '</tr>';
			$nc = 0;
		}
	}
	echo '</table>';

	//echo '<img src="' . $e. '" alt="My Image">';

?>
