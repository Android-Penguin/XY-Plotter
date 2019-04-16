#pragma config(Sensor, port1,  bottomBumper,   sensorVexIQ_Touch)//Bumper switch at the bottom end of the rack
#pragma config(Sensor, port5,  leftBumper,     sensorVexIQ_Touch)//Bumper switch on the left side of the carriage
#pragma config(Sensor, port6,  topBumper,      sensorVexIQ_Touch)//Bumper switch at the top end of the rack
#pragma config(Sensor, port7,  rightBumper,    sensorVexIQ_Touch)//Bumper swith on the right side of the carriage
#pragma config(Motor,  motor2,          carriageAxis,  tmotorVexIQ, PIDControl, encoder)//Motor that drives the sprocket on the chain axis
#pragma config(Motor,  motor3,          rackAxis,      tmotorVexIQ, PIDControl, reversed, encoder)//Motor that drives the pinion on the rack axis
#pragma config(Motor,  motor4,          liftArm,       tmotorVexIQ, PIDControl, encoder)//Motor that lifts and lowers the pen

const int targetMotorSpeed = 20;//Highest speed a motor will move at when drawing a line
const float xDegreesPerMM = 9;//Multiplyer to convert millimeters to degrees for the carriage axis
const float yDegreesPerMM = 9;//Multiplyer to convert millimeters to degrees for the rack axis

/************************************************
Moves the pen to an XY Coorinate on the whiteboard
*/
void moveTo(float xCoordMM, float yCoordMM) {
	//Converts millimeters on the whiteboard to degrees of motor rotation
	float xDegrees = xCoordMM * xDegreesPerMM;
	float yDegrees = yCoordMM * yDegreesPerMM;
	//Speed of the motor travelling on the shorter axis
	float reducedMotorSpeed;
	//Adjustment to the previus speed to compensate for inaccurate motor speed control
	float adjustedReducedMotorSpeed;

	/*Sets motor targets and speeds as follows
	 - Determines which axis has the furthest travel
	 - Sets the target position for both axes
	 - Sets longer axis travel speed to the target motor speed
	 - Sets the shorter axis travel speed proportional to that of the first so that both axes arrive at the target coordinate at the same time
	 - Adjusts the previous speed to compensate for inaccurate motor speed control*/
	if(abs(xDegrees - getMotorEncoder(carriageAxis)) > abs(yDegrees - getMotorEncoder(rackAxis))) {
		//X axis has greater travel
		setMotorTarget(carriageAxis, xDegrees, targetMotorSpeed);
		reducedMotorSpeed = targetMotorSpeed * abs((yDegrees - getMotorEncoder(rackAxis)) / (xDegrees - getMotorEncoder(carriageAxis)));
		adjustedReducedMotorSpeed = (1 + (((targetMotorSpeed - reducedMotorSpeed) / targetMotorSpeed) * 0.4)) * reducedMotorSpeed;
		setMotorTarget(rackAxis, yDegrees, adjustedReducedMotorSpeed);
	} else {
		//Y axis has greater travel
		reducedMotorSpeed = targetMotorSpeed * abs((xDegrees - getMotorEncoder(carriageAxis)) / (yDegrees - getMotorEncoder(rackAxis)));
		adjustedReducedMotorSpeed = (1 + (((targetMotorSpeed - reducedMotorSpeed) / targetMotorSpeed) * 0.53)) * reducedMotorSpeed;
		setMotorTarget(carriageAxis, xDegrees, adjustedReducedMotorSpeed);
		setMotorTarget(rackAxis, yDegrees, targetMotorSpeed);
	}
	waitUntilMotorMoveComplete(carriageAxis);
	waitUntilMotorMoveComplete(rackAxis);
}
/*********
Raises pen
*/
void penUp() {
	setServoTarget(liftArm, 0);
	sleep(600);
}

/*********
Lowers pen
*/
void penDown() {
	setMotorTarget(liftArm, 40, 10);
	waitUntilMotorMoveComplete(liftArm);
}
/***************************************************************************************************
Returns the robot to point (0,0) on the whitboard (Bottom left corner) and resets the motor encoders
This is done so that the robot is at a known location to start drawing from
*/
void goHome() {
	bool bottomBumperPressed = false;
	bool leftBumperPressed = false;

	setMotorSpeed(rackAxis, -100);
	setMotorSpeed(carriageAxis, -100);
	//Move to bottom left of drawing area until the left and bottom bumper switches are pressed
	while(bottomBumperPressed == false || leftBumperPressed == false) {
		if(getBumperValue(bottomBumper) == 1) {
			setMotorSpeed(rackAxis, 0);
			bottomBumperPressed = true;
		}
		if(getBumperValue(leftBumper) == 1) {
			setMotorSpeed(carriageAxis, 0);
			leftBumperPressed = true;
		}
	}
	bottomBumperPressed = false;
	leftBumperPressed = false;
	resetMotorEncoder(carriageAxis);
	resetMotorEncoder(rackAxis);
	sleep(1000);
}

task main() {
	//Test code to draw lines for debugging
	penUp();
	goHome();
	penDown();
	moveTo(20, 154);
	moveTo(20, 100);
	moveTo(200, 80);
	penUp();
	moveTo(432, 154);
}
