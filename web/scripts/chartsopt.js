var optfc = {
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
var optpress = {
	title: {
	text: '压力'
	},
	tooltip: {trigger: 'axis'},
	legend: {x:'center',data:['Psys','Ph2']},
	//toolbox: {feature: {saveAsImage: {}}},
	xAxis: {title:'时间',data: []},
	yAxis: [{name:'Psys/Kpa',	type: 'value',axisLabel: {formatter: '{value}'}},
			{name:'Ph2/Kpa',	type: 'value',axisLabel: {formatter: '{value}'}}],
	series: [{name: 'Psys',type: 'line',data: []},
			 {name: 'Ph2',type: 'line',yAxisIndex:1,data: []}]
};
var optflow = {
	title: {
	text: '泵次与风扇转速'
	},
	tooltip: {trigger: 'axis'},
	legend: {x:'center',data:['泵次','转速']},
	//toolbox: {feature: {saveAsImage: {}}},
	xAxis: {title:'时间',data: []},
	yAxis: [{name:'次/10分',	type: 'value',axisLabel: {formatter: '{value}'}},
			{name:'RPM',	type: 'value',axisLabel: {formatter: '{value}'}}],
	series: [{name: '泵次',type: 'line',data: []},
			 {name: '转速',type: 'line',yAxisIndex:1,data: []}]
};