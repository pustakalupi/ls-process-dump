const {ipcRenderer} = require('electron');
var osu = require('os-utils');
var nbind = require('nbind');
var lib = nbind.init("build/Release").lib;

var options = {
    maintainAspectRatio: false,
    scales: {
        yAxes: [{
            stacked: true,
            gridLines: {
                display: true,
                color: "rgba(255,99,132,0.2)"
            }
        }],
        xAxes: [{
            gridLines: {
                display: false
            }
        }]
    }
};

$( document ).ready(function() {
    $("#browse").click(function(){
        if($("#btn-start").hasClass( "disabled" ) == true){
            return;
        }

        ipcRenderer.send('open-file', () => {
            console.log("Event sent.");
        });
    });

    ipcRenderer.on('set-directory-path', (event, arg) => {
        $("#browse-result").val(arg);
    });

    showCPUChart();
    showMemoryChart();

    setInterval(function(){
        $('#process-table').html('');
        var obj = lib.ProcessWalker.walkProcess();
        obj.forEach(function(item, index){
            $('#process-table').append(
                `<tr>
                <th scope="row">` + index + `</th>
                <td>`+ item.exeFileName +`</td>
                <td>`+ item.size +`</td>
                <td>`+ item.processID +`</td>
                <td>`+ item.threadCount +`</td>
                <td>`+ item.parentProcessID +`</td>
                <td>`+ item.basePriority +`</td>
              </tr>`
            );
            //console.log(item.processVMUsage + "," + item.exeFullPath);
        });
    }, 1000);
});

function showCPUChart(){
    var configCPUUsage = {
        type: 'line',
        data: {
            labels: ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"],
            datasets: [{
                label: "CPU Usage Over Time",
                data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                fill: false
            }]
        },
        options: options
    };
      
    var cpuUsageChart = new Chart(document.getElementById("cpuUsageChart").getContext("2d"), configCPUUsage);

    setInterval(function(){      
        osu.cpuUsage(function(v){
            var cpuUsageVal = v;
            configCPUUsage.data.datasets[0].data.shift();
            configCPUUsage.data.datasets[0].data.push(cpuUsageVal * 100);
            cpuUsageChart.update();
        });
    }, 1000);
}

function showMemoryChart(){
    var configMemoryUsage = {
        type: 'line',
        data: {
            labels: ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"],
            datasets: [{
                label: "Memory Usage Over Time",
                data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                fill: false
            }]
        },
        options: options
    };
    
    var memoryUsageChart = new Chart(document.getElementById("memoryUsageChart").getContext("2d"), configMemoryUsage);

    setInterval(function(){
        configMemoryUsage.data.datasets[0].data.shift();
        configMemoryUsage.data.datasets[0].data.push(osu.freememPercentage() * 100);
        memoryUsageChart.update();
    }, 1000);
}