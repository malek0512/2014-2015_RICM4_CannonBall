START "Mosquitto Broker" 	mosquitto.exe -p 1883 -v
START "MongoDB" 			mongod.exe --journal --rest --dbpath C:\data\
START "Server" 				node subscriber.js localhost 1883 localhost