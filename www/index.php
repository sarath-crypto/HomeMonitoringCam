<?php

	header("Refresh: 1");
	$conn = new mysqli('localhost','usersecam','secam123','secam');
	if ($conn->connect_error) {
  		die("Connection failed: " . $conn->connect_error);
	}else{
		$sql = "SELECT ts,data FROM img";	
		$result = $conn->query($sql);
		if ($result->num_rows > 0) {
  			while($row = $result->fetch_assoc()) {
				echo '<div class="caption"><h3><img src="data:image/jpeg;base64,'.base64_encode($row['data']).'"/></br>'. $row['ts']. '</h3></div>';
  			}						
		}
		$conn->close();
	}
	echo '<a href="summary.php">Show Summary</a>';
?>
