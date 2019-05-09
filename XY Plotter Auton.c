#pragma config(Sensor, port1,  bottomBumper,   sensorVexIQ_Touch)//Bumper switch at the bottom end of the rack
#pragma config(Sensor, port5,  leftBumper,     sensorVexIQ_Touch)//Bumper switch on the left side of the carriage
#pragma config(Sensor, port6,  topBumper,      sensorVexIQ_Touch)//Bumper switch at the top end of the rack
#pragma config(Sensor, port7,  rightBumper,    sensorVexIQ_Touch)//Bumper switch on the right side of the carriage
#pragma config(Sensor, port8,  debugLED,       sensorVexIQ_LED)//Temporary debug LED for testing
#pragma config(Motor,  motor2,          carriageAxis,  tmotorVexIQ, PIDControl, encoder)//Motor that drives the sprocket on the chain axis
#pragma config(Motor,  motor3,          rackAxis,      tmotorVexIQ, PIDControl, reversed, encoder)//Motor that drives the pinion on the rack axis
#pragma config(Motor,  motor4,          liftArm,       tmotorVexIQ, PIDControl, encoder)//Motor that lifts and lowers the pen

#include "XLetterCoords.c"
#include "YLetterCoords.c"
#define PENUP true
#define PENDOWN false
bool penState = PENUP;
const int targetMotorSpeed = 25;//Highest speed a motor will move at when drawing a line
const float xDegreesPerMM = 9;//Multiplyer to convert millimeters to degrees for the carriage axis
const float yDegreesPerMM = 9;//Multiplyer to convert millimeters to degrees for the rack axis
char textToPrint [21] = "                    ";
char cursorLine [21] = "                    ";

/************************************************
User text entry to select text to be written
*/
void selectText() {
	bool inputComplete = false;
	int cursorPos = 0;
	int currentCharacter = 65;
	int removeBlank;
	textToPrint[cursorPos] = currentCharacter;
	cursorLine[cursorPos] = '*';

	while(inputComplete == false) {
		displayTextLine(0, textToPrint);
		displayTextLine(1, cursorLine);

		//Button pressed when user finishes editing
		if(getJoystickValue(BtnFUp) == 1) {
			//Scans through the array of text to print and removes all blank spaces after the last character
			for (removeBlank = 19; removeBlank > 0; removeBlank--) {
				if(textToPrint[removeBlank] == ' ') {
					textToPrint[removeBlank] = 0;
				}
				else {
					break;
				}
			}
			inputComplete = true;
		}
		/*********************************************************
		Moves the cursor left and right along the screen
		*/
		if(getJoystickValue(ChC) > 40 && cursorPos < 10) {
			//Replaces the cursor with a blank space
			cursorLine[cursorPos] = ' ';
			//Moves the cursor along one space
			cursorPos ++;
			//Replaces the blank space with a cursor
			cursorLine[cursorPos] = '*';

			if(textToPrint[cursorPos] == ' ') {
				//Sets current character to 1 before an A so that A is the first character selected from a blank space
				currentCharacter = 64;
			}
			else {
				//If current character is not a blank space sets the current character to the character that is currently selected
				currentCharacter = textToPrint[cursorPos];
			}
			resetTimer(T1);
		}
		while(getJoystickValue(ChC) > 40) {
			//Timer is used as a debounce timer to ensure cursor only moves one position along the screen
			if(getTimerValue(T1) > 300) {
				break;
			}
		}
		if(getJoystickValue(ChC) < -40 && cursorPos > 0) {
			//Replaces the cursor with a blank space
			cursorLine[cursorPos] = ' ';
			//Moves the cursor along one space
			cursorPos --;
			//Replaces the blank space with a cursor
			cursorLine[cursorPos] = '*';

			if(textToPrint[cursorPos] == ' ') {
				//Sets current character to 1 before an A so that A is the first character selected from a blank space
				currentCharacter = 64;
			}
			else {
				//If current character is not a blank space sets the current character to the character that is currently selected
				currentCharacter = textToPrint[cursorPos];
			}
			resetTimer(T1);
		}
		while(getJoystickValue(ChC) < -40) {
			//Timer is used as a debounce timer to ensure cursor only moves one position along the screen
			if(getTimerValue(T1) > 300) {
				break;
			}
		}
		/*********************************************************
		Changes the character at the current cursor position
		*/
		if(getJoystickValue(BtnRUp) == 1 && currentCharacter < 67) {
			//Changes the letter forward one character
			currentCharacter ++;
			//Changes the array to the new character
			textToPrint[cursorPos] = currentCharacter;
			resetTimer(T2);
		}
		while(getJoystickValue(BtnRUp) == 1) {
			//Timer is used as a debounce timer to ensure the letter is only changes by one
			if(getTimerValue(T2) > 300) {
				break;
			}
		}
		if(getJoystickValue(BtnLUp) == 1 && currentCharacter > 65) {
			//Changes the letter back one character
			currentCharacter --;
			//Changes the array to the new character
			textToPrint[cursorPos] = currentCharacter;
			resetTimer(T2);
		}
		while(getJoystickValue(BtnLUp) == 1) {
			//Timer is used as a debounce timer to ensure the letter is only changes by one
			if(getTimerValue(T2) > 300) {
				break;
			}
		}
	}
}

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
		if(getMotorEncoder(carriageAxis) != xDegrees) {
			reducedMotorSpeed = targetMotorSpeed * abs((yDegrees - getMotorEncoder(rackAxis)) / (xDegrees - getMotorEncoder(carriageAxis)));
		}
		else {
			reducedMotorSpeed = targetMotorSpeed;
		}
		adjustedReducedMotorSpeed = (1 + (((targetMotorSpeed - reducedMotorSpeed) / targetMotorSpeed) * 0.4)) * reducedMotorSpeed;
		setMotorTarget(rackAxis, yDegrees, reducedMotorSpeed);
		} else {
		//Y axis has greater travel
		if(getMotorEncoder(rackAxis) != yDegrees) {
			reducedMotorSpeed = targetMotorSpeed * abs((xDegrees - getMotorEncoder(carriageAxis)) / (yDegrees - getMotorEncoder(rackAxis)));
		}
		else {
			reducedMotorSpeed = targetMotorSpeed;
		}
		adjustedReducedMotorSpeed = (1 + (((targetMotorSpeed - reducedMotorSpeed) / targetMotorSpeed) * 0.53)) * reducedMotorSpeed;
		setMotorTarget(carriageAxis, xDegrees, reducedMotorSpeed);
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
	penState = PENUP;
	sleep(600);
}

/*********
Lowers pen
*/
void penDown() {
	setMotorTarget(liftArm, 40, 10);
	penState = PENDOWN;
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
	float  letterBaseX = getMotorEncoder(carriageAxis);
	float  letterBaseY = getMotorEncoder(rackAxis);
	float lineTargetX;
	float lineTargetY;
	float maxX = 0;
	float fontSize = 30;
	int charLookup;
	int characterIndex;
	int lineIndex;

	penUp();
	goHome();
	resetMotorEncoder(carriageAxis);
	resetMotorEncoder(rackAxis);
	letterBaseX = getMotorEncoder(carriageAxis);
	letterBaseY = getMotorEncoder(rackAxis);
	selectText();

	//Scans through the "text to print" array character by character and prints it
	for (characterIndex = 0; characterIndex<strlen(textToPrint); characterIndex++) {
		//Letters have an ASCII value between 65 and 90
		if(textToPrint[characterIndex] >= 65 && textToPrint[characterIndex] <= 90) {
			/*Locates the letter in the x and y coordinate arrays. The minus 65 is used to offset the index because
			"A" is listed as 0 and not 65 in the coordinate arrays*/
			charLookup = textToPrint[characterIndex] - 65;
		}
		//Numbers have an ASCII value between 48 and 57
		if(textToPrint[characterIndex] >= 48 && textToPrint[characterIndex] <= 57) {
			/*Locates the letter in the x and y coordinate arrays. The minus 49 is used to offset the index because
			"1" is listed as 4 (Will be 27 when all letters are added) and not 48 in the coordinate arrays.
			The plus 3 is used to offset the index because there are 3 letters before the "1" (Will be 26 once all letters are added)*/
			charLookup = textToPrint[characterIndex] - 49 + 3;
		}
		//Changes the base x position one character width for a space
		if(textToPrint[characterIndex] == ' ') {
			letterBaseX = letterBaseX + ((fontSize / 5) * 3);
		}
		else {
			lineIndex = 0;
			while(lineIndex < 20) {
				//A "-3" in the coordinate arrays represents the end of a character
				if(charYLines[charLookup][lineIndex] == -3) {
					break;
				}
				//A "-2" in the coordinate arrays represents lifting the pen
				if(charYLines[charLookup][lineIndex] == -2) {
					penUp();
				}
				else {
					//Sets the target x and y positions based on the current position (letterBaseX), the coordinates in the arrays and the font size
					/*Relative X and Y coordinates for the strokes of each character are stored as a normalised value with a range of 0 to 100.
					100 is the height of a capital letter*/
					lineTargetX = letterBaseX + ((charXLines[charLookup][lineIndex] * fontSize)/100);
					lineTargetY = letterBaseY + ((charYLines[charLookup][lineIndex] * fontSize)/100);
					moveTo(lineTargetX, lineTargetY);
					if(lineTargetX > maxX) {
						maxX = lineTargetX;
					}
					/*If the pen is raised it means we've completed one stroke and have moved to the begining of the next therefore the
					pen needs to be lowered to start drawing. e.g. moving between letter*/
					if(penState == PENUP) {
						penDown();
					}
				}
				//Indexes through the coordinates of each letter line by line
				lineIndex ++;
			}
			//Raises the pen at the end of a character and sets the base x position for the next letter
			penUp();
			letterBaseX = maxX + fontSize / 10;
		}
	}
	//Moves to the top right of the drawing area after printing all the letters in the
	moveTopRight();
}
