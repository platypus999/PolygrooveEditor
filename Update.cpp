
#include "Chart.hpp"
#include <set>

void Chart::Update() {
	//再生されていないときは、画面を表示する
	if (!isPlaying) {
		UpdateEditor();
	}
	else {
		UpdatePlayer();
	}
	if (KeyControl.pressed() && KeyP.down()) {
		//Ctrl-Pで再生開始・終了
		if (!isPlaying) {
			//再生開始
			startPlayClock = timeToClock(scrollTime);
			if (KeyShift.pressed()) {
				//先頭から再生開始
				startPlayClock = 0.;
			}
			previousPlayClock = currentPlayClock = startPlayClock;
			stopwatch.restart();
			//コンボは0から
			comboCounter = 0;
			//曲を再生開始
			backgroundMusic.play();
		}
		else {
			//曲を再生終了
			backgroundMusic.stop();
		}
		isPlaying = !isPlaying;
	}
}

void Chart::UpdatePlayer() {
	//今のミリ秒数を反映する
	int duration = stopwatch.ms();
	previousPlayClock = currentPlayClock;
	currentPlayClock = startPlayClock + duration;
	//譜面を最後まで表示し終えたら元の画面に戻る
	if (currentPlayClock >= imosClock.back().first) {
		isPlaying = false;
		return;
	}
}

void Chart::UpdateEditor() {
	//すべてのテキストボックスが非アクティブかどうか
	bool isAllTextboxInactive = true;
	//再計算が必要か
	bool recalculation = false;

	//マウスホイール関係
	if (KeyShift.pressed()) {
		//Shift+ホイールで左右に動かす
		scrollPos += Quot(int(Mouse::Wheel())) * scrollShiftSpeed;
		scrollPos = scrollPos.loop();
	}
	else if (KeyControl.pressed()) {
		//Ctrl+ホイールで縮尺比を変える
		pixelPerTime *= 1.0 + Mouse::Wheel() * pixelPerTimeSpeed;
		pixelPerTime = Clamp(pixelPerTime, minPixelPerTime, maxPixelPerTime);
	}
	else {
		//ホイールで上下に動かす
		scrollTime -= Quot(int(Mouse::Wheel())) * scrollSpeed * Quot(maxPixelPerTime / pixelPerTime, 1);
		scrollTime = Clamp(scrollTime, Quot(0, 1), imosTime.back());
	}


	//Tabでタブメニューを開閉する
	if (KeyTab.down())isTabMenu = !isTabMenu;
	//タブメニュー上のテキストボックスを管理する
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
			//レーンモードの時
			//パレットを描画
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
			//Divモードの時
			//分割線の編集線上にある情報を描画
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
		//Tabメニューが開いていないときには小節編集ボックスを更新する
		if (!measureEditWindow.isDeadFlag()) {
			if (measureEditWindow.isTextboxActive()) {
				isAllTextboxInactive = false;
			}
			else {
				//テキストボックスに入力中でない場合、値の取得を行う
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
				//値が変更されたら再計算する
				if (hasValueChanged) {
					recalculation = true;
				}
			}
		}
	}
	//タブメニューが開かれている場合、小節編集ボックスは閉じる
	if (isTabMenu) {
		measureEditWindow.deadFlag(true);
	}


	//SpaceキーでDiv ModeとLane Modeを変更する
	if (KeySpace.down()) {
		isLaneMode = !isLaneMode;
	}

	//数字キーで拍数の最小単位を変更する
	if (isAllTextboxInactive) {
		for (const auto& numkey : numberShortcut) {
			if (numkey.first.down()) {
				granularity = numkey.second;
				tesGranularity.text = Format(granularity);
				recalculation = true;
			}
		}
	}

	//上下キーで選択中の小節の範囲を上下する
	if (measureEditIndex >= 0) {
		if (KeyUp.down()) {
			measureSelectUntilIndex = Clamp(measureSelectUntilIndex + 1, measureEditIndex, measureNum - 1);
		}
		if (KeyDown.down()) {
			measureSelectUntilIndex = Clamp(measureSelectUntilIndex - 1, measureEditIndex, measureNum - 1);
		}
	}



	//マウス入力を見る
	auto mx = Cursor::Pos().x;
	auto my = Cursor::Pos().y;
	double mTime = rawFromY(my);
	//ゴーストノーツの位置を初期化
	ghostNoteLane = -1;
	if (mTime < 0 || imosTime.back().real() <= mTime) {
		//画面の上下外の場合は除外
	}
	else {
		//画面の左右どちら側にあるか
		if (mx < leftMargin) {
			//左の情報画面
			if (isTabMenu) {
				//タブメニュー上では何もしない
			}
			else {
				//小節情報
				if (MouseL.down()) {
					//一番近い拍線を見る
					auto itr = std::upper_bound(tickTimeReal.begin(), tickTimeReal.end(), mTime);
					auto jtr = std::prev(itr);
					Quot closestTime = tickTime[jtr - tickTimeReal.begin()];
					//クリックされた小節番号を計算する
					int measureIndex = std::upper_bound(imosTime.begin(), imosTime.end(), closestTime) - imosTime.begin() - 1;
					//編集ウィンドウを作る(同じ場所を再度クリックした場合は無視)
					if (measureEditIndex != measureIndex) {
						measureEditWindow.deadFlag(false);
						measureEditIndex = measureIndex;
						measureSelectUntilIndex = measureIndex;
						measureEditWindow.initWindow();
						//テキストボックス内に編集するものを入れる
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
			//エディタ画面
			//X座標から角度を計算
			double theta = rawFromX(mx);
			//レーン番号を計算
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
			//一番近い拍線を見る
			auto itr = std::upper_bound(tickTimeReal.begin(), tickTimeReal.end(), mTime);
			auto jtr = std::prev(itr);
			int closestTimeIndex = (itr != tickTimeReal.end() && std::abs(mTime - *itr) < std::abs(mTime - *jtr)) ? itr - tickTimeReal.begin() : jtr - tickTimeReal.begin();
			auto tickDistance = (itr != tickTimeReal.end()) ? Min(std::abs(mTime - *itr), std::abs(mTime - *jtr)) : std::abs(mTime - *jtr);
			Quot closestTime = tickTime[closestTimeIndex];
			//何小節目か
			int measureIndex = std::upper_bound(imosTime.begin(), imosTime.end(), closestTime) - imosTime.begin() - 1;
			//何拍目か
			Quot relativeTime = closestTime - imosTime[measureIndex];
			//マウスの位置が小節の先頭からどのくらいか
			auto distanceFromTop = mTime - imosTime[measureIndex].real();
			//ゴーストノーツを更新
			ghostNoteLane = lane;
			ghostNoteTime = closestTime;
			if (isLaneMode) {
				//オブジェクトの編集をする
				//左クリックでオブジェクトの配置をする
				if (MouseL.down()) {
					if (KeyShift.pressed()) {
						if (KeyControl.pressed()) {
							//Ctrl+Shift+クリックで一番近い装飾を取得しnowNoteStringに代入
							//一番近い装飾を計算
							auto closestObject = tickDistance;
							for (auto& display : laneDisplayData[measureIndex][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								closestObject = Min(dif, closestObject);
							}
							if (closestObject <= tickDistance) {
								//オブジェクトのほうが近いので代入する
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
							//Shift+クリックで装飾を配置
							laneDisplayData[measureIndex][lane][relativeTime] = nowDispString;
							Logger.writeln(Format(U"Placed display ", nowDispString, U" in lane ", lane, U" measure ", measureIndex, U" time ", relativeTime.parse()));
						}
					}
					else {
						if (KeyControl.pressed()) {
							//Ctrl+クリックで一番近いノーツを取得しnowNoteStringに代入
							//一番近いノーツを計算
							auto closestObject = tickDistance;
							for (auto& display : laneNoteData[measureIndex][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								closestObject = Min(dif, closestObject);
							}
							if (closestObject <= tickDistance) {
								//オブジェクトのほうが近いので代入する
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
							//クリックでノーツを配置
							laneNoteData[measureIndex][lane][relativeTime] = nowNoteString;
							Logger.writeln(Format(U"Placed note ", nowNoteString, U" in lane ", lane, U" measure ", measureIndex, U" time ", relativeTime.parse()));
						}
					}
					//ノーツを設置したので再計算が必要
					recalculation = true;
				}
				//右クリックでオブジェクトの削除をする
				if (MouseR.down()) {
					if (KeyShift.pressed()) {
						//Shift+クリックで装飾を削除
						auto closestObject = tickDistance;
						for (auto& display : laneDisplayData[measureIndex][lane]) {
							auto dif = std::abs(display.first.real() - distanceFromTop);
							closestObject = Min(dif, closestObject);
						}
						if (closestObject <= tickDistance) {
							//オブジェクトのほうが近いので削除する
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
						//クリックでノーツを削除
						auto closestObject = tickDistance;
						for (auto& display : laneNoteData[measureIndex][lane]) {
							auto dif = std::abs(display.first.real() - distanceFromTop);
							closestObject = Min(dif, closestObject);
						}
						if (closestObject <= tickDistance) {
							//オブジェクトのほうが近いので削除する
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
					//ノーツを削除したので再計算が必要
					recalculation = true;
				}
			}
			else {
				//分割線の編集をする
				if (MouseL.down()) {
					//左クリックで分割線編集線をマウスの場所に移動
					divEditLineTime = ghostNoteTime;
					//一番近い分割線(複数個もありうる)を見つける
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
						//Shift+クリックで全体を選択
						for (auto i : step(divNum)) {
							divEditIndex[i] = true;
						}
					}
					else if (KeyControl.pressed()) {
						//Ctrl+クリックで集合的選択
						for (auto i : step(divNum)) {
							divEditIndex[i] |= nowClickFlag[i];
						}
					}
					else {
						//ただのクリックはそこを選択
						for (auto i : step(divNum)) {
							divEditIndex[i] = nowClickFlag[i];
						}
					}
				}
			}
		}
	}


	//スクロール位置によって小節ウィンドウの位置を更新
	if (measureEditWindow.isDeadFlag()) {
		measureEditIndex = -1;
	}
	if (measureEditIndex >= 0) {
		measureEditWindow.changePosition({ 0.0, toY(Quot(imosTime[measureEditIndex])) });
		measureEditWindow.changeSize({ leftMargin, toY(imosTime[measureEditIndex]) - toY(imosTime[measureEditIndex + 1]) });
	}

	//小節ウィンドウを更新
	measureEditWindow.update();

	//小節ウィンドウのボタンに関する操作
	if (measureEditIndex >= 0) {
		//マウス入力にもキーボード入力にも対応する
		bool copyFlag = measureEditWindow.getButtonFlag(buttonStringCopy) || (isAllTextboxInactive && KeyControl.pressed() && KeyC.down());
		bool cutFlag = measureEditWindow.getButtonFlag(buttonStringCut) || (isAllTextboxInactive && KeyControl.pressed() && KeyX.down());
		bool pasteFlag = measureEditWindow.getButtonFlag(buttonStringPaste) || (isAllTextboxInactive && KeyControl.pressed() && KeyV.down());
		bool clearFlag = measureEditWindow.getButtonFlag(buttonStringClear) || (isAllTextboxInactive && KeyControl.pressed() && KeyDelete.down());

		//Copyボタンで範囲をコピーする
		if (copyFlag || cutFlag) {
			//Clipboardを初期化する
			copyboardMeasureAttribute.clear();
			copyboardLaneNoteData.clear();
			copyboardLaneDisplayData.clear();
			copyboardDivPositionData.clear();
			copyboardDivVelocityData.clear();
			//Clipboardに内容を移す
			for (int i = measureEditIndex; i <= measureSelectUntilIndex; i++) {
				copyboardMeasureAttribute.push_back(measureAttribute[i]);
				copyboardLaneNoteData.push_back(laneNoteData[i]);
				copyboardLaneDisplayData.push_back(laneDisplayData[i]);
				copyboardDivPositionData.push_back(divPositionData[i]);
				copyboardDivVelocityData.push_back(divVelocityData[i]);
			}
			//さらにCutボタンで範囲を削除する(最初の小節は削除できない)
			if (cutFlag) {
				if (measureEditIndex != 0) {
					measureAttribute.erase(measureAttribute.begin() + measureEditIndex, measureAttribute.begin() + measureSelectUntilIndex + 1);
					laneNoteData.erase(laneNoteData.begin() + measureEditIndex, laneNoteData.begin() + measureSelectUntilIndex + 1);
					laneDisplayData.erase(laneDisplayData.begin() + measureEditIndex, laneDisplayData.begin() + measureSelectUntilIndex + 1);
					divPositionData.erase(divPositionData.begin() + measureEditIndex, divPositionData.begin() + measureSelectUntilIndex + 1);
					divVelocityData.erase(divVelocityData.begin() + measureEditIndex, divVelocityData.begin() + measureSelectUntilIndex + 1);
					//情報編集ウィンドウを閉じる
					measureEditWindow.deadFlag(true);
					measureEditIndex = -1;
					measureSelectUntilIndex = -1;
				}
			}
			//再計算する
			recalculation = true;
		}
		//PasteボタンでClipboardの内容を挿入する(最初の小節の前には挿入できない)
		if (pasteFlag) {
			int index = measureEditIndex + (KeyShift.pressed() ? 1 : 0);
			if (index != 0) {
				measureAttribute.insert(measureAttribute.begin() + index, copyboardMeasureAttribute.begin(), copyboardMeasureAttribute.end());
				laneNoteData.insert(laneNoteData.begin() + index, copyboardLaneNoteData.begin(), copyboardLaneNoteData.end());
				laneDisplayData.insert(laneDisplayData.begin() + index, copyboardLaneDisplayData.begin(), copyboardLaneDisplayData.end());
				divPositionData.insert(divPositionData.begin() + index, copyboardDivPositionData.begin(), copyboardDivPositionData.end());
				divVelocityData.insert(divVelocityData.begin() + index, copyboardDivVelocityData.begin(), copyboardDivVelocityData.end());
			}
			//再計算する
			recalculation = true;
		}
		//Clearボタンで小節の内容を削除する(measureAttributeのみ変更なし)
		if (clearFlag) {
			for (int i = measureEditIndex; i <= measureSelectUntilIndex; i++) {
				//Shiftで装飾を消す
				if (KeyShift.pressed()) {
					for (auto& elm : laneDisplayData[i])elm.clear();
				}
				else {
					for (auto& elm : laneNoteData[i])elm.clear();
				}
				//for (auto& elm : divPositionData[i])elm.clear();
				//for (auto& elm : divVelocityData[i])elm.clear();
			}
			//再計算する
			recalculation = true;
		}
	}

	//再計算が必要だったらする
	if (recalculation) {
		calculate();
	}
}
