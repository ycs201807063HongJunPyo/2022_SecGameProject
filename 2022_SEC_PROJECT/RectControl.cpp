#include "RectControl.h"
#include<stdlib.h>
#include<time.h>

RectControl::RectControl() {

}

int RectControl::CheckTile(int mouseX, int mouseY, int tileLeft, int tileTop, int tileRight, int tileBottom) {
	int check = 0;
	if (mouseX >= tileLeft && mouseX < tileRight) {
		if (mouseY >= tileTop && mouseY < tileBottom) {
			check = 1;
		}
	}
	return check;
}