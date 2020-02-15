
#include "Chart.hpp"

void Chart::Update() {
	//�Đ�����Ă��Ȃ��Ƃ��́A��ʂ�\������
	if (!isPlaying) {
		UpdateEditor();
	}
	else {
		UpdatePlayer();
	}
	if (KeyControl.pressed() && KeyP.down()) {
		//Ctrl-P�ōĐ��J�n�E�I��
		if (!isPlaying) {
			//�Đ��J�n
			startPlayClock = timeToClock(scrollTime);
			if (KeyShift.pressed()) {
				//�擪����Đ��J�n
				startPlayClock = 0.;
			}
			currentPlayClock = startPlayClock;
			stopwatch.restart();
		}
		else {
			//�Đ��I��
		}
		isPlaying = !isPlaying;
	}
}

void Chart::UpdatePlayer() {
	//���̃~���b���𔽉f����
	int duration = stopwatch.ms();
	currentPlayClock = startPlayClock + duration;
	//���ʂ��Ō�܂ŕ\�����I�����猳�̉�ʂɖ߂�
	if (currentPlayClock >= imosClock.back().first) {
		isPlaying = false;
		return;
	}
}

void Chart::UpdateEditor() {
	//���ׂẴe�L�X�g�{�b�N�X����A�N�e�B�u���ǂ���
	bool isAllTextboxInactive = true;
	//�Čv�Z���K�v��
	bool recalculation = false;

	//�}�E�X�z�C�[���֌W
	if (KeyShift.pressed()) {
		//Shift+�z�C�[���ō��E�ɓ�����
		scrollPos += Quot(int(Mouse::Wheel())) * scrollShiftSpeed;
		scrollPos = scrollPos.loop();
	}
	else if (KeyControl.pressed()) {
		//Ctrl+�z�C�[���ŏk�ڔ��ς���
		pixelPerTime *= 1.0 + Mouse::Wheel() * pixelPerTimeSpeed;
		pixelPerTime = Clamp(pixelPerTime, minPixelPerTime, maxPixelPerTime);
	}
	else {
		//�z�C�[���ŏ㉺�ɓ�����
		scrollTime -= Quot(int(Mouse::Wheel())) * scrollSpeed * Quot(maxPixelPerTime / pixelPerTime, 1);
		scrollTime = Clamp(scrollTime, Quot(0, 1), imosTime.back());
	}

	//Tab�Ń^�u���j���[���J����
	if (KeyTab.down())isTabMenu = !isTabMenu;
	if (isTabMenu) {
		const Transformer2D transformer(Mat3x2::Scale(0.625, 0.625), Mat3x2::Scale(0.625, 0.625));
		boxFont(U"Interval:").draw(Vec2(20, 20));
		SimpleGUI::TextBox(tesGranularity, Vec2(140, 20), 100, 3);
		if (!tesGranularity.active) {
			auto opt = ParseOpt<int>(tesGranularity.text);
			if (granularity != opt.value_or(0)) {
				recalculation = true;
			}
			granularity = opt.value_or(0);
			granularity = Clamp(granularity, 1, maxGranularity);
			tesGranularity.text = Format(granularity);
		}
		else {
			isAllTextboxInactive = false;
		}
		if (isLaneMode) {
			//���[�����[�h�̎�
			//�p���b�g��`��
			SimpleGUI::TextBox(tesNowNoteString, Vec2(20, 100), 216);
			if (!tesNowNoteString.active) {
				String tmp = tesNowNoteString.text;
				if (nowNoteString != tmp) {
					if (noteColor.count(tmp)) {
						nowNoteString = tmp;
					}
					else {
						tesNowNoteString.text = nowNoteString;
					}
				}
			}
			else {
				isAllTextboxInactive = false;
			}
			SimpleGUI::TextBox(tesNowDispString, Vec2(20, 140), 216);
			if (!tesNowDispString.active) {
				String tmp = tesNowDispString.text;
				if (nowDispString != tmp) {
					if (isValidColorString(tmp)) {
						nowDispString = tmp;
					}
					else {
						tesNowDispString.text = nowDispString;
					}
				}
			}
			else {
				isAllTextboxInactive = false;
			}
		}
		else {
			//Div���[�h�̎�
			//�������̕ҏW����ɂ������`��

		}
	}

	//Space�L�[��Div Mode��Lane Mode��ύX����
	if (KeySpace.down()) {
		isLaneMode = !isLaneMode;
	}

	//�����L�[�Ŕ����̍ŏ��P�ʂ�ύX����
	if (isAllTextboxInactive) {
		for (const auto& numkey : numberShortcut) {
			if (numkey.first.down()) {
				granularity = numkey.second;
				tesGranularity.text = Format(granularity);
				recalculation = true;
			}
		}
	}

	//�}�E�X���͂�����
	auto mx = Cursor::Pos().x;
	auto my = Cursor::Pos().y;
	double mTime = rawFromY(my);
	//�S�[�X�g�m�[�c�̈ʒu��������
	ghostNoteLane = -1;
	if (mTime < 0 || imosTime.back().real() <= mTime) {
		//��ʂ̏㉺�O�̏ꍇ�͏��O
	}
	else {
		//��ʂ̍��E�ǂ��瑤�ɂ��邩
		if (mx < leftMargin) {
			//���̏����
			if (isTabMenu) {
				//�^�u���j���[��ł͉������Ȃ�
			}
			else {
				//���ߏ��

			}
		}
		else {
			//�G�f�B�^���
			//X���W����p�x���v�Z
			double theta = rawFromX(mx);
			//���[���ԍ����v�Z
			int lane = divNum - 1;
			for (int i = 0; i < divNum; ++i) {
				auto L = divPosReal(i, mTime);
				auto R = divPosReal((i + 1) % divNum, mTime);
				if (R < L)R += 1.0;
				if ((L <= theta && theta < R) || (L <= theta + 1.0 && theta + 1.0 < R)) {
					lane = i;
					break;
				}
			}
			//��ԋ߂�����������
			auto itr = std::upper_bound(tickTimeReal.begin(), tickTimeReal.end(), mTime);
			auto jtr = std::prev(itr);
			int closestTimeIndex = (itr != tickTimeReal.end() && std::abs(mTime - *itr) < std::abs(mTime - *jtr)) ? itr - tickTimeReal.begin() : jtr - tickTimeReal.begin();
			auto tickDistance = (itr != tickTimeReal.end()) ? Min(std::abs(mTime - *itr), std::abs(mTime - *jtr)) : std::abs(mTime - *jtr);
			Quot closestTime = tickTime[closestTimeIndex];
			//�����ߖڂ�
			int measureNum = std::upper_bound(imosTime.begin(), imosTime.end(), closestTime) - imosTime.begin() - 1;
			//�����ڂ�
			Quot relativeTime = closestTime - imosTime[measureNum];
			//�}�E�X�̈ʒu�����߂̐擪����ǂ̂��炢��
			auto distanceFromTop = mTime - imosTime[measureNum].real();
			//�S�[�X�g�m�[�c���X�V
			ghostNoteLane = lane;
			ghostNoteTime = closestTime;
			if (isLaneMode) {
				//�I�u�W�F�N�g�̕ҏW������
				//���N���b�N�ŃI�u�W�F�N�g�̔z�u������
				if (MouseL.down()) {
					if (KeyShift.pressed()) {
						if (KeyControl.pressed()) {
							//Ctrl+Shift+�N���b�N�ň�ԋ߂��������擾��nowNoteString�ɑ��
							//��ԋ߂��������v�Z
							auto closestObject = tickDistance;
							for (auto& display : laneDisplayData[measureNum][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								closestObject = Min(dif, closestObject);
							}
							if (closestObject <= tickDistance) {
								//�I�u�W�F�N�g�̂ق����߂��̂ő������
								for (auto& display : laneDisplayData[measureNum][lane]) {
									auto dif = std::abs(display.first.real() - distanceFromTop);
									if (std::abs(dif - closestObject) <= DBL_EPSILON) {
										nowDispString = display.second;
										tesNowDispString.text = display.second;
										break;
									}
								}
							}
						}
						else {
							//Shift+�N���b�N�ő�����z�u
							laneDisplayData[measureNum][lane][relativeTime] = nowDispString;
							Logger.writeln(Format(U"Placed display ", nowDispString, U" in lane ", lane, U" measure ", measureNum, U" time ", relativeTime.parse()));
						}
					}
					else {
						if (KeyControl.pressed()) {
							//Ctrl+�N���b�N�ň�ԋ߂��m�[�c���擾��nowNoteString�ɑ��
							//��ԋ߂��m�[�c���v�Z
							auto closestObject = tickDistance;
							for (auto& display : laneNoteData[measureNum][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								closestObject = Min(dif, closestObject);
							}
							if (closestObject <= tickDistance) {
								//�I�u�W�F�N�g�̂ق����߂��̂ő������
								for (auto& display : laneNoteData[measureNum][lane]) {
									auto dif = std::abs(display.first.real() - distanceFromTop);
									if (std::abs(dif - closestObject) <= DBL_EPSILON) {
										nowNoteString = display.second;
										tesNowNoteString.text = display.second;
										break;
									}
								}
							}
						}
						else {
							//�N���b�N�Ńm�[�c��z�u
							laneNoteData[measureNum][lane][relativeTime] = nowNoteString;
							Logger.writeln(Format(U"Placed note ", nowNoteString, U" in lane ", lane, U" measure ", measureNum, U" time ", relativeTime.parse()));
						}
					}
					//�m�[�c��ݒu�����̂ōČv�Z���K�v
					recalculation = true;
				}
				//�E�N���b�N�ŃI�u�W�F�N�g�̍폜������
				if (MouseR.down()) {
					if (KeyShift.pressed()) {
						//Shift+�N���b�N�ő������폜
						auto closestObject = tickDistance;
						for (auto& display : laneDisplayData[measureNum][lane]) {
							auto dif = std::abs(display.first.real() - distanceFromTop);
							closestObject = Min(dif, closestObject);
						}
						if (closestObject <= tickDistance) {
							//�I�u�W�F�N�g�̂ق����߂��̂ō폜����
							for (auto& display : laneDisplayData[measureNum][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								if (std::abs(dif - closestObject) <= DBL_EPSILON) {
									Logger.writeln(Format(U"Erased display ", display.second, U" in lane ", lane, U" measure ", measureNum, U" time ", display.first.parse()));
									laneDisplayData[measureNum][lane].erase(display.first);
									break;
								}
							}
						}
					}
					else {
						//�N���b�N�Ńm�[�c���폜
						auto closestObject = tickDistance;
						for (auto& display : laneNoteData[measureNum][lane]) {
							auto dif = std::abs(display.first.real() - distanceFromTop);
							closestObject = Min(dif, closestObject);
						}
						if (closestObject <= tickDistance) {
							//�I�u�W�F�N�g�̂ق����߂��̂ō폜����
							for (auto& display : laneNoteData[measureNum][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								if (std::abs(dif - closestObject) <= DBL_EPSILON) {
									Logger.writeln(Format(U"Erased display ", display.second, U" in lane ", lane, U" measure ", measureNum, U" time ", display.first.parse()));
									laneNoteData[measureNum][lane].erase(display.first);
									break;
								}
							}
						}
					}
					//�m�[�c���폜�����̂ōČv�Z���K�v
					recalculation = true;
				}
			}
			else {
				//�������̕ҏW������
				if (MouseL.down()) {
					//���N���b�N�ŕ������ҏW�����}�E�X�̏ꏊ�Ɉړ�
					divEditLineTime = ghostNoteTime;
					//��ԋ߂�������(���������肤��)��������
					Array<bool>nowClickFlag(divNum);
					double smallestDistance = 1.0;
					for (auto i : step(divNum)) {
						double nowDist = (Abs(theta - divPosReal(i, mTime)));
						nowDist = Min(nowDist, Abs(theta - divPosReal(i, mTime) + 1.0));
						nowDist = Min(nowDist, Abs(theta - divPosReal(i, mTime) - 1.0));
						smallestDistance = Min(smallestDistance, nowDist);
					}
					for (auto i : step(divNum)) {
						double nowDist = (Abs(theta - divPosReal(i, mTime)));
						nowDist = Min(nowDist, Abs(theta - divPosReal(i, mTime) + 1.0));
						nowDist = Min(nowDist, Abs(theta - divPosReal(i, mTime) - 1.0));
						if (smallestDistance == nowDist) {
							nowClickFlag[i] = true;
						}
					}
					if (KeyShift.pressed()) {
						//Shift+�N���b�N�őS�̂�I��
						for (auto i : step(divNum)) {
							divEditIndex[i] = true;
						}
					}
					else if (KeyControl.pressed()) {
						//Ctrl+�N���b�N�ŏW���I�I��
						for (auto i : step(divNum)) {
							divEditIndex[i] |= nowClickFlag[i];
						}
					}
					else {
						//�����̃N���b�N�͂�����I��
						for (auto i : step(divNum)) {
							divEditIndex[i] = nowClickFlag[i];
						}
					}
				}
			}
		}
	}

	//�Čv�Z���K�v�������炷��
	if (recalculation) {
		calculate();
	}
}
