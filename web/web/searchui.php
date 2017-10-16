<?php
include_once("checklogin.php");
checkajaxlogin();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
 <head>
  <meta charset="UTF-8">
  <title>数据查询</title>
  <script src="scripts/echarts.min.js" ></script>
  <script src="scripts/jquery-1.12.4.min.js"></script>
  <script type="text/javascript" src="scripts/jedate.js"></script>
  <link type="text/css" rel="stylesheet" href="css/jedate.css">
  <style type="text/css">
  body{
  	margin: 0;padding: 0;
  	font-size: 12px;
	font-family: "微软雅黑", "宋体", "Arial Narrow";
}
  </style>
<style type="text/css">
.pic{width: 95%;height:400px;margin:2px auto;border:1px solid gray;background-color:white;}
.header{height: 25px;width: 100%;text-align: center;}
.foot{height: 25px}
</style>
 </head>
 <body>
<div style="margin:5px auto;height: 460px; width:700px;">
    <div class="header">
	<form method="post">
	<input name="deviceid" type="hidden" value="<?php echo 1; ?>">
	<div id="typesel" style="display: inline-block;">
		<input type="radio" name="typeid" value="0" checked="">DCDC
		<input type="radio" name="typeid" value="1">温度
		<input type="radio" name="typeid" value="2">流量
		<input type="radio" name="typeid" value="3">压力
	</div>
	<span style="margin-left:20px">时间段:</span>
  	<input id="txtBeginDate" value="<?php echo Date("Y-m-d H:i:s",strtotime("-1 hours")); ?>" style="width:120px;padding:2px 5px;border:1px solid #ccc;"/><span>--</span>
  	<input id="txtEndDate" value = "<?php echo Date("Y-m-d H:i:s"); ?>" style="width:120px;padding:2px 5px;border:1px solid #ccc;margin-right:20px;" />
  	<input id="search" type="button" value="查询" />
  	</form>
  	</div>
    <div id="charts" class="pic"></div>
    <div id="info" class="foot" ></div>
</div>

<script type="text/javascript" src="scripts/chartsopt.js"></script>
<script type="text/javascript">
var Charts = echarts.init(document.getElementById('charts'));
var datatype = "0";
        // 使用刚指定的配置项和数据显示图表。
Charts.setOption(optfc);


$("#typesel input").click(function(){
	var t =$(this).val(); 
	switch(t){
		case "0": if(datatype!="0") Charts.setOption(optfc);	break;
		case "1": if(datatype!="1") Charts.setOption(opttemp);	break;
		case "2": if(datatype!="2") Charts.setOption(optflow);	break;
		case "3": if(datatype!="3") Charts.setOption(optpress);	break;
		default:break;
	}
	datatype=t;
});
$("#search").click(function(){
	//alert($("#txtBeginDate").val());
	$.post("searchdata.php",
		{typeid:datatype,starttime:$("#txtBeginDate").val(),endtime:$("#txtEndDate").val()},
		function(data,status){
			if(status=="success"){
				//$("#info").html(data);
				var obj = $.parseJSON(data);
				if(obj.error==""){
					switch(obj.datatype){
						case "0":
						case "1":{
						var option={
							xAxis: { data: obj.uptime},
							series: [{data: obj.d1},{data: obj.d2},{data: obj.d3},{data: obj.d4}]
						};
						Charts.setOption(option);
						break;}
						case "2":
						case "3":{
						var option={
							xAxis: {data: obj.uptime },
							series: [{data: obj.d1},{data: obj.d2},{data:[]},{data:[]}]
						};
						Charts.setOption(option);
						break;}
					}
				}else{
					//$(top.document).location="login.html";
					$("#info").html(status);
					top.document.location="login.html";
				}

			}else{$("#info").html(status);}
	})
});

var start = {
    dateCell: '#txtBeginDate',
    format: 'YYYY-MM-DD hh:mm:ss',
    minDate: '2014-06-16 23:59:59', //设定最小日期为当前日期
	festival:true,
    maxDate: '2099-06-16 23:59:59', //最大日期
   isTime: true,
    choosefun: function(datas){
         end.minDate = datas; //开始日选好后，重置结束日的最小日期
    }
};
var end = {
    dateCell: '#txtEndDate',
    format: 'YYYY-MM-DD hh:mm:ss',
    minDate: jeDate.now(0), //设定最小日期为当前日期
	festival:true,
    maxDate: '2099-06-16 23:59:59', //最大日期
    isTime: true,
    choosefun: function(datas){
         start.maxDate = datas; //将结束日的初始值设定为开始日的最大日期
    },
	//okfun:function(val){alert(val)}
};
jeDate(start);
jeDate(end);
</script>
 </body>
</html>

