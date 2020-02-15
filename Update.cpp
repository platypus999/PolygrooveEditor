
#include "Chart.hpp"

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
			currentPlayClock = startPlayClock;
			stopwatch.restart();
		}
		else {
			//再生終了
		}
		isPlaying = !isPlaying;
	}
}

void Chart::UpdatePlayer() {
	//今のミリ秒数を反映する
	int duration = stopwatch.ms();
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

		}
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
			int measureNum = std::upper_bound(imosTime.begin(), imosTime.end(), closestTime) - imosTime.begin() - 1;
			//何拍目か
			Quot relativeTime = closestTime - imosTime[measureNum];
			//マウスの位置が小節の先頭からどのくらいか
			auto distanceFromTop = mTime - imosTime[measureNum].real();
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
							for (auto& display : laneDisplayData[measureNum][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								closestObject = Min(dif, closestObject);
							}
							if (closestObject <= tickDistance) {
								//オブジェクトのほうが近いので代入する
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
							//Shift+クリックで装飾を配置
							laneDisplayData[measureNum][lane][relativeTime] = nowDispString;
							Logger.writeln(Format(U"Placed display ", nowDispString, U" in lane ", lane, U" measure ", measureNum, U" time ", relativeTime.parse()));
						}
					}
					else {
						if (KeyControl.pressed()) {
							//Ctrl+クリックで一番近いノーツを取得しnowNoteStringに代入
							//一番近いノーツを計算
							auto closestObject = tickDistance;
							for (auto& display : laneNoteData[measureNum][lane]) {
								auto dif = std::abs(display.first.real() - distanceFromTop);
								closestObject = Min(dif, closestObject);
							}
							if (closestObject <= tickDistance) {
								//オブジェクトのほうが近いので代入する
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
							//クリックでノーツを配置
							laneNoteData[measureNum][lane][relativeTime] = nowNoteString;
							Logger.writeln(Format(U"Placed note ", nowNoteString, U" in lane ", lane, U" measure ", measureNum, U" time ", relativeTime.parse()));
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
						for (auto& display : laneDisplayData[measureNum][lane]) {
							auto dif = std::abs(display.first.real() - distanceFromTop);
							closestObject = Min(dif, closestObject);
						}
						if (closestObject <= tickDistance) {
							//オブジェクトのほうが近いので削除する
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
						//クリックでノーツを削除
						auto closestObject = tickDistance;
						for (auto& display : laneNoteData[measureNum][lane]) {
							auto dif = std::abs(display.first.real() - distanceFromTop);
							closestObject = Min(dif, closestObject);
						}
						if (closestObject <= tickDistance) {
							//オブジェクトのほうが近いので削除する
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

	//再計算が必要だったらする
	if (recalculation) {
		calculate();
	}
}
