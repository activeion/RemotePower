<?php
include_once("checklogin.php");
session_start();
checklogin();
$userid = $_SESSION["userid"];
?>
<!DOCTYPE HTML>
<html>
 <head>
  <meta charset="UTF-8">
  <title>主页</title>
  <script src="scripts/echarts.min.js" ></script>
  <script src="scripts/jquery-1.12.4.min.js"></script>
  <style type="text/css">
  body{margin:0;padding:0;background:#d0d0d0;}
.header{left:0;right:0;height:50px;padding-left:30px;border-bottom:3px solid gray;}
.main{position:relative;width:100%;min-height:860px;_height:860px}

.left{position:absolute;top:0;left:0;bottom:0;width:180px;background-color:#c0c0c0;border-right:3px solid black;}
.lefthd{position:absolute;top:0;left:0;right:0;_width:100%;height:25px;padding:3px;background-color:#c0c0c0;border-bottom:2px solid gray}
.menu_wrap{position:absolute;top:33px;width:100%;padding:0}
.menuul {list-style-type: none;margin:0;padding:0;width:100%;text-align:left bottom}
.menuul li.cur,li:hover{background-color:#d0d0d0;cursor:default}
.menuul li{height:25px;font-size:16px;padding-left:10px;border-bottom:1px solid #e0e0e0;cursor:pointer;overflow:hidden;white-space:nowrap;text-overflow:ellipsis;-o-text-overflow: ellipsis;}

#content{position:absolute;top:0;left:181px;right:0;height:100%;min-width:1200px;}
.lable{margin-left:20px;padding-left:5px;border-left:1px solid white;display:inline-block;}
.pic{width: 48%;height:400px;float:left;border:1px solid black;background-color:white;margin:6px}
.foot{left:0;right:0;;height:30px;border-top:2px solid black;padding:2px}

.black_overlay{display:none; position: absolute;  top: 0%;  left: 0%;  width: 100%;  height: 100%;  background-color: black;  z-index:1001;  -moz-opacity: 0.8;  opacity:.50;  filter: alpha(opacity=50);  }  
.white_content {  display: none;  position: absolute;  top: 5%;  left: 20%; bottom:5%; right:20%; padding:6px; border: 1px solid gray;  background-color: white;  z-index:1002;  overflow: auto; }
.closeicon{float: right;font-size: 14px;cursor: pointer;text-align: center;width: 35px;}
  </style>
 </head>
 <body>
 <div class="header"><h1>Remote Power Monitor</h1></div>
 <!-- end of header-->
 <div class="main">
	<!--start of left-->
 <div class="left">
	<div class ="lefthd"><?php echo "用户:".$_SESSION["username"];?></div>
	<div class="menu_wrap">
	 <ul id="devicelist" class="menuul">
<?php	
	$mysqli = new mysqli("localhost", "eh2tech", "eh2tech", "remotepower");
	if ($mysqli->connect_errno) {
		echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
	}
	$sql="select * from devices where user=".$userid;
	$result = $mysqli->query($sql);
	if(!$result){
		echo "ERROR:(".$mysqli->errno.")".$mysqli->error."<br/>";
		//exit;
	}
	$d_name="";
	$d_type="";
	$deviceid=0;
	$sn="";
	if($result->num_rows>0){
		for($i=0;$i<$result->num_rows;$i++){
			$row=$result->fetch_array();
			if($i==0){
				//<a href='javascript:getAllData(".$row["id"].")'></a>
				echo "<li class='cur' value='".$row["id"]."'>".$row["d_name"]."</li>";
				$deviceid=$row["id"];
				$sn=$row["sn"];
				$d_name=$row["d_name"];
				$d_type=$row["type"];
			}else{
				echo "<li value='".$row["id"]."'>".$row["d_name"]."</li>";
			}
		}
	}else{
		echo "<li>没有设备</li>";
	}
	$result->free();
	$mysqli->close();
?>
	 </ul>
	</div>
 </div>
 <!-- end of left-->
 <div id="content">
	<div id="titleinfo" style="margin-left:4px;width:95%;height:25px;background-color:#c0c0c0;padding:3px;text-align:left center">
	<?php
	echo "<span class='lable'>设备:".$d_name."</span><span class='lable'>型号:".$d_type."</span><span class='lable'>序列号:".$sn."</span>";
	?>
	<span class="lable"  style='float:right'><input type="button" value="查询历史数据" onclick="showsearch(<?php echo $deviceid;?>)"></span>
	</div>

    <div id="fcinfo" class="pic"></div>
	<div id="tempinfo" class="pic"></div>
	<div id="pressinfo" class="pic"></div>
	<div id="flowinfo" class="pic"></div>
	
 </div>
 </div>
 <!--end of main-->
 <div class="foot">请使用IE8或更高版本、Chrome、Firefox、Edge、Safari等浏览器。</div>
    <script type="text/javascript">
		var deviceid= <?php echo $deviceid ?>;
		var counter=0;
		var timer;
        // 基于准备好的dom，初始化echarts实例
		var fcChart = echarts.init(document.getElementById('fcinfo'));
		var tempChart = echarts.init(document.getElementById('tempinfo'));
		var pressChart = echarts.init(document.getElementById('pressinfo'));
		var flowChart = echarts.init(document.getElementById('flowinfo'));
		// 指定图表的配置项和数据
        var option = {
			title: {
			text: '发电参数'
			},
			tooltip: {trigger: 'axis'},
			legend: {x:'center',data:['堆电压','堆电流','输出电压','输出电流']},
			//toolbox: {feature: {saveAsImage: {}} },
			xAxis: {title:'时间',data: []},
			yAxis: [
				{name:'电压/V', type: 'value',axisLabel: {formatter: '{value}'}},
				{name:'电流/A',	type: 'value',axisLabel: {formatter: '{value}'}}],
			series: [
				{name: '堆电压',type: 'line',data: []},
				{name: '堆电流',type: 'line',yAxisIndex:1,data: []},
				{name: '输出电压',type: 'line',data: []	},
				{name: '输出电流',type: 'line',yAxisIndex:1,data: []}]
		};
        // 使用刚指定的配置项和数据显示图表。
        fcChart.setOption(option);
		
		var opttemp = {
			title: {
			text: '温度'
			},
			tooltip: {trigger: 'axis'},
			legend: {x:'center',data:['Tb','Tc','Tin','Tout']},
			//toolbox: {feature: {saveAsImage: {}}},
			xAxis: {title:'时间',data: []},
			yAxis: [{name:'Rfm/℃',	type: 'value',axisLabel: {formatter: '{value}'}},
					{name:'Fc/℃',	type: 'value',axisLabel: {formatter: '{value}'}}],
			series: [{name: 'Tb',type: 'line',data: []},
					 {name: 'Tc',type: 'line',data: []},
					 {name: 'Tin',type: 'line',	yAxisIndex:1,data: []},
					 {name: 'Tout',type: 'line',yAxisIndex:1, data: []}]
		};
		tempChart.setOption(opttemp);
		var optpress = {
			title: {
			text: '压力'
			},
			tooltip: {trigger: 'axis'},
			legend: {x:'center',data:['Psys','Ph2']},
			toolbox: {feature: {saveAsImage: {}}},
			xAxis: {title:'时间',data: []},
			yAxis: [{name:'Psys/Kpa',	type: 'value',axisLabel: {formatter: '{value}'}},
					{name:'Ph2/Kpa',	type: 'value',axisLabel: {formatter: '{value}'}}],
			series: [{name: 'Psys',type: 'line',data: []},
					 {name: 'Ph2',type: 'line',yAxisIndex:1,data: []}]
		};
		pressChart.setOption(optpress);
		var optflow = {
			title: {
			text: '泵次与风扇转速'
			},
			tooltip: {trigger: 'axis'},
			legend: {x:'center',data:['泵次','转速']},
			toolbox: {feature: {saveAsImage: {}}},
			xAxis: {title:'时间',data: []},
			yAxis: [{name:'次/10分',	type: 'value',axisLabel: {formatter: '{value}'}},
					{name:'RPM',	type: 'value',axisLabel: {formatter: '{value}'}}],
			series: [{name: '泵次',type: 'line',data: []},
					 {name: '转速',type: 'line',yAxisIndex:1,data: []}]
		};
		flowChart.setOption(optflow);
		//
		function getData(id,tid){
			$.get("data.php?id="+id+"&typeid="+tid,function(data,status){
				var obj = $.parseJSON(data);
				if(obj.error==""){
				switch(obj.datatype){
					case '0':
					var option={
						xAxis: {
						data: obj.uptime
						},
						series: [{data: obj.d1},{data: obj.d2},{data: obj.d3},{data: obj.d4}]
					};
					fcChart.setOption(option);
					break;
					case '1':
					var option={
						xAxis: {
						data: obj.uptime
						},
						series: [{data: obj.d1},{data: obj.d2},{data: obj.d3},{data: obj.d4}]
					};
					tempChart.setOption(option);
					break;
					case '2':
					var option={
						xAxis: {
						data: obj.uptime
						},
						yAxis: {},
						series: [{data: obj.d1},{data: obj.d2}]
					};
					flowChart.setOption(option);
					break;
					case '3':
					var option={
						xAxis: {
						data: obj.uptime
						},
						yAxis: {},
						series: [{data: obj.d1},{data: obj.d2}]
					};
					pressChart.setOption(option);
					break;
					default:
					break;
				}
			}else{
				top.document.location="login.html";
			}
			});// end of $.get
		}
		var devicelist=null;
		function getList(page){
			$.get("getlist.php?page="+page,function(data,status){
				var obj = $.parseJSON(data);
				if(obj.error==""&&obj.len>0){
					devicelist=obj;
					var ulobj = $("#devicelist");
					ulobj.empty()
					for(i=0;i<obj.len;i++){
						//alert(obj.data[i].sn);
						var li="<li><a class='mitem' href='#'>"+obj.data[i].name+"</a></li>";
						ulobj.append(li);
					}
				}else{
					top.document.location="login.html";
				}
			}
		)}
		//getList(1);
function getAllData(devid){
	$.get("getalldata.php?id="+devid,function(data,status){	
		//status:"success"、"notmodified"、"error"、"timeout"、"parsererror"
		if(status=="success"){
			var obj = $.parseJSON(data);
			if(obj.error!=""){
				top.document.location="login.html";
				return;
			}
			var span=$("#titleinfo");
			span.empty();
			span.html("<span class='lable'>设备:"+obj.detail.d_name+"</span><span class='lable'>型号:"+obj.detail.type+"</span><span class='lable'>序列号:"+obj.detail.sn+"</span>"+"<span class='lable' style='float:right'><input type='button'  value='查询历史数据' onclick='showsearch(deviceid)' /></span>");

			deviceid=obj.devid;
			var optpower={
				xAxis: {
				data: obj.power.uptime
				},
				series: [{data: obj.power.d1},{data: obj.power.d2},{data: obj.power.d3},{data: obj.power.d4}]
			};
			fcChart.setOption(optpower);

			var opttemp={
				xAxis: {
				data: obj.temp.uptime
				},
				series: [{data: obj.temp.d1},{data: obj.temp.d2},{data: obj.temp.d3},{data: obj.temp.d4}]
			};
			tempChart.setOption(opttemp);

			var optflow={
				xAxis: {
				data: obj.flow.uptime
				},
				yAxis: {},
				series: [{data: obj.flow.d1},{data: obj.flow.d2}]
			};
			flowChart.setOption(optflow);

			var optpress={
				xAxis: {
				data: obj.press.uptime
				},
				yAxis: {},
				series: [{data: obj.press.d1},{data: obj.press.d2}]
			};
			pressChart.setOption(optpress);
		}
		counter=0;
	})
}

getAllData(1);

function timer_tick(){
	counter++;
	if(counter%3==0){getData(deviceid,0)}
	if(counter%10==0){getData(deviceid,1)}
	if(counter%10==0){getData(deviceid,2)}
	if(counter%10==0){getData(deviceid,3)}
}
timer = setInterval("timer_tick()",1000);
//$("li").on('click',null,function(){alert($(this).val())});

function liclick(){
	li = $(this);
	preli = $("#devicelist li.cur");
	preli.removeClass('cur');
	preli.click(liclick);
	li.addClass('cur');
	li.unbind();
	getAllData(li.val());
}
$("#devicelist li").click(liclick);
$("#devicelist li.cur").unbind();

function showsearch(devid){
	$("#light").show();$("#fade").show();
}

</script>

<div id="light" class="white_content"> 
<div style="height: 20px;border-bottom: 1px solid gray"><span style="font-size: 16px"><b>历史数据查询</b></span><span class="closeicon" onclick="$('#light').hide();$('#fade').hide();"> 关闭</span></div>
<div style="height: 579px"><iframe src="searchui.php" style="border: none;height: 550px;width: 100%"></iframe></div>
</div> 
<div id="fade" class="black_overlay"> </div> 
 </body>
</html>

