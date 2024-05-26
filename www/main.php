<?php
	session_start();
	
	if ($_SERVER["REQUEST_METHOD"] == "POST") {
                $pass = htmlspecialchars($_POST['pass_word']);
                if (empty($pass) == FALSE){
			if($pass == "password"){
				$_SESSION['auth'] = true;
			}
		}
	}
	if($_SESSION['auth']){
		header("Refresh: 1");
		$conn = new mysqli('localhost','userecsys','ecsys123','ecsys');
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
	}else{
		echo '<meta http-equiv = "refresh" content = "0; url = index.php" />';

	}
?>
