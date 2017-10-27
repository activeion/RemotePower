<?php
include_once("checklogin.php");
checkajaxlogin();

$id="0";

if(is_array($_GET)&&count($_GET)>0)//判断是否有Get参数 
{ 
	if(isset($_GET["id"]))
	{ 
		$id=$_GET["id"];
	}
}
if($id=="0"){

}


$out["error"]="";
$out["devid"]=$id;

$_SESSION['deviceid']=$id;

$mysqli = new mysqli("localhost", "eh2tech", "eh2tech", "remotepower");
if ($mysqli->connect_errno) {
	echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}

$sql = "select * from devices where id=".$id." LIMIT 0,15";
$result = $mysqli->query($sql);
if(!$result){
	echo $sql."<br>";
	echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
	exit;
}
if($result->num_rows>0){
	$row=$result->fetch_array();
	$out["detail"]["sn"]=$row["sn"];
	$out["detail"]["d_name"]=$row["d_name"];
	$out["detail"]["type"]=$row["type"];
}
$result->free();

$sql = "select uptime,vstack as d1,istack as d2,vout as d3,iout as d4 from power where deviceid=".$id." order by uptime desc LIMIT 100";
$result = $mysqli->query($sql);
if(!$result){
	echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
	exit;
}
if($result->num_rows>0){
	for($i=0;$i<$result->num_rows;$i++){
		$row=$result->fetch_array();
		$t[$i]=substr($row["uptime"],11,8);
		$d1[$i]=$row["d1"];
		$d2[$i]=$row["d2"];
		$d3[$i]=$row["d3"];
		$d4[$i]=$row["d4"];
	}
	$out["power"]["uptime"]=array_reverse($t);
	$out["power"]["d1"]=array_reverse($d1);
	$out["power"]["d2"]=array_reverse($d2);
	$out["power"]["d3"]=array_reverse($d3);
	$out["power"]["d4"]=array_reverse($d4);
	unset($t);unset($d1);unset($d2);unset($d3);unset($d4);
}
$result->free();

$sql = "select uptime,rfm_tb as d1,rfm_tc as d2,fc_tin as d3,fc_tout as d4 from temp where deviceid=".$id." order by uptime desc  LIMIT 100";
$result = $mysqli->query($sql);
if(!$result){
	echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
	exit;
}
if($result->num_rows>0){
	for($i=0;$i<$result->num_rows;$i++){
		$row=$result->fetch_array();
		$t[$i]=substr($row["uptime"],11,8);
		$d1[$i]=$row["d1"];
		$d2[$i]=$row["d2"];
		$d3[$i]=$row["d3"];
		$d4[$i]=$row["d4"];
	}
	$out["temp"]["uptime"]=array_reverse($t);
	$out["temp"]["d1"]=array_reverse($d1);
	$out["temp"]["d2"]=array_reverse($d2);
	$out["temp"]["d3"]=array_reverse($d3);
	$out["temp"]["d4"]=array_reverse($d4);
	unset($t);unset($d1);unset($d2);unset($d3);unset($d4);
}
$result->free();

$sql = "select uptime,rfm_pumptocat as d1,rfm_back as d2,fc_fanspeed as d3,fc_back as d4 from flow where deviceid=".$id." order by uptime desc LIMIT 100";
$result = $mysqli->query($sql);
if(!$result){
	echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
	exit;
}
if($result->num_rows>0){
	for($i=0;$i<$result->num_rows;$i++){
		$row=$result->fetch_array();
		$t[$i]=substr($row["uptime"],11,8);
		$d1[$i]=$row["d1"];
		$d2[$i]=$row["d2"];
		$d3[$i]=$row["d3"];
		$d4[$i]=$row["d4"];
	}
	$out["flow"]["uptime"]=array_reverse($t);
	$out["flow"]["d1"]=array_reverse($d1);
	$out["flow"]["d2"]=array_reverse($d2);
	$out["flow"]["d3"]=array_reverse($d3);
	$out["flow"]["d4"]=array_reverse($d4);
	unset($t);unset($d1);unset($d2);unset($d3);unset($d4);
}
$result->free();

$sql = "select uptime,rfm_syspress as d1,rfm_h2press as d2,vout as d3,iout as d4 from press where deviceid=".$id." order by uptime desc LIMIT 100";
$result = $mysqli->query($sql);
if(!$result){
	echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
	exit;
}
if($result->num_rows>0){
	for($i=0;$i<$result->num_rows;$i++){
		$row=$result->fetch_array();
		$t[$i]=substr($row["uptime"],11,8);
		$d1[$i]=$row["d1"];
		$d2[$i]=$row["d2"];
		$d3[$i]=$row["d3"];
		$d4[$i]=$row["d4"];
	}
	$out["press"]["uptime"]=array_reverse($t);
	$out["press"]["d1"]=array_reverse($d1);
	$out["press"]["d2"]=array_reverse($d2);
	$out["press"]["d3"]=array_reverse($d3);
	$out["press"]["d4"]=array_reverse($d4);
	unset($t);unset($d1);unset($d2);unset($d3);unset($d4);
}
	$jsonstr = json_encode($out);
	echo $jsonstr;
$result->free();
$mysqli->close();
?>