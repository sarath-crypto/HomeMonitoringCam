<?php
function drawBarGraph($cachefilename, $ydata){
	require_once ('jpgraph/jpgraph.php');
	require_once ('jpgraph/jpgraph_utils.inc.php');
	require_once ('jpgraph/jpgraph_bar.php');
	
	$graph = new Graph(1600,200);
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
		$graph->title->Set("Events Graph");
		$graph->xaxis->SetTickLabels(array(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23));
		$graph->xaxis->title->Set("Time");
		$graph->yaxis->title->Set("Events");
		$graph->title->SetFont(FF_FONT1,FS_BOLD);
		$graph->yaxis->title->SetFont(FF_FONT1,FS_BOLD);
		$graph->xaxis->title->SetFont(FF_FONT1,FS_BOLD);
		$absolutePath = (CACHE_DIR . "" . $cachefilename);
		$graph -> Stroke($absolutePath);
	}
}
	if ($_SERVER["REQUEST_METHOD"] == "POST") {
		$selhr = htmlspecialchars($_POST['hour']);
		$date = htmlspecialchars($_POST['date']);
		if (strlen($selhr) > 0){
			$dir = "data/";
			$f = scandir($dir);
			foreach ($f as $e){
				$fp = $dir . $e;
				if(is_file($fp)){
					$hr = substr($e,6,2);
					$dy = substr($e,4,2);
					if(($hr == $selhr) && ($dy == $date))$files[] = $fp;
				}
			}
			$files_sz = sizeof($files);
			echo "[Entries:".$files_sz ."] [Hour:".$selhr."] [Date:".$date."] <a href='summary.php'>Back to summary page</a></br>";
		 	echo    '<style>
                		table, th, td {
                		border: 1px solid black;
                		}
                		th, td {
                		background-color: #96D4D4;
                		}</style>';
			
			echo '<table>';
			foreach ($files as $e){
                		if( $nc == 0){
                        		echo '<tr style="height:200px;text-align: center; vertical-align: middle;">';
                		}
				echo '<td width="200px">';
				echo '<img src=';
				echo $e;
			        echo ' alt="" border=1 height=180 width=180>';
				echo '</td>';
                		$nc += 1;
                		if( $nc == 7){
                        		echo '</tr>';
                        		$nc = 0;
				}
			}
			echo '</table>';
		}
	}
?>
