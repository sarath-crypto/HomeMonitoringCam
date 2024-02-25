<?php
	echo '<form action="analytics.php" method="POST" id="action-form"></form>';

	header("Refresh: 10");
	$files = array();
	$dir_sz = 0;
	$dir = "data/";
	$f = scandir($dir);
	foreach ($f as $e){
		$fp = $dir . $e;
		if(is_file($fp)){
			$files[] = $fp;
			$dir_sz += filesize($fp);
		}
	}
	$files_num = sizeof($files);
	$dir_sz = round($dir_sz/1000000,2);
	$df = disk_free_space("/");
	$df = round($df/1000000,2);
	$du = round(($dir_sz/171.79869184),2);

	echo "[Total Entries:".$files_num ."] [Total Directory Size:". $dir_sz . "MB] [Disk Free Space:" . $df . "MB] [Disk Usage:" . $du ."%] <a href='index.php'>Back to main page</a>";

	$d = array();
	foreach ($files as $e){
		$dy = substr($e,9,2);
		if(sizeof($d) == 0){
			$d[] = $dy;
		}else if(in_array($dy,$d) == FALSE){
			$d[] = $dy;
		}
	}
	echo 	'<style>
		table, th, td {
		border: 1px solid black;
		}
		th, td {
  		background-color: #96D4D4;
		}
		</style>';
		
	$nc = 0;
	echo '<form><table>';
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
		echo '<td width="200px"><h3>Records:';
		echo $en;
		echo '</h3><h3>Size:';
		echo $fz;
	        echo 'MB</h3>';
		echo "<input type='submit' value=$e name='data' style='height:100px;width:100px;font-size:60px' form='action-form'></td>";

		$nc += 1;	
		if( $nc == 7){
			echo '</tr>';
			$nc = 0;
		}
	}
	echo '</table>';
	echo '</form>';
?>
