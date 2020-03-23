
#include "Chart.hpp"
#include "GreatEffect.h"

void Chart::Show() const {
	ShowEditor();
	if (isPlaying) {
		ShowPlayer();
	}
}

void Chart::ShowPlayer() const {
	//���`�悷�ׂ����[�������l����
	Array<bool>nowDisplayedLane(divNum);

	//�o�b�N�̉摜��`�悷��

	//���S�̑��p�`��`�悷��

	//���[����������у����O�m�[�c��`�悷��
	for (auto lane : step(divNum)) {
		for (int i = 0; i < laneVector[lane].size(); ++i) {
			auto& background = laneVector[lane][i];
			Quot timeStart = background.first;
			Quot timeEnd = (i + 1 != laneVector[lane].size()) ? laneVector[lane][i + 1].first : imosTime.back();
			double leftPosNow = divPosReal(lane, clockToRealTime(currentPlayClock));
			double rightPosNow = divPosReal((lane + 1) % divNum, clockToRealTime(currentPlayClock));
			String colorLong = background.second.first;
			String colorDisp = background.second.second;
			//�ɍ��W�ɕϊ�����
			double radiusLow = Max(timeToHSTime(timeStart).real() - realTimeToRealHSTime(clockToRealTime(currentPlayClock)), 0.0) * noteSpeed + polygonRadius;
			double radiusHigh = Max(timeToHSTime(timeEnd).real() - realTimeToRealHSTime(clockToRealTime(currentPlayClock)), 0.0) * noteSpeed + polygonRadius;
			double realThetaLeft = leftPosNow * Math::Pi * 2;
			double realThetaRight = rightPosNow * Math::Pi * 2;
			//�����胉�C����ʉߒ��̃m�[�c���H
			bool currentDisplaying = (timeToClock(timeStart) <= currentPlayClock && currentPlayClock <= timeToClock(timeEnd));
			if (currentDisplaying) {
				nowDisplayedLane[lane] = true;
			}
			//���ۂɕ`��
			if (colorDisp != noColor && currentDisplaying) {
				Quad(polToRec(polygonRadius, realThetaLeft),
					polToRec(polygonRadius, realThetaRight),
					polToRec(Max(Window::ClientWidth(), Window::ClientHeight()), realThetaRight),
					polToRec(Max(Window::ClientWidth(), Window::ClientHeight()), realThetaLeft)).draw(Color(colorDisp).setA(displayAlpha));
			}
			if (colorLong != noColor && nowDisplayedLane[lane]) {
				if (nowDisplayedLane[lane] == true) {
					Quad(polToRec(radiusLow, realThetaLeft),
						polToRec(radiusLow, realThetaRight),
						polToRec(radiusHigh, realThetaRight),
						polToRec(radiusHigh, realThetaLeft)).draw(Color(colorLong).setA(255));
				}
			}
		}
	}
	//�m�[�c��`�悷��
	for (auto measure : step(measureNum)) {
		for (auto lane : step(divNum)) {
			for (auto elm : laneNoteData[measure][lane]) {
				Quot time = imosTime[measure] + elm.first;
				//���̃t���[���ŏ������m�[�c�̓^�b�v�G�t�F�N�g��������
				auto clockTime = timeToClock(time);
				if (previousPlayClock < clockTime && clockTime <= currentPlayClock) {
					double leftPos = divPosReal(lane, clockToRealTime(currentPlayClock));
					double rightPos = divPosReal((lane + 1) % divNum, clockToRealTime(currentPlayClock));
					//�ɍ��W�ɕϊ�
					double radius = (timeToHSTime(time).real() - realTimeToRealHSTime(clockToRealTime(currentPlayClock))) * noteSpeed + polygonRadius;
					double realThetaLeft = leftPos * Math::Pi * 2;
					double realThetaRight = rightPos * Math::Pi * 2;
					//�G�t�F�N�g��ǉ�
					greatEffect.add<GreatEffect>((polToRec(radius, realThetaLeft) + polToRec(radius, realThetaRight)) / 2, U"PERFECT");
				}
				//���łɉ߂��������m�[�c�A�܂��`�悷��ɂ͑����m�[�c�̏ꍇ�͕`������Ȃ�
				if (clockTime <= currentPlayClock || currentPlayClock + renderNoteMilliseconds <= clockTime)continue;

				if (noteColor.contains(elm.second)) {
					double leftPos = divPosReal(lane, clockToRealTime(currentPlayClock));
					double rightPos = divPosReal((lane + 1) % divNum, clockToRealTime(currentPlayClock));
					//�ɍ��W�ɕϊ�
					double radius = (timeToHSTime(time).real() - realTimeToRealHSTime(clockToRealTime(currentPlayClock))) * noteSpeed + polygonRadius;
					double realThetaLeft = leftPos * Math::Pi * 2;
					double realThetaRight = rightPos * Math::Pi * 2;
					//���ۂɕ`��
					Quad(polToRec(radius - notePlayDelta, realThetaLeft),
						polToRec(radius - notePlayDelta, realThetaRight),
						polToRec(radius + notePlayDelta, realThetaRight),
						polToRec(radius + notePlayDelta, realThetaLeft)).draw(Color(noteColor.at(elm.second)));
				}
				else {
					//���݂��Ȃ���ނ̃m�[�c�ł���
					//�G���[�̂ق����ǂ��H
				}
			}
		}
	}
	//���ߐ���`�悷��


	//�G�t�F�N�g��`��
	greatEffect.update();

}

void Chart::ShowEditor() const {
	//�e���[���̔w�i��`��
	for (auto lane : step(divNum)) {
		for (int i = 0; i < laneVector[lane].size(); ++i) {
			auto& background = laneVector[lane][i];
			Quot timeStart = background.first;
			Quot leftPosStart = divPos(lane, timeStart);
			Quot rightPosStart = divPos((lane + 1) % divNum, timeStart);
			if (rightPosStart < leftPosStart) {
				rightPosStart += Quot(1, 1);
			}
			Quot timeEnd = (i + 1 != laneVector[lane].size()) ? laneVector[lane][i + 1].first : imosTime.back();
			Quot leftPosEnd = leftPosStart + divSpeed(lane, timeStart) * (timeEnd - timeStart);
			Quot rightPosEnd = rightPosStart + divSpeed((lane + 1) % divNum, timeStart) * (timeEnd - timeStart);
			if (!isOk(rightPosStart - leftPosStart) || !isOk(rightPosEnd - leftPosEnd)) {
				//invalid�ȃm�[�c�Ȃ̂ŕ`��ł��Ȃ�
				continue;
			}
			auto KBL = leftPosStart.gauss();
			auto KBR = rightPosStart.gauss();
			auto KEL = leftPosEnd.gauss();
			auto KER = rightPosEnd.gauss();
			auto KL = Min({ KBL, KBR, KEL, KER }) - 1;
			auto KR = Max({ KBL, KBR, KEL, KER }) + 1;
			//��ʂ���͂ݏo�������͕����񂸂炵�ĕ`��
			int renderCount = 0;
			for (auto K : Range(KL, KR + 1)) {
				if (background.second.second != noColor) {
					//�����̕`�������
					Quad(Vec2(toX(leftPosStart - Quot(K)), toY(timeStart)),
						Vec2(toX(leftPosEnd - Quot(K)), toY(timeEnd)),
						Vec2(toX(rightPosEnd - Quot(K)), toY(timeEnd)),
						Vec2(toX(rightPosStart - Quot(K)), toY(timeStart))).draw(Color(background.second.second).setA(displayAlpha));
				}
				if (background.second.first != noColor) {
					//�����O�m�[�c�̕`�������
					Quad(Vec2(toX(leftPosStart - Quot(K)), toY(timeStart)),
						Vec2(toX(leftPosEnd - Quot(K)), toY(timeEnd)),
						Vec2(toX(rightPosEnd - Quot(K)), toY(timeEnd)),
						Vec2(toX(rightPosStart - Quot(K)), toY(timeStart))).draw(Color(background.second.first).setA(longNoteAlpha));
				}
				++renderCount;
				if (renderCount >= renderLimit) {
					break;
				}
			}
		}
	}
	//��������`��
	for (auto lane : step(divNum)) {
		for (int i = 0; i < splitVector[lane].size(); ++i) {
			Quot timeBegin = splitVector[lane][i].first;
			Quot theta = splitVector[lane][i].second.first;
			Quot omega = splitVector[lane][i].second.second;
			Quot timeEnd = (i + 1 != splitVector[lane].size()) ? splitVector[lane][i + 1].first : imosTime.back();
			Quot endTheta = (timeEnd - timeBegin) * omega + theta;
			//��ʂ���͂ݏo�������͕����񂸂炵�ĕ`��
			auto kBegin = theta.gauss();
			auto kEnd = endTheta.gauss();
			auto KL = Min(kBegin, kEnd) - 1;
			auto KR = Max(kBegin, kEnd) + 1;
			int renderCount = 0;
			//(theta,timeBegin)����(endTheta,timeEnd)�܂ł̐�������
			for (auto K : Range(KL, KR + 1)) {
				Line(Vec2(toX(theta - Quot(K, 1)), toY(timeBegin)), Vec2(toX(endTheta - Quot(K, 1)), toY(timeEnd))).draw(divLineWidth, Color(divLineColor));
				++renderCount;
				if (renderCount >= renderLimit) {
					break;
				}
			}
		}
	}
	//�ꔏ(granularity�ˑ�)���Ƃɕ⏕����`��
	if (granularity <= renderLimit && isTabMenu) {
		for (auto tick : tickTime) {
			Line(Vec2(toX(Quot(-1, 1)), toY(tick)), Vec2(toX(Quot(1, 1)), toY(tick))).draw(tickLineWidth, Color(tickLineColor));
		}
	}
	//���ߐ���`��
	for (auto measure : step(measureNum)) {
		if (isDrawMeasureLine[measure]) {
			Line(Vec2(toX(Quot(-1, 1)), toY(imosTime[measure])), Vec2(toX(Quot(1, 1)), toY(imosTime[measure]))).draw(measureLineWidth, Color(measureLineColor));
		}
	}
	//Shift�������Ă���Ԑ�Έʒu�̐���`��
	if (KeyShift.pressed()) {
		Line(Vec2(toX(Quot(0, 1)), toY(Quot(0, 1))), Vec2(toX(Quot(0, 1)), toY(imosTime.back()))).draw(Color(absoluteLineColor));
	}
	//�P���̃m�[�c�Ƒ�����`��
	for (auto measure : step(measureNum)) {
		for (auto lane : step(divNum)) {
			if (KeyLShift.pressed()) {
				//�V�t�g��������Ă���Ƃ��ɑ������\������
				for (auto elm : laneDisplayData[measure][lane]) {
					Quot time = imosTime[measure] + elm.first;
					Quot leftPos = divPos(lane, time);
					Quot rightPos = divPos((lane + 1) % divNum, time);
					if (rightPos < leftPos) {
						rightPos += Quot(1, 1);
					}
					if (elm.second != noColor) {
						//���̈ʒu�Ƃ��ꂼ�ꍶ�E�ɕ`�悷��
						Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(displayThickness, Color(elm.second));
						Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(displayThickness, Color(elm.second));
						Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(displayThickness, Color(elm.second));
						if (isTabMenu && isLaneMode) {
							//�^�u���j���[���J����Ă��Ă���Lane mode�̎�����Shift�������A�����̐F(#xxxxxx�\�L)��\��
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos) + toX(rightPos)) / 2, toY(time)), Color(elm.second));
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos - Quot(1, 1)) + toX(rightPos - Quot(1, 1))) / 2, toY(time)), Color(elm.second));
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos + Quot(1, 1)) + toX(rightPos + Quot(1, 1))) / 2, toY(time)), Color(elm.second));
						}
					}
					else {
						//noColor���`�悷��
						//���̈ʒu�Ƃ��ꂼ�ꍶ�E�ɕ`�悷��
						Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(displayThickness, Color(blackString));
						Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(displayThickness, Color(blackString));
						Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(displayThickness, Color(blackString));
						if (isTabMenu && isLaneMode) {
							//�^�u���j���[���J����Ă��Ă���Lane mode�̎�����Shift�������AnoColor��\��
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos) + toX(rightPos)) / 2, toY(time)), Color(elm.second));
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos - Quot(1, 1)) + toX(rightPos - Quot(1, 1))) / 2, toY(time)), Color(blackString));
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos + Quot(1, 1)) + toX(rightPos + Quot(1, 1))) / 2, toY(time)), Color(blackString));
						}
					}
				}
			}
			for (auto elm : laneNoteData[measure][lane]) {
				Quot time = imosTime[measure] + elm.first;
				if (noteColor.contains(elm.second)) {
					Quot leftPos = divPos(lane, time);
					Quot rightPos = divPos((lane + 1) % divNum, time);
					if (rightPos < leftPos) {
						rightPos += Quot(1, 1);
					}
					//���̈ʒu�Ƃ��ꂼ�ꍶ�E�ɕ`�悷��
					Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(noteThickness, Color(noteColor.at(elm.second)));
					Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(noteThickness, Color(noteColor.at(elm.second)));
					Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(noteThickness, Color(noteColor.at(elm.second)));
					if (isTabMenu && isLaneMode) {
						//�^�u���j���[���J����Ă��Ă���Lane mode�̎��A�m�[�c��ID��\��
						editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos) + toX(rightPos)) / 2, toY(time)), Color(noteColor.at(elm.second)));
						editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos - Quot(1, 1)) + toX(rightPos - Quot(1, 1))) / 2, toY(time)), Color(noteColor.at(elm.second)));
						editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos + Quot(1, 1)) + toX(rightPos + Quot(1, 1))) / 2, toY(time)), Color(noteColor.at(elm.second)));
					}
				}
				else {
					//���݂��Ȃ���ނ̃m�[�c�ł���
					//�G���[�̂ق����ǂ��H
				}
			}
		}
	}
	//�S�[�X�g�m�[�c��\��
	if (isLaneMode && ghostNoteLane >= 0) {
		Quot leftPos = divPos(ghostNoteLane, ghostNoteTime);
		Quot rightPos = divPos((ghostNoteLane + 1) % divNum, ghostNoteTime);
		if (rightPos < leftPos)rightPos += Quot(1, 1);
		Quot time = ghostNoteTime;
		//���̈ʒu�Ƃ��ꂼ�ꍶ�E�ɕ`�悷��
		if (KeyLShift.pressed()) {
			//����
			Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(noteThickness, Color(nowDispString).setA(ghostAlpha));
			Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(noteThickness, Color(nowDispString).setA(ghostAlpha));
			Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(noteThickness, Color(nowDispString).setA(ghostAlpha));
		}
		else {
			//�m�[�c
			Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(noteThickness, Color(noteColor.at(nowNoteString)).setA(ghostAlpha));
			Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(noteThickness, Color(noteColor.at(nowNoteString)).setA(ghostAlpha));
			Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(noteThickness, Color(noteColor.at(nowNoteString)).setA(ghostAlpha));
		}
	}
	//�������ҏW����\��
	if (!isLaneMode) {
		Line(Vec2(toX(Quot(-1, 1)), toY(ghostNoteTime)), Vec2(toX(Quot(2, 1)), toY(ghostNoteTime))).draw(divEditLineWidth, Color(divEditLineColor).setA(ghostAlpha));
		Line(Vec2(toX(Quot(-1, 1)), toY(divEditLineTime)), Vec2(toX(Quot(2, 1)), toY(divEditLineTime))).draw(divEditLineWidth, Color(divEditLineColor));
	}
	//�܂���̐ߖڂ�`��
	for (auto lane : step(divNum)) {
		for (int i = 0; i < splitVector[lane].size(); ++i) {
			Quot timeBegin = splitVector[lane][i].first;
			Quot theta = splitVector[lane][i].second.first;
			Color c = Palette::White;
			if (timeBegin == divEditLineTime) {
				for (auto x : step(divNum)) {
					if (divEditIndex[x] && divPos(x, timeBegin) == theta) {
						c = Palette::Orange;
					}
				}
			}
			//�܂���̂Ȃ��ڂ̓_��`��
			Circle(Vec2(toX(theta), toY(timeBegin)), divJointRadius).draw(c);
			Circle(Vec2(toX(theta + Quot(1, 1)), toY(timeBegin)), divJointRadius).draw(c);
			Circle(Vec2(toX(theta - Quot(1, 1)), toY(timeBegin)), divJointRadius).draw(c);
		}
	}
	//�I�����Ă��鏬�ߔ͈͂�`��
	if (measureEditIndex >= 0 && measureSelectUntilIndex >= 0) {
		Rect(leftMargin, toY(imosTime[measureSelectUntilIndex + 1]), editSpaceWidth, toY(imosTime[measureEditIndex]) - toY(imosTime[measureSelectUntilIndex + 1])).draw(Color(selectMeasureColor).setA(selectMeasureAlpha));
	}
	//���̏��ߏ��̕`��
	Rect({ 0, 0 }, { leftMargin, editSpaceHeight }).draw(Color(leftMarginColor));
	for (auto measure : step(measureNum)) {
		auto yBase = toY(imosTime[measure]);
		auto ascent = infoFont.ascent();
		Line(Vec2(0, yBase), Vec2(leftMargin, yBase)).draw(measureLineWidth, Color(infoColor));
		infoFont(U"Measure #" + ToString(measure)).draw(0, yBase - ascent - measureLineWidth);
		int yIndex = 1;
		for (auto info : measureAttribute[measure]) {
			yIndex++;
			infoFont(info.first + U": " + info.second).draw(0, yBase - ascent * yIndex - measureLineWidth);
		}
	}
	//���ʂ̍Ō�̐����`��
	Line(Vec2(0, toY(imosTime.back())), Vec2(leftMargin, toY(imosTime.back()))).draw(measureLineWidth, Color(infoColor));
	infoFont(U"End of chart").draw(0, toY(imosTime.back()) - infoFont.ascent() - measureLineWidth);
	//�^�u���j���[�̕`��
	if (isTabMenu) {
		Rect({ 0, 0 }, { leftMargin, editSpaceHeight }).draw(Color(tabMenuColor));
		//���[�h�������ɕ\������
		infoFont(isLaneMode ? U"LANE MODE" : U"DIV MODE").draw(0, editSpaceHeight - infoFont.ascent(), Color(modeTextColor));

	}
	if (!isTabMenu) {
		//�^�u���j���[���Ȃ��ꍇ�A���ߏ��ҏWGUI��ǉ�
		measureEditWindow.draw();
	}
}