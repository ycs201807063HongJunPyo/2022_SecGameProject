#include "RectControl.h"
#include<stdlib.h>
#include<time.h>

RectControl::RectControl() {

}

int RectControl::CheckTile(int mouseX, int mouseY, int tileLeft, int tileTop, int tileRight, int tileBottom) {
	int check = 0;
	//정답 체크 1
	if (mouseX >= tileLeft && mouseX < tileRight) {
		if (mouseY >= tileTop && mouseY < tileBottom) {
			check = 1;
		}
	}
	//오류 체크 2
	if (mouseX <= (tileLeft - 2) || mouseX >= (tileRight + 2)) {
		check = 2;
	}
	if (mouseY <= (tileTop -2) || mouseY >= (tileBottom+2)) {
		check = 2;
	}
	return check;
}