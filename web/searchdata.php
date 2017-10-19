<?php
include_once("checklogin.php");
checkajaxlogin();
if(isset($_SESSION['userid'])){
	$userid = $_SESSION['userid'];
}else{
	$userid=0;
}
$out["error"]="";
$id="1";
$typeid="0";
$s =  new DateTime();
$starttime = Date("Y-m-d H:i:s",strtotime("-1 hours"));
$endtime = Date("Y-m-d H:i:s");
//echo $starttime ."---".$endtime;

if(is_array($_POST)&&count($_POST)>0)//判断是否有Get参数
{ 
	if(isset($_POST["id"]))
	{ 
		$id=$_POST["id"];
	}
	if(isset($_POST["typeid"]))
	{ 
		$typeid=$_POST["typeid"];
	}
	if(isset($_POST["starttime"]))
	{ 
		$starttime=$_POST["starttime"];
	}
	if(isset($_POST["endtime"]))
	{ 
		$endtime=$_POST["endtime"];
	}
}
//echo "typeid:".$typeid."<br>starttime:".$starttime."<br>endtime:".$endtime;
switch($typeid){
	case 0:
		$sql = "select uptime,vstack as d1,istack as d2,vout as d3,iout as d4 from power";
		break;
	case 1:
		$sql = "select uptime,rfm_tb as d1,rfm_tc as d2,fc_tin as d3,fc_tout as d4 from temp";
		break;
	case 2:
		$sql = "select uptime,rfm_pumptocat as d1,rfm_back as d2,fc_fanspeed as d3,fc_back as d4 from flow";
		break;
	case 3:
		$sql = "select uptime,rfm_syspress as d1,rfm_h2press as d2,vout as d3,iout as d4 from press";
		break;
	default:
		$sql = "select uptime,vstack as d1,istack as d2,vout as d3,iout as d4 from power";
	break;
}
$sql .=" where deviceid=".$id." and uptime>='".$starttime."' and uptime<'".$endtime."'";
//echo $sql ;exit;
$out["datatype"]=$typeid;
$mysqli = new mysqli("localhost", "eh2tech", "eh2tech", "remotepower");
if ($mysqli->connect_errno) {
	echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}
$result = $mysqli->query($sql);
if(!$result){
	echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
	exit;
}
//echo "<br>Rows::".$result->num_rows."<br>";
if($result->num_rows>0){
	for($i=0;$i<$result->num_rows;$i++){
		$row=$result->fetch_array();
		$out["uptime"][$i]=$row["uptime"];//substr($row["uptime"],11,8);
		$out["d1"][$i]=$row["d1"];
		$out["d2"][$i]=$row["d2"];
		$out["d3"][$i]=$row["d3"];
		$out["d4"][$i]=$row["d4"];
	}
	/*$out["uptime"]=array_reverse($t);
	$out["d1"]=array_reverse($d1);
	$out["d2"]=array_reverse($d2);
	$out["d3"]=array_reverse($d3);
	$out["d4"]=array_reverse($d4);*/
	
}else{

}
$jsonstr = json_encode($out);
echo $jsonstr;

$result->free();
$mysqli->close();

//$endtime = array_sum(explode(' ', microtime()));
//echo "<br><br>ExcuteTime:".($endtime-$starttime)."<br>";
?>