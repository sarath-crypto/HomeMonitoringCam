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
		$data = htmlspecialchars($_POST['data']);
		if (empty($data) == FALSE){
			$ydata = array(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
			$files = array();
			$dir_sz = 0;
			$dir = "data/".$data."/";
			$f = scandir($dir);
			foreach ($f as $e){
				$fp = $dir . $e;
				if(is_file($fp)){
					$files[] = $fp;
					$dir_sz += filesize($fp);
					$hr = substr($e,6,2);
					$ydata[(int)$hr] += 1;
				}
			}
			$files_sz = sizeof($files);
			echo "[Entries:".$files_sz ."] [Directory Size:". round($dir_sz/1000000,2) . "MB] [Date:". $data . "] <a href='summary.php'>Back to summary page</a></br>";

			$filename = 'graph/bar.png';
			$graph = drawBarGraph($filename, $ydata);

			echo '<table border="4"><tr  style="height:200px"><td width="1600px">';
			echo '<img style="vertical-align: bottom;"  width="100%" height="100%" src=';
			echo $filename; 
			echo "></img>";
			echo '</td></tr></table>';
		}
	}

echo 	'<br><form method="post" action="showdata.php">  
	<label>Select Hour:</label>
 	<select name="hour">
    	<option value="0">0</option>
    	<option value="1">1</option>
    	<option value="2">2</option>
	<option value="3">3</option>
	<option value="4">4</option>
    	<option value="5">5</option>
    	<option value="6">6</option>
    	<option value="7">7</option>
    	<option value="8">8</option>
    	<option value="9">9</option>
    	<option value="10">10</option>
    	<option value="11">11</option>
    	<option value="12">12</option>
    	<option value="13">13</option>
    	<option value="14">14</option>
    	<option value="15">15</option>
	<option value="16">16</option>
	<option value="17">17</option>
    	<option value="18">18</option>
    	<option value="19">19</option>
    	<option value="20">20</option>
    	<option value="21">21</option>
    	<option value="22">22</option>
    	<option value="23">23</option>
  	</select>
	<br><br>
	<input type="hidden" name="date" value="';
	echo $data;
	echo '">
	<input type="submit" />
	</form>';
?>
