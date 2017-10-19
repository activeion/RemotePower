<?php
if(is_array($_POST)&&count($_POST)>0)//判断是否有Get参数
{ 
	if(isset($_POST["username"]))
	{ 
		$username=$_POST["username"];
	}
	if(isset($_POST["password"]))
	{ 
		$password=$_POST["password"];
	}
	$password = md5($password);
	$sql = "select * from user where username='".$username."' and password='".$password."'";
	$mysqli = new mysqli("localhost", "eh2tech", "eh2tech", "remotepower");
	if ($mysqli->connect_errno) {
		echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
		exit;
	}

	$userr = $mysqli->query($sql);
	if(!$userr){
		echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
		//exit;
	}else{
		if($userr->num_rows==1){
			$row=$userr->fetch_array();
			session_start();
			$_SESSION["userid"]=$row["id"];
			$_SESSION["username"]=$row["username"];
			header("location:index.php");
		}
		$userr->free();
	}

	$mysqli->close();
	exit;
}

?>