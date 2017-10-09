<?php
error_reporting(E_ALL);
ini_set('display_errors', '1'); 
header("Content-type: text/html; charset=utf-8");
$page=1;
$out["error"]="";
if(is_array($_GET)&&count($_GET)>0)//判断是否有Get参数 
{ 
	if(isset($_GET["page"]))
	{ 
		$page=$_GET["page"];
	}
}
$userid="1";
$start=15*($page-1);
$sql = "select * from devices where user=".$userid." LIMIT ".$start.",15";
//echo $sql;

$mysqli = new mysqli("localhost", "eh2tech", "eh2tech", "remotepower");
if ($mysqli->connect_errno) {
	echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}
$result = $mysqli->query($sql);
if(!$result){
	echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
	exit;
}
$out["page"]=$page;
$out["len"]=$result->num_rows;
if($result->num_rows>0){
	for($i=0;$i<$result->num_rows;$i++){
		$row=$result->fetch_array();
		$out["data"][$i]["id"]=$row["id"];
		$out["data"][$i]["sn"]=$row["sn"];
		$out["data"][$i]["name"]=$row["d_name"];
		$out["data"][$i]["type"]=$row["type"];
		$out["data"][$i]["description"]=$row["description"];
	}
	$jsonstr = json_encode($out);
	echo $jsonstr;
}
$result->free();
$mysqli->close();
?>