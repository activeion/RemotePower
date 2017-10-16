<?php
function checklogin(){
	if(!isset($_SESSION['userid'])){
		header("location:login.html");
	}
}

function checkajaxlogin(){
	session_start();
	if(!isset($_SESSION['userid'])){
		$out["error"]="notlogin";
		$jsonstr = json_encode($out);
		echo $jsonstr;
		exit;
	}
}
?>