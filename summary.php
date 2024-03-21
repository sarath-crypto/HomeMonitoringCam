<?php
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



function drawBarGraph($cachefilename, $ydata, $xdata){
        require_once ('jpgraph/jpgraph.php');
        require_once ('jpgraph/jpgraph_utils.inc.php');
        require_once ('jpgraph/jpgraph_bar.php');

        $graph = new Graph(1228,200);
        $valid = $graph -> cache -> IsValid($cachefilename);
        if ($valid){
                return;
        }else{
                $graph -> SetupCache($cachefilename, 1);
                $graph->clearTheme();
                $graph -> SetScale("textlin");
                $graph->SetShadow();
                $bplot = new BarPlot($ydata);
                $bplot->SetFillColor('blue');
                $bplot->SetWidth(1.0);
                $graph->Add($bplot);
                $graph->title->Set("Disk Usage(MB) Vs Dates Graph");
                $graph->xaxis->SetTickLabels($xdata);
                $graph->xaxis->title->Set("Date");
                $graph->yaxis->title->Set("Usage");
                $graph->title->SetFont(FF_FONT1,FS_BOLD);
                $graph->yaxis->title->SetFont(FF_FONT1,FS_BOLD);
                $graph->xaxis->title->SetFont(FF_FONT1,FS_BOLD);
                $absolutePath = (CACHE_DIR . "" . $cachefilename);
                $graph -> Stroke($absolutePath);
        }
}

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

	$ydata = array(100-$du,$du);
	$f_pie = 'graph/pie.png';
	$graph = drawPieGraph($f_pie, $ydata);


	unset($ydata);
	foreach ($dirs as $e){
    		$io = popen ( '/usr/bin/du -sk ' . $e, 'r' );
    		$sz = fgets ( $io, 4096);
    		$sz = substr ( $sz, 0, strpos ( $sz, "\t" ) );
		pclose ($io);
		$ydata[] = $sz/1024;
	}

	$f_bar = 'graph/bar_summary.png';
        $graph = drawBarGraph($f_bar,$ydata,$icons);

	$io = popen ( '/usr/bin/uptime ', 'r' );
    	$ut = fgets ( $io, 4096);
	pclose ($io);
	echo "&emsp;[UpTime:" . $ut . "]";
	
	echo '<table><tr  style="height:200px">';
	
	echo '<td width="200px">';
	echo '<img style="vertical-align: bottom;"  width="100%" height="100%" src=';
	echo $f_pie;
	echo '></img></td>';

	echo '<td width="1228 px">';
	echo '<img style="vertical-align: bottom;"  src=';
	echo $f_bar;
	echo '></img></td>';


	echo '</tr></table>';


	echo '<form action="analytics.php" method="POST" id="action-form"></form>';

	header("Refresh: 60");
	
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
