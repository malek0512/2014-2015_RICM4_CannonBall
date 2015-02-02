#include "AccelerometerSensor.h"


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
