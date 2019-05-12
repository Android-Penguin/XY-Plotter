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
char line1Text [21] = "                    ";
char line2Text [21] = "                    ";
char line3Text [21] = "                    ";
char cursorLine [21] = "                    ";
char textWidth [21];

/*********************************************************
Deletes blank spaces after the user finishes editing text
*/
void rightTrim (char * stringToTrim) {
	int removeBlank;
	//Scans through the string and removes all blank spaces after the last character
	for (removeBlank = strlen(stringToTrim); removeBlank > 0; removeBlank--) {
		if(stringToTrim[removeBlank] == ' ') {
			stringToTrim[removeBlank] = 0;
		}
		else {
			break;
		}
	}
}

/*********************************************************
User text entry to select text to be written
*/
void selectText() {
	bool inputComplete = false;
	int cursorPos = 0;
	int currentEditLine = 1;
	int currentCharacter = ' ';
	int charIndex;
	int currentWidth;
	int maxWidth = 76;
	int remainingWidth;
	char * textToEdit;
	textToEdit = &line1Text;
	cursorLine[cursorPos] = '*';

	while(inputComplete == false) {
		/*********************************************************
		Changes the letter at the current cursor position
		*/
		if(getJoystickValue(BtnRUp) == 1 && currentCharacter < 'Z' && currentCharacter >= 'A') {
			//Changes the letter forward one character
			currentCharacter ++;
			//Changes the character in the array to the new character
			textToEdit[cursorPos] = currentCharacter;
			resetTimer(T2);
		}
		else {
			if(getJoystickValue(BtnRUp) == 1 && (currentCharacter == 'Z' || currentCharacter == ' ')) {
				//Sets the character to "A" if character is "Z" or a blank space
				currentCharacter = 'A';
				//Changes the character in the array to the new character
				textToEdit[cursorPos] = currentCharacter;
				resetTimer(T2);
			}
		}
		while(getJoystickValue(BtnRUp) == 1) {
			//Timer is used as a debounce timer to ensure the letter only changes by one
			if(getTimerValue(T2) > 300) {
				break;
			}
		}
		if(getJoystickValue(BtnLUp) == 1 && currentCharacter > 'A') {
			//Changes the letter back one character
			currentCharacter --;
			//Changes the array to the new character
			textToEdit[cursorPos] = currentCharacter;
			resetTimer(T2);
		}
		else {
			if(getJoystickValue(BtnLUp) == 1 && (currentCharacter == 'A' || currentCharacter == ' ')) {
				//Sets the character to "Z" if the character is "A" or a blank space
				currentCharacter = 'Z';
				//Changes the character in the array to the new character
				textToEdit[cursorPos] = currentCharacter;
				resetTimer(T2);
			}
		}
		while(getJoystickValue(BtnLUp) == 1) {
			//Timer is used as a debounce timer to ensure the letter only changes by one
			if(getTimerValue(T2) > 300) {
				break;
			}
		}

		/*********************************************************
		Moves the cursor left and right along the screen
		*/
		if(getJoystickValue(ChC) > 40 && cursorPos < 18) {
			//Replaces the cursor with a blank space
			cursorLine[cursorPos] = ' ';
			//Moves the cursor along one space
			cursorPos ++;
			//Replaces the blank space with a cursor
			cursorLine[cursorPos] = '*';
			//Sets the current character to the character that is currently selected
			currentCharacter = textToEdit[cursorPos];
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
			//Sets the current character to the character that is currently selected
			currentCharacter = textToEdit[cursorPos];
			resetTimer(T1);
		}
		while(getJoystickValue(ChC) < -40) {
			//Timer is used as a debounce timer to ensure cursor only moves one position along the screen
			if(getTimerValue(T1) > 300) {
				break;
			}
		}

		/*********************************************************
		Moves cursor up and down a line
		*/
		if(getJoystickValue(BtnEDown) ==1 && currentEditLine < 3) {
			currentEditLine++;
			resetTimer(T3);
		}
		while(getJoystickValue(BtnEDown) ==1) {
			//Timer is used as a debounce timer to ensure cursor only moves one line
			if(getTimerValue(T3) > 300) {
				break;
			}
		}
		if(getJoystickValue(BtnEUp) ==1 && currentEditLine > 1) {
			currentEditLine--;
			resetTimer(T3);
		}
		while(getJoystickValue(BtnEUp) ==1) {
			//Timer is used as a debounce timer to ensure cursor only moves one line
			if(getTimerValue(T3) > 300) {
				break;
			}
		}

		switch(currentEditLine) {
		case 1:
			textToEdit = &line1Text;
			displayTextLine(0, line1Text);
			displayTextLine(1, cursorLine);
			displayTextLine(2, line2Text);
			displayTextLine(3, line3Text);
			break;

		case 2:
			textToEdit = &line2Text;
			displayTextLine(0, line1Text);
			displayTextLine(1, line2Text);
			displayTextLine(2, cursorLine);
			displayTextLine(3, line3Text);
			break;

		case 3:
			textToEdit = &line3Text;
			displayTextLine(0, line1Text);
			displayTextLine(1, line2Text);
			displayTextLine(2, line3Text);
			displayTextLine(3, cursorLine);
			break;
		}

		/*********************************************************
		Keeps track of the character widths
		*/
		if(textToEdit[cursorPos] == 'M' || textToEdit[cursorPos] == 'W') {
			textWidth[cursorPos] = 8;
		}
		else {
			if(textToEdit[cursorPos] == 'G') {
				textWidth[cursorPos] = 7;
			}
			else {
				if(textToEdit[cursorPos] == 'A' || textToEdit[cursorPos] == 'O' || textToEdit[cursorPos] == 'Q') {
					textWidth[cursorPos] = 6;
				}
				else {
					if(textToEdit[cursorPos] == 'C' || textToEdit[cursorPos] == 'S' || textToEdit[cursorPos] == 'U' || textToEdit[cursorPos] == 'V') {
						textWidth[cursorPos] = 5;
					}
					else {
						textWidth[cursorPos] = 4;
					}
				}
			}
		}
		currentWidth = 0;
		for (charIndex = 0; charIndex<18; charIndex++) {
			currentWidth = currentWidth + textWidth[charIndex];
			remainingWidth = maxWidth-currentWidth;
			if(remainingWidth < 0) {
				textToEdit[charIndex] = ' ';
			}
		}

		/*********************************************************
		Removes blank spaces from end of each line
		*/
		if(getJoystickValue(BtnFUp) == 1) {
			rightTrim(line1Text);
			rightTrim(line2Text);
			rightTrim(line3Text);
			inputComplete = true;
		}
	}
}

/*********************************************************
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

/*********************************************************
Raises pen
*/
void penUp() {
	setServoTarget(liftArm, 0);
	penState = PENUP;
	sleep(600);
}

/*********************************************************
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

/*********************************************************
Prints one line of text from a string
*/
void printText (char * textToPrint) {
	resetMotorEncoder(carriageAxis);
	resetMotorEncoder(rackAxis);
	float  letterBaseX = getMotorEncoder(carriageAxis);
	float  letterBaseY = getMotorEncoder(rackAxis);
	float lineTargetX;
	float lineTargetY;
	float maxX = 0;
	float fontSize = 45;
	int charLookup;
	int characterIndex;
	int lineIndex;

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
}

task main() {
	penUp();
	goHome();
	resetMotorEncoder(carriageAxis);
	resetMotorEncoder(rackAxis);
	selectText();
	moveTo(0, 104);
	printText(line1Text);
	goHome();
	resetMotorEncoder(carriageAxis);
	resetMotorEncoder(rackAxis);
	moveTo(0, 52);
	printText(line2Text);
	goHome();
	resetMotorEncoder(carriageAxis);
	resetMotorEncoder(rackAxis);
	printText(line3Text);
	//Moves to the top right of the drawing area after printing all 3 lines
	moveTopRight();
}
