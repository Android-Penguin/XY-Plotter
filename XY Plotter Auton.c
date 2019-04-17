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
	float scaleMultiplier = 10;//************************************************************Temporary size multiplier
	//Converts millimeters on the whiteboard to degrees of motor rotation
	float xDegrees = ((xCoordMM - 9) * scaleMultiplier) * xDegreesPerMM;//*******************Temporarily subtracting 9
	float yDegrees = ((yCoordMM - 11) * scaleMultiplier) * yDegreesPerMM;//******************Temporarily subtracting 11
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

/************************************************************************************
Moves the robot to the top right corner of the whiteboard.
This is done after finishing a drawing so that the text/lines are more easily visible
*/
void moveTopRight () {
	bool topBumperPressed = false;
	bool rightBumperPressed = false;

	setMotorSpeed(rackAxis, 100);
	setMotorSpeed(carriageAxis, 100);
	//Move to the top right of drawing area until the right and top bumper switches are pressed
	while(topBumperPressed == false || rightBumperPressed == false) {
		if(getBumperValue(topBumper) == 1) {
			setMotorSpeed(rackAxis, 0);
			topBumperPressed = true;
		}
		if(getBumperValue(rightBumper) == 1) {
			setMotorSpeed(carriageAxis, 0);
			rightBumperPressed = true;
		}
	}
	topBumperPressed = false;
	rightBumperPressed = false;
	sleep(1000);
}

task main() {
	//Test code to draw lines for debugging
	penUp();
	goHome();
	//S
	moveTo(15.5, 20);
	penDown();
	moveTo(14, 21);
	moveTo(12, 21);
	moveTo(10.5, 20);
	moveTo(11, 17.5);
	moveTo(13, 16.5);
	moveTo(14, 15);
	moveTo(14, 13);
	moveTo(13, 11.5);
	moveTo(11, 11);
	moveTo(9, 12);
	penUp();
	//h
	moveTo(17, 11);
	penDown();
	moveTo(17, 21);
	penUp();
	moveTo(17, 15);
	penDown();
	moveTo(18, 16);
	moveTo(20, 16);
	moveTo(21.5, 15);
	moveTo(21.5, 11);
	penUp();
	//a
	moveTo(26.5, 16.5);
	penDown();
	moveTo(26.5, 11);
	penUp();
	moveTo(26.5, 12);
	penDown();
	moveTo(26, 11);
	moveTo(24, 11);
	moveTo(22.5, 12);
	moveTo(22.5, 15);
	moveTo(24, 16);
	moveTo(26, 16);
	moveTo(26.5, 15);
	penUp();
	//u
	moveTo(28, 16.5);
	penDown();
	moveTo(28, 12);
	moveTo(28.5, 11);
	moveTo(31, 11);
	moveTo(32, 12);
	penUp();
	moveTo(32, 16.5);
	penDown();
	moveTo(32, 11);
	penUp();
	//n
	moveTo(33.5, 16.5);
	penDown();
	moveTo(33.5, 11);
	penUp();
	moveTo(33.5, 15.5);
	penDown();
	moveTo(35, 16.5);
	moveTo(37.5, 16.5);
	moveTo(38, 15.5);
	moveTo(38, 11);


	penUp();
	moveTopRight();
}
