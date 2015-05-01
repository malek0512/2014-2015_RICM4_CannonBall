#include "AccelerometerSensor.h"

//The lines in comments are for compatible problems with the Ensimag project. The tablet we used doesn't have the same requirements

AccelerometerSensor::AccelerometerSensor() : _accelerometer(Accelerometer::GetDefault())
{
}


AccelerometerSensor::~AccelerometerSensor()
{
}

double AccelerometerSensor::getX()
{
	return 0;
	//return _accelerometer->GetCurrentReading()->AccelerationX;
}

double AccelerometerSensor::getY()
{
	return 0;
	//return _accelerometer->GetCurrentReading()->AccelerationY;
}

double AccelerometerSensor::getZ()
{
	return 0;
	//return _accelerometer->GetCurrentReading()->AccelerationZ;
}
