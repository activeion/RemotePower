<?php
error_reporting(E_ALL);
ini_set('display_errors', '1'); 

$id="1";
$typeid="0";

if(is_array($_GET)&&count($_GET)>0)//判断是否有Get参数 
{ 
	if(isset($_GET["id"]))
	{ 
		$id=$_GET["id"];
	}
	if(isset($_GET["typeid"]))
	{ 
		$typeid=$_GET["typeid"];
	}
}
switch($typeid){
	case 0:
		$sql = "select uptime,vstack as d1,istack as d2,vout as d3,iout as d4 from power where deviceid=".$id." order by uptime desc LIMIT 100";
		break;
	case 1:
		$sql = "select uptime,rfm_tb as d1,rfm_tc as d2,fc_tin as d3,fc_tout as d4 from temp where deviceid=".$id." order by uptime desc  LIMIT 100";
		break;
	case 2:
		$sql = "select uptime,rfm_pumptocat as d1,rfm_back as d2,fc_fanspeed as d3,fc_back as d4 from flow where deviceid=".$id." order by uptime desc LIMIT 100";
		break;
	case 3:
		$sql = "select uptime,rfm_syspress as d1,rfm_h2press as d2,vout as d3,iout as d4 from press where deviceid=".$id." order by uptime desc LIMIT 100";
		break;
	default:
		$sql = "select uptime,vstack as d1,istack as d2,vout as d3,iout as d4 from power where deviceid=".$id."  order by uptime desc LIMIT 100";
	break;
}
$out=[
	"datatype"=>$typeid,
];
$mysqli = new mysqli("localhost", "eh2tech", "eh2tech", "remotepower");
if ($mysqli->connect_errno) {
	echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}
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
	$out["uptime"]=array_reverse($t);
	$out["d1"]=array_reverse($d1);
	$out["d2"]=array_reverse($d2);
	$out["d3"]=array_reverse($d3);
	$out["d4"]=array_reverse($d4);
	$jsonstr = json_encode($out);
	echo $jsonstr;
}
$result->free();
$mysqli->close();
?>