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
			return check;
		}
	}
	//오류 체크 2, 애매한 판정 3
	if (mouseX <= (tileLeft - 3) || mouseX >= (tileRight + 3)) {
		check = 2;
	}
	else {
		check = 3;
	}
	if (mouseY <= (tileTop -3) || mouseY >= (tileBottom+3)) {
		check = 2;
	}
	else {
		check = 3;
	}
	return check;
}