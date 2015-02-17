#!/usr/bin/env node

var path = require('path');
var mqtt = require('mqtt');
var mongoose = require('mongoose');
var app = require('http').createServer(handler);
var io = require('socket.io')(app);
var fs = require('fs');

/* Main program */
app.listen(1337);
var argv = process.argv;
var Accelerometer = mongoose.model('Accelerometer', { 
	time: Number, 
	x: Number, 
	y: Number, 
	z: Number 
});
var AccelerometerCounter = 0;

var Steering = mongoose.model('Steering', { 	
	time: Number, 
	value: Number 
});
var SteeringCounter = 0;

var Throttle = mongoose.model('Throttle', { 
	time: Number, 
	value: Number 
});
var ThrottleCounter = 0;

var FPS = mongoose.model('FPS', { 
	time: Number, 
	value: String 
});
var FPSCounter = 0;

var Mode = mongoose.model('Mode', { 
	value: String 
});

var NbMarkers = mongoose.model('NbMarkers', { 
	time: Number, 
	value: Number 
});
var NbMarkersCounter = 0;

var Closest = mongoose.model('Closest', { 
	time: Number, 
	value: Number 
});
var ClosestCounter = 0;

for (var i = 2; i <= 4; i++) {
    if(!argv[i]) {
        usage();
        process.exit(-1);
    }
}

var mqtt_host = argv[2],
    mqtt_port = argv[3],
    mongo_host = argv[4];

//*******************************************************
var client = mqtt.createClient(mqtt_port, mqtt_host);

//Crée la Data Base metrics dans l'instance courante de mongodb lancer comme service au prealable
mongoose.connect('mongodb://' + mongo_host + '/metrics');
var db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));

client.subscribe('metrics/accelerometer');
client.subscribe('metrics/steering');
client.subscribe('metrics/throttle');
client.subscribe('metrics/avg');
client.subscribe('metrics/mode');
client.subscribe('metrics/nb_markers');
client.subscribe('metrics/closest');
client.subscribe('camera/images');
client.subscribe('camera/commands');
console.log('Ready -- Listening for publishing ...\n');
/*client.subscribe('presence');
client.publish('presence', 'Hello mqtt');
*/
var nbClient = 0;
io.sockets.on('connection', function (socket) {
	nbClient ++;
	io.sockets.emit('new_client', nbClient);
	//Relai l'information sur le broker, pour que le programme main la recoit
	
	socket.on('camera/commands', function(data) {
		console.log('Publishing the commande from subscriber.js' + data.value.toString() +'\n');
		client.publish('camera/commands', data.value.toString());
	});
});

io.sockets.on('disconnect', function() {
    nbClient --;
	io.sockets.emit('disconnected_client', nbClient);
});

		
//io.sockets.on('connection', function () {
	client.on('message', function(topic, message) {
		if (topic != 'camera/images') {
			console.log('Client Lisening '+ topic.toString() + '  ' + message.toString());
		}
		if (topic === 'metrics/accelerometer') {
			
			var split = message.toString().split(':');
			var insert = new Accelerometer({ time : AccelerometerCounter,
											 x: split[0],
											 y: split[1],
											 z: split[2] });
			insert.save(function (err) {
				if (err)	console.log(err);
			});
			io.sockets.emit('Accelerometer', { time: AccelerometerCounter,
										   x: parseFloat(split[0]),
										   y: parseFloat(split[1]),
										   z: parseFloat(split[2]) });

			AccelerometerCounter++;
		} else if (topic === 'metrics/steering') {
			var insert = new Steering({ time: SteeringCounter, value: message });
			insert.save(function (err) {
				if (err)
					console.log(err);
			});
			io.sockets.emit('Steering', {time: SteeringCounter, value: parseInt(message) });
			SteeringCounter++;
		} else if (topic === 'metrics/throttle') {
			var insert = new Throttle({ time: ThrottleCounter, value: message });
			insert.save(function (err) {
				if (err)
					console.log(err);
			});
			io.sockets.emit('Throttle', { time: ThrottleCounter, value: parseInt(message) });
			ThrottleCounter++;
		} else if (topic === 'metrics/avg') {
			var insert = new FPS({ time: FPSCounter, value: (1/parseFloat(message)) * 1000 });
			insert.save(function (err) {
				if (err)
					console.log(err);
			});
			io.sockets.emit('FPS', { time: FPSCounter, value: (1/parseFloat(message)) * 1000 });
			FPSCounter++;
		} else if (topic == 'metrics/mode') {
			var insert = new Mode({ value: message });
			insert.save(function (err) {
				if (err)
					console.log(err);
			});
			io.sockets.emit('Mode', { value: message });
		} else if (topic == 'metrics/nb_markers') {
			var insert = new NbMarkers({ time: NbMarkersCounter, value: parseInt(message) });
			insert.save(function (err) {
				if (err)
					console.log(err);
			});
			io.sockets.emit('NbMarkers', { time: NbMarkersCounter, value: parseInt(message) });
			NbMarkersCounter++;
		} else if (topic == 'metrics/closest') {
			var insert = new Closest({ time: ClosestCounter, value: parseFloat(message) });
			insert.save(function (err) {
				if (err)
					console.log(err);
			});
			io.sockets.emit('Closest', { time: ClosestCounter, value: parseFloat(message) });
			ClosestCounter++;
		} else if (topic == 'camera/images') {
			console.log('Client Lisening '+ topic.toString() + '  ' + 'sent');
			//io.sockets.emit('Images', { value: message.toString('base64') });

			fs.readFile(__dirname + '/../../camera.jpeg', function(err, buf){
				//socket.emit('image', { image: true, buffer: buf });
				io.sockets.emit('image', { image: true, buffer: buf.toString('base64') });
				console.log('image file is initialized');
			});
  
		} 
	});
//});
client.options.reconnectPeriod = 60;

/* Functions */
function usage() {
    console.log('Usage : ' + argv[0] + ' ' + path.basename(argv[1])
            + ' mqtt_host mqtt_port mongo_host');
}

//Appelée a l'initialisation, sert a l'envoi du fichier view.html au client (voir tuto socket.io
function handler (req, res) {
    fs.readFile(__dirname + '/view.html',
            function (err, data) {
                if (err) {
                    res.writeHead(500);
                    return res.end('Error loading view.html');
                }

                res.writeHead(200);
                res.end(data);
            });
}
