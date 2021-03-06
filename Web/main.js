var express = require('express');
var appMain = express();
var serverMain = require('http').Server(appMain);
var fs = require('fs');
var mySockets = [];
var localData = {};
localData.loc = {};
localData.loc.ipAddress = process.argv[3];
localData.loc.port = process.argv[4];
localData.lastHeartBeat = new Date().getTime();
localData.jobs = {};
localData.clients = {};

appMain.get('/', function(req, res) {
    res.sendFile(__dirname + '/gui/index.html');
});
appMain.use('/gui', express.static(__dirname + '/gui'));

var net = require('net');
var nextConvId = 1;

var client = new net.Socket();
client.connect(process.argv[2], '127.0.0.1', function() {
  //  console.log('Connected');
    client.write(JSON.stringify({
      convId: '0',
      msg: 'Hello, server! Love, Client.'
    }));
});

client.on('data', function(data) {
    var msgs = data.toString();
    var msg;
    //console.log("received data" + mySockets.length);
    while (msgs) {
        var index = msgs.indexOf("~");
        if(index == -1)index = msgs.length;
        msg = msgs.substring(0, index);
        msgs = msgs.substring(index+1);
        var temp = JSON.parse(msg.toString());
        switch (temp.MsgType) {
            case "Heartbeat":
                localData.lastHeartBeat = new Date().getTime();
                client.write(JSON.stringify({MsgType:'HeartbeatAck', convId: temp.convId}) + '~');
                break;
            case 'AddJobAck':
               // console.log("received AddJobAck" + mySockets.length);
                for(var i = 0; i < mySockets.length; i++) {
                 //   console.log('Emitting' + temp.JobId);
                    mySockets[i].emit('AddJobAck', temp);
                }
                  localData.jobs[JSON.stringify(temp.JobId)] = temp;
                break;
            case 'updateProgress':
              for(var i = 0; i < mySockets.length; i++) {
                mySockets[i].emit('jobsProgress', temp.progress);
              }
              for(var i = 0; i < temp.progress.length; i++) {
                localData.jobs[temp.progress[i].JobId].progress = temp.progress[i].progress;
              }
              break;
            case 'newClient':
              for(var i = 0; i < mySockets.length; i++) {
                mySockets[i].emit('newClient', temp);
              }  
              localData.clients[JSON.stringify(temp.clientId)] = temp;
              break;
            case 'lostClient':
              for(var i = 0; i < mySockets.length; i++) {
                mySockets[i].emit('lostClient', temp);
              }  
              delete localData.clients[JSON.stringify(temp.clientId)];
              break;
            default:
              //console.log("bad message received");
        }
    }
});

client.on("error", function(err) {
    //console.log(err.stack)
});

client.on('close', function() {
    //console.log('Connection closed');
});

serverMain.listen(process.env.PORT || 8080);

var ioMain = require('socket.io')(serverMain, {});
ioMain.sockets.on('connection', function(socket) {
    mySockets.push(socket);
    socket.on('systemData', function(data) {
        switch(data.MsgType) {
            case 'AddJob':
                data.convId = nextConvId++;
                client.write(JSON.stringify(data) + '~');
                //console.log("Adding Job");
                break;
            case 'RequestIp':
                socket.emit('RequestIpAck', localData.loc);
                break;
            case 'RequestCurrentJobs':
                //console.log('requesting current jobs');
                socket.emit('RequestCurrentJobsAck', localData.jobs);
                break;
            case 'RequestCurrentClients':
                //console.log('requesting current clients');
                socket.emit('RequestCurrentClientsAck', localData.clients);
                break;
          default:
            //console.log('cannot handle msgtype: ' + data.MsgType);
        }
    });
    socket.on('disconnect', function(){
        mySockets.splice(mySockets.indexOf(socket), 1);
        //console.log('disconnnected');
    });
});

var checkRefresh = function() {
  if(localData.lastHeartBeat + 3000 < new Date()){
    console.log('lost to the server, shutting down');
    process.exit(0);
  }
}

setInterval(checkRefresh, 1000);

