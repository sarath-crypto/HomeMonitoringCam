<?php

function drawPieGraph($cachefilename, $ydata){
        require_once ('jpgraph/jpgraph.php');
	require_once ('jpgraph/jpgraph_pie.php');

	$graph = new PieGraph(200,200);
        $valid = $graph -> cache -> IsValid($cachefilename);
	if ($valid){
                return;
        }else{
		$graph -> SetupCache($cachefilename, 1);
                $graph->clearTheme();
		$graph->SetShadow();
		$graph->title->Set("Disk Usage");
		$pie = new PiePlot($ydata);
		$colors = array('#FF0000', '#00FF00');
		$pie->SetSliceColors($colors);
                $graph->Add($pie);
                $absolutePath = (CACHE_DIR . "" . $cachefilename);
                $graph -> Stroke($absolutePath);
        }
}
	$df = disk_free_space("/");
	$ds = disk_total_space("/");
	$du = ($df/$ds)*100;
	
	echo "<a href='index.php'>Back to main page</a>";

	$ydata = array($du,100-$du);
	$filename = 'graph/pie.png';
        $graph = drawPieGraph($filename, $ydata);
	echo '<table><tr  style="height:200px"><td width="200px">';
	echo '<img style="vertical-align: bottom;"  width="100%" height="100%" src=';
	echo $filename;
	echo "></img>";
	echo '</td></tr></table>';


	echo '<form action="analytics.php" method="POST" id="action-form"></form>';

	header("Refresh: 60");
	$dirs = array();
	$dir = "data/";
	$f = scandir($dir);
	foreach ($f as $e){
		$fp = $dir . $e;
		if(is_dir($fp))$dirs[] = $fp;
	}
	$dirs = array_slice($dirs,2);
	$icons = array();		
	foreach ($dirs as $e){
		$icons[] = substr($e,5);
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
	foreach ($icons as $e){
		$en = 0;
		$fz = 0;
		if( $nc == 0){
			echo '<tr style="height:200px;text-align: center; vertical-align: middle;">';
		}
		echo '<td width="200px">';
		echo "<input type='submit' value=$e name='data' style='height:100px;width:100px;font-size:60px' form='action-form'></td>";

		$nc += 1;	
		if( $nc == 7){
			echo '</tr>';
			$nc = 0;
		}
	}
	echo '</table></form>';
?>
