
#include "Chart.hpp"
#include <set>

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
			previousPlayClock = currentPlayClock = startPlayClock;
			stopwatch.restart();
			//�R���{��0����
			comboCounter = 0;
			//�Ȃ��Đ��J�n
			backgroundMusic.play();
		}
		else {
			//�Ȃ��Đ��I��
			backgroundMusic.stop();
		}
		isPlaying = !isPlaying;
	}
}

void Chart::UpdatePlayer() {
	//���̃~���b���𔽉f����
	int duration = stopwatch.ms();
	previousPlayClock = currentPlayClock;
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
	//�^�u���j���[��̃e�L�X�g�{�b�N�X���Ǘ�����
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
			for (int index = 0; index < divNum; index++) {
				SimpleGUI::CheckBox(divEditIndex[index], Format(U"Div ", index), Vec2(20, 200 + 40 * index), 216);
			}

			int measureIndex = upper_bound(imosTime.begin(), imosTime.end(), divEditLineTime) - imosTime.begin() - 1;
			auto posFlag = [&](int lane) {return divPositionData[measureIndex][lane].count(divEditLineTime - imosTime[measureIndex]); };
			auto velFlag = [&](int lane) {return divVelocityData[measureIndex][lane].count(divEditLineTime - imosTime[measureIndex]); };
			auto posInfo = [&](int lane) {return divPositionData[measureIndex][lane][divEditLineTime - imosTime[measureIndex]]; };
			auto velSub = [&](int lane, Quot q) { divVelocityData[measureIndex][lane][divEditLineTime - imosTime[measureIndex]] = q; };
			auto posSub = [&](int lane, Quot q) { divPositionData[measureIndex][lane][divEditLineTime - imosTime[measureIndex]] = q; };
			auto velInfo = [&](int lane) {return divVelocityData[measureIndex][lane][divEditLineTime - imosTime[measureIndex]]; };
			auto posDelete = [&](int lane) {divPositionData[measureIndex][lane].erase(divEditLineTime - imosTime[measureIndex]); };
			auto velDelete = [&](int lane) {divVelocityData[measureIndex][lane].erase(divEditLineTime - imosTime[measureIndex]); };
			std::set<Quot> posMap, velMap;
			for (int lane = 0; lane < divNum; lane++) {
				if (divEditIndex[lane]) {
					if (posFlag(lane))posMap.insert(posInfo(lane));
					if (velFlag(lane))velMap.insert(velInfo(lane));
				}
			}
			String posStr, velStr;
			if (!tesPosition.active) {
				if (posMap.empty()) {
					posStr = tesPosition.text = U"";
				}
				else if (posMap.size() == 1) {
					posStr = tesPosition.text = posMap.begin()->parse();
				}
				else {
					posStr = tesPosition.text = U"...";
				}
			}
			if (!tesVelocity.active) {
				if (velMap.empty()) {
					velStr = tesVelocity.text = U"";
				}
				else if (velMap.size() == 1) {
					velStr = tesVelocity.text = velMap.begin()->parse();
				}
				else {
					velStr = tesVelocity.text = U"...";
				}
			}

			boxFont(U"Pos:").draw(Vec2(20, 100));
			boxFont(U"Vel:").draw(Vec2(20, 140));
			SimpleGUI::TextBox(tesPosition, Vec2(80, 100), 156);
			SimpleGUI::TextBox(tesVelocity, Vec2(80, 140), 156);

			if (!tesPosition.active) {
				if (tesPosition.text.empty()) {
					for (int lane = 0; lane < divNum; lane++) {
						if (divEditIndex[lane]) {
							if (posFlag(lane))posDelete(lane);
							recalculation = true;
						}
					}
				}
				else if (Quot::isParsable(tesPosition.text)) {
					for (int lane = 0; lane < divNum; lane++) {
						if (divEditIndex[lane]) {
							posSub(lane, Quot(tesPosition.text));
							recalculation = true;
						}
					}
				}
				else {
					tesPosition.text = posStr;
				}
			}
			else {
				isAllTextboxInactive = false;
			}

			if (!tesVelocity.active) {
				if (tesVelocity.text.empty()) {
					for (int lane = 0; lane < divNum; lane++) {
						if (divEditIndex[lane]) {
							if (velFlag(lane))velDelete(lane);
						}
					}
				}
				else if (Quot::isParsable(tesVelocity.text)) {
					for (int lane = 0; lane < divNum; lane++) {
						if (divEditIndex[lane]) {
							velSub(lane, Quot(tesVelocity.text));
							recalculation = true;
						}
					}
				}
				else {
					tesVelocity.text = velStr;
				}
			}
			else {
				isAllTextboxInactive = false;
			}

		}
	}
	else {
		//Tab���j���[���J���Ă��Ȃ��Ƃ��ɂ͏��ߕҏW�{�b�N�X���X�V����
		if (!measureEditWindow.isDeadFlag()) {
			if (measureEditWindow.isTextboxActive()) {
				isAllTextboxInactive = false;
			}
			else {
				//�e�L�X�g�{�b�N�X�ɓ��͒��łȂ��ꍇ�A�l�̎擾���s��
				bool hasValueChanged = false;
				auto& nowMeasure = measureAttribute[measureEditIndex];
				//barDisplay
				{
					String sBarDisplay = measureEditWindow.getValue(U"barDisplay");
					if (sBarDisplay.empty())nowMeasure.erase(U"barDisplay");
					else {
						auto tmp = ParseOpt<bool>(sBarDisplay);
						if (tmp != none) {
							hasValueChanged |= (nowMeasure[U"barDisplay"] != sBarDisplay);
							nowMeasure[U"barDisplay"] = sBarDisplay;
						}
						else {
							measureEditWindow.overrideValue(U"barDisplay",
								nowMeasure.count(U"barDisplay") ? nowMeasure[U"barDisplay"] : U"");
						}
					}
				}
				//bpm
				{
					String sBPM = measureEditWindow.getValue(U"bpm");
					if (sBPM.empty())nowMeasure.erase(U"bpm");
					else {
						std::function<s3d::Optional<double>(StringView)> ParseOptDouble = [](StringView sv)->Optional<double> {
							try {
								return ParseOpt<double>(sv);
							}
							catch (Error&) {
								return none;
							};
						};
						auto tmp = ParseOptDouble(sBPM);
						if (tmp != none && tmp.value() > 0) {
							hasValueChanged |= (nowMeasure[U"bpm"] != sBPM);
							nowMeasure[U"bpm"] = sBPM;
						}
						else {
							measureEditWindow.overrideValue(U"bpm",
								nowMeasure.count(U"bpm") ? nowMeasure[U"bpm"] : U"");
						}
					}
				}
				//innerHS
				{
					String sInnerHS = measureEditWindow.getValue(U"innerHS");
					if (sInnerHS.empty())nowMeasure.erase(U"innerHS");
					else {
						auto tmp = ParseOpt<int>(sInnerHS);
						if (tmp != none && tmp.value() >= 0) {
							hasValueChanged |= (nowMeasure[U"innerHS"] != sInnerHS);
							nowMeasure[U"innerHS"] = sInnerHS;
						}
						else {
							measureEditWindow.overrideValue(U"innerHS",
								nowMeasure.count(U"innerHS") ? nowMeasure[U"innerHS"] : U"");
						}
					}
				}
				//length
				{
					String sLength = measureEditWindow.getValue(U"length");
					if (sLength.empty())nowMeasure.erase(U"length");
					else {
						if (Quot::isParsable(sLength) && Quot(sLength) > Quot(0, 1)) {
							hasValueChanged |= (nowMeasure[U"length"] != sLength);
							nowMeasure[U"length"] = sLength;
						}
						else {
							measureEditWindow.overrideValue(U"length",
								nowMeasure.count(U"length") ? nowMeasure[U"length"] : U"");
						}
					}
				}
				//�l���ύX���ꂽ��Čv�Z����
				if (hasValueChanged) {
					recalculation = true;
				}
			}
		}
	}
	//�^�u���j���[���J����Ă���ꍇ�A���ߕҏW�{�b�N�X�͕���
	if (isTabMenu) {
		measureEditWindow.deadFlag(true);
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

	//�㉺�L�[�őI�𒆂̏��߂͈̔͂��㉺����
	if (measureEditIndex >= 0) {
		if (KeyUp.down()) {
			measureSelectUntilIndex = Clamp(measureSelectUntilIndex + 1, measureEditIndex, measureNum - 1);
		}
		if (KeyDown.down()) {
			measureSelectUntilIndex = Clamp(measureSelectUntilIndex - 1, measureEditIndex, measureNum - 1);
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
				if (MouseL.down()) {
					//��ԋ߂�����������
					auto itr = std::upper_bound(tickTimeReal.begin(), tickTimeReal.end(), mTime);
					auto jtr = std::prev(itr);
					Quot closestTime = tickTime[jtr - tickTimeReal.begin()];
					//�N���b�N���ꂽ���ߔԍ����v�Z����
					int measureIndex = std::upper_bound(imosTime.begin(), imosTime.end(), closestTime) - imosTime.begin() - 1;
					//�ҏW�E�B���h�E�����(�����ꏊ���ēx�N���b�N�����ꍇ�͖���)
					if (measureEditIndex != measureIndex) {
						measureEditWindow.deadFlag(false);
						measureEditIndex = measureIndex;
						measureSelectUntilIndex = measureIndex;
						measureEditWindow.initWindow();
						//�e�L�X�g�{�b�N�X���ɕҏW������̂�����
						//measureEditWindow.addLabeledBox(U"TestLabel");
						measureEditWindow.addLabeledBox(U"barDisplay");
						measureEditWindow.addLabeledBox(U"bpm");
						measureEditWindow.addLabeledBox(U"innerHS");
						measureEditWindow.addLabeledBox(U"length");
						auto& nowMeasure = measureAttribute[measureIndex];
						if (nowMeasure.count(U"barDisplay"))
							measureEditWindow.overrideValue(U"barDisplay", nowMeasure[U"barDisplay"]);
						if (nowMeasure.count(U"bpm"))
							measureEditWindow.overrideValue(U"bpm", nowMeasure[U"bpm"]);
						if (nowMeasure.count(U"innerHS"))
							measureEditWindow.overrideValue(U"innerHS", nowMeasure[U"innerHS"]);
						if (nowMeasure.count(U"length"))
							measureEditWindow.overrideValue(U"length", nowMeasure[U"length"]);
						measureEditWindow.addButton(buttonStringCopy);
						measureEditWindow.addButton(buttonStringCut);
						measureEditWindow.addButton(buttonStringPaste);
						measureEditWindow.addButton(buttonStringClear);
					}
				}
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
			int measureIndex = std::upper_bound(imosTime.begin(), imosTime.end(), closestTime) - imosTime.begin() - 1;
			//�����ڂ�
			Quot relativeTime = closestTime - imosTime[measureIndex];
			//�}�E�X�̈ʒu�����߂̐擪����ǂ̂��炢��
			auto distanceFromTop = mTime - imosTime[measureIndex].real();
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
							for (auto& display : laneDisplayData[measureIndex][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								closestObject = Min(dif, closestObject);
							}
							if (closestObject <= tickDistance) {
								//�I�u�W�F�N�g�̂ق����߂��̂ő������
								for (auto& display : laneDisplayData[measureIndex][lane]) {
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
							laneDisplayData[measureIndex][lane][relativeTime] = nowDispString;
							Logger.writeln(Format(U"Placed display ", nowDispString, U" in lane ", lane, U" measure ", measureIndex, U" time ", relativeTime.parse()));
						}
					}
					else {
						if (KeyControl.pressed()) {
							//Ctrl+�N���b�N�ň�ԋ߂��m�[�c���擾��nowNoteString�ɑ��
							//��ԋ߂��m�[�c���v�Z
							auto closestObject = tickDistance;
							for (auto& display : laneNoteData[measureIndex][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								closestObject = Min(dif, closestObject);
							}
							if (closestObject <= tickDistance) {
								//�I�u�W�F�N�g�̂ق����߂��̂ő������
								for (auto& display : laneNoteData[measureIndex][lane]) {
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
							laneNoteData[measureIndex][lane][relativeTime] = nowNoteString;
							Logger.writeln(Format(U"Placed note ", nowNoteString, U" in lane ", lane, U" measure ", measureIndex, U" time ", relativeTime.parse()));
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
						for (auto& display : laneDisplayData[measureIndex][lane]) {
							auto dif = std::abs(display.first.real() - distanceFromTop);
							closestObject = Min(dif, closestObject);
						}
						if (closestObject <= tickDistance) {
							//�I�u�W�F�N�g�̂ق����߂��̂ō폜����
							for (auto& display : laneDisplayData[measureIndex][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								if (std::abs(dif - closestObject) <= DBL_EPSILON) {
									Logger.writeln(Format(U"Erased display ", display.second, U" in lane ", lane, U" measure ", measureIndex, U" time ", display.first.parse()));
									laneDisplayData[measureIndex][lane].erase(display.first);
									break;
								}
							}
						}
					}
					else {
						//�N���b�N�Ńm�[�c���폜
						auto closestObject = tickDistance;
						for (auto& display : laneNoteData[measureIndex][lane]) {
							auto dif = std::abs(display.first.real() - distanceFromTop);
							closestObject = Min(dif, closestObject);
						}
						if (closestObject <= tickDistance) {
							//�I�u�W�F�N�g�̂ق����߂��̂ō폜����
							for (auto& display : laneNoteData[measureIndex][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								if (std::abs(dif - closestObject) <= DBL_EPSILON) {
									Logger.writeln(Format(U"Erased display ", display.second, U" in lane ", lane, U" measure ", measureIndex, U" time ", display.first.parse()));
									laneNoteData[measureIndex][lane].erase(display.first);
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


	//�X�N���[���ʒu�ɂ���ď��߃E�B���h�E�̈ʒu���X�V
	if (measureEditWindow.isDeadFlag()) {
		measureEditIndex = -1;
	}
	if (measureEditIndex >= 0) {
		measureEditWindow.changePosition({ 0.0, toY(Quot(imosTime[measureEditIndex])) });
		measureEditWindow.changeSize({ leftMargin, toY(imosTime[measureEditIndex]) - toY(imosTime[measureEditIndex + 1]) });
	}

	//���߃E�B���h�E���X�V
	measureEditWindow.update();

	//���߃E�B���h�E�̃{�^���Ɋւ��鑀��
	if (measureEditIndex >= 0) {
		//�}�E�X���͂ɂ��L�[�{�[�h���͂ɂ��Ή�����
		bool copyFlag = measureEditWindow.getButtonFlag(buttonStringCopy) || (isAllTextboxInactive && KeyControl.pressed() && KeyC.down());
		bool cutFlag = measureEditWindow.getButtonFlag(buttonStringCut) || (isAllTextboxInactive && KeyControl.pressed() && KeyX.down());
		bool pasteFlag = measureEditWindow.getButtonFlag(buttonStringPaste) || (isAllTextboxInactive && KeyControl.pressed() && KeyV.down());
		bool clearFlag = measureEditWindow.getButtonFlag(buttonStringClear) || (isAllTextboxInactive && KeyControl.pressed() && KeyDelete.down());

		//Copy�{�^���Ŕ͈͂��R�s�[����
		if (copyFlag || cutFlag) {
			//Clipboard������������
			copyboardMeasureAttribute.clear();
			copyboardLaneNoteData.clear();
			copyboardLaneDisplayData.clear();
			copyboardDivPositionData.clear();
			copyboardDivVelocityData.clear();
			//Clipboard�ɓ��e���ڂ�
			for (int i = measureEditIndex; i <= measureSelectUntilIndex; i++) {
				copyboardMeasureAttribute.push_back(measureAttribute[i]);
				copyboardLaneNoteData.push_back(laneNoteData[i]);
				copyboardLaneDisplayData.push_back(laneDisplayData[i]);
				copyboardDivPositionData.push_back(divPositionData[i]);
				copyboardDivVelocityData.push_back(divVelocityData[i]);
			}
			//�����Cut�{�^���Ŕ͈͂��폜����(�ŏ��̏��߂͍폜�ł��Ȃ�)
			if (cutFlag) {
				if (measureEditIndex != 0) {
					measureAttribute.erase(measureAttribute.begin() + measureEditIndex, measureAttribute.begin() + measureSelectUntilIndex + 1);
					laneNoteData.erase(laneNoteData.begin() + measureEditIndex, laneNoteData.begin() + measureSelectUntilIndex + 1);
					laneDisplayData.erase(laneDisplayData.begin() + measureEditIndex, laneDisplayData.begin() + measureSelectUntilIndex + 1);
					divPositionData.erase(divPositionData.begin() + measureEditIndex, divPositionData.begin() + measureSelectUntilIndex + 1);
					divVelocityData.erase(divVelocityData.begin() + measureEditIndex, divVelocityData.begin() + measureSelectUntilIndex + 1);
					//���ҏW�E�B���h�E�����
					measureEditWindow.deadFlag(true);
					measureEditIndex = -1;
					measureSelectUntilIndex = -1;
				}
			}
			//�Čv�Z����
			recalculation = true;
		}
		//Paste�{�^����Clipboard�̓��e��}������(�ŏ��̏��߂̑O�ɂ͑}���ł��Ȃ�)
		if (pasteFlag) {
			int index = measureEditIndex + (KeyShift.pressed() ? 1 : 0);
			if (index != 0) {
				measureAttribute.insert(measureAttribute.begin() + index, copyboardMeasureAttribute.begin(), copyboardMeasureAttribute.end());
				laneNoteData.insert(laneNoteData.begin() + index, copyboardLaneNoteData.begin(), copyboardLaneNoteData.end());
				laneDisplayData.insert(laneDisplayData.begin() + index, copyboardLaneDisplayData.begin(), copyboardLaneDisplayData.end());
				divPositionData.insert(divPositionData.begin() + index, copyboardDivPositionData.begin(), copyboardDivPositionData.end());
				divVelocityData.insert(divVelocityData.begin() + index, copyboardDivVelocityData.begin(), copyboardDivVelocityData.end());
			}
			//�Čv�Z����
			recalculation = true;
		}
		//Clear�{�^���ŏ��߂̓��e���폜����(measureAttribute�̂ݕύX�Ȃ�)
		if (clearFlag) {
			for (int i = measureEditIndex; i <= measureSelectUntilIndex; i++) {
				//Shift�ő���������
				if (KeyShift.pressed()) {
					for (auto& elm : laneDisplayData[i])elm.clear();
				}
				else {
					for (auto& elm : laneNoteData[i])elm.clear();
				}
				//for (auto& elm : divPositionData[i])elm.clear();
				//for (auto& elm : divVelocityData[i])elm.clear();
			}
			//�Čv�Z����
			recalculation = true;
		}
	}

	//�Čv�Z���K�v�������炷��
	if (recalculation) {
		calculate();
	}
}
