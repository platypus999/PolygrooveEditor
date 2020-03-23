
#include "Chart.hpp"
#include "GreatEffect.h"

void Chart::Show() const {
	ShowEditor();
	if (isPlaying) {
		ShowPlayer();
	}
}

void Chart::ShowPlayer() const {
	//今描画すべきレーンかを考える
	Array<bool>nowDisplayedLane(divNum);

	//バックの画像を描画する

	//中心の多角形を描画する

	//レーン装飾およびロングノーツを描画する
	for (auto lane : step(divNum)) {
		for (int i = 0; i < laneVector[lane].size(); ++i) {
			auto& background = laneVector[lane][i];
			Quot timeStart = background.first;
			Quot timeEnd = (i + 1 != laneVector[lane].size()) ? laneVector[lane][i + 1].first : imosTime.back();
			double leftPosNow = divPosReal(lane, clockToRealTime(currentPlayClock));
			double rightPosNow = divPosReal((lane + 1) % divNum, clockToRealTime(currentPlayClock));
			String colorLong = background.second.first;
			String colorDisp = background.second.second;
			//極座標に変換する
			double radiusLow = Max(timeToHSTime(timeStart).real() - realTimeToRealHSTime(clockToRealTime(currentPlayClock)), 0.0) * noteSpeed + polygonRadius;
			double radiusHigh = Max(timeToHSTime(timeEnd).real() - realTimeToRealHSTime(clockToRealTime(currentPlayClock)), 0.0) * noteSpeed + polygonRadius;
			double realThetaLeft = leftPosNow * Math::Pi * 2;
			double realThetaRight = rightPosNow * Math::Pi * 2;
			//今判定ラインを通過中のノーツか？
			bool currentDisplaying = (timeToClock(timeStart) <= currentPlayClock && currentPlayClock <= timeToClock(timeEnd));
			if (currentDisplaying) {
				nowDisplayedLane[lane] = true;
			}
			//実際に描画
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
	//ノーツを描画する
	for (auto measure : step(measureNum)) {
		for (auto lane : step(divNum)) {
			for (auto elm : laneNoteData[measure][lane]) {
				Quot time = imosTime[measure] + elm.first;
				//このフレームで消えたノーツはタップエフェクトを加える
				auto clockTime = timeToClock(time);
				if (previousPlayClock < clockTime && clockTime <= currentPlayClock) {
					double leftPos = divPosReal(lane, clockToRealTime(currentPlayClock));
					double rightPos = divPosReal((lane + 1) % divNum, clockToRealTime(currentPlayClock));
					//極座標に変換
					double radius = (timeToHSTime(time).real() - realTimeToRealHSTime(clockToRealTime(currentPlayClock))) * noteSpeed + polygonRadius;
					double realThetaLeft = leftPos * Math::Pi * 2;
					double realThetaRight = rightPos * Math::Pi * 2;
					//エフェクトを追加
					greatEffect.add<GreatEffect>((polToRec(radius, realThetaLeft) + polToRec(radius, realThetaRight)) / 2, U"PERFECT");
				}
				//すでに過ぎ去ったノーツ、まだ描画するには早いノーツの場合は描画をしない
				if (clockTime <= currentPlayClock || currentPlayClock + renderNoteMilliseconds <= clockTime)continue;

				if (noteColor.contains(elm.second)) {
					double leftPos = divPosReal(lane, clockToRealTime(currentPlayClock));
					double rightPos = divPosReal((lane + 1) % divNum, clockToRealTime(currentPlayClock));
					//極座標に変換
					double radius = (timeToHSTime(time).real() - realTimeToRealHSTime(clockToRealTime(currentPlayClock))) * noteSpeed + polygonRadius;
					double realThetaLeft = leftPos * Math::Pi * 2;
					double realThetaRight = rightPos * Math::Pi * 2;
					//実際に描画
					Quad(polToRec(radius - notePlayDelta, realThetaLeft),
						polToRec(radius - notePlayDelta, realThetaRight),
						polToRec(radius + notePlayDelta, realThetaRight),
						polToRec(radius + notePlayDelta, realThetaLeft)).draw(Color(noteColor.at(elm.second)));
				}
				else {
					//存在しない種類のノーツである
					//エラーのほうが良い？
				}
			}
		}
	}
	//小節線を描画する


	//エフェクトを描画
	greatEffect.update();

}

void Chart::ShowEditor() const {
	//各レーンの背景を描画
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
				//invalidなノーツなので描画できない
				continue;
			}
			auto KBL = leftPosStart.gauss();
			auto KBR = rightPosStart.gauss();
			auto KEL = leftPosEnd.gauss();
			auto KER = rightPosEnd.gauss();
			auto KL = Min({ KBL, KBR, KEL, KER }) - 1;
			auto KR = Max({ KBL, KBR, KEL, KER }) + 1;
			//画面からはみ出した分は複数回ずらして描画
			int renderCount = 0;
			for (auto K : Range(KL, KR + 1)) {
				if (background.second.second != noColor) {
					//装飾の描画をする
					Quad(Vec2(toX(leftPosStart - Quot(K)), toY(timeStart)),
						Vec2(toX(leftPosEnd - Quot(K)), toY(timeEnd)),
						Vec2(toX(rightPosEnd - Quot(K)), toY(timeEnd)),
						Vec2(toX(rightPosStart - Quot(K)), toY(timeStart))).draw(Color(background.second.second).setA(displayAlpha));
				}
				if (background.second.first != noColor) {
					//ロングノーツの描画をする
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
	//分割線を描画
	for (auto lane : step(divNum)) {
		for (int i = 0; i < splitVector[lane].size(); ++i) {
			Quot timeBegin = splitVector[lane][i].first;
			Quot theta = splitVector[lane][i].second.first;
			Quot omega = splitVector[lane][i].second.second;
			Quot timeEnd = (i + 1 != splitVector[lane].size()) ? splitVector[lane][i + 1].first : imosTime.back();
			Quot endTheta = (timeEnd - timeBegin) * omega + theta;
			//画面からはみ出した分は複数回ずらして描画
			auto kBegin = theta.gauss();
			auto kEnd = endTheta.gauss();
			auto KL = Min(kBegin, kEnd) - 1;
			auto KR = Max(kBegin, kEnd) + 1;
			int renderCount = 0;
			//(theta,timeBegin)から(endTheta,timeEnd)までの線を引く
			for (auto K : Range(KL, KR + 1)) {
				Line(Vec2(toX(theta - Quot(K, 1)), toY(timeBegin)), Vec2(toX(endTheta - Quot(K, 1)), toY(timeEnd))).draw(divLineWidth, Color(divLineColor));
				++renderCount;
				if (renderCount >= renderLimit) {
					break;
				}
			}
		}
	}
	//一拍(granularity依存)ごとに補助線を描画
	if (granularity <= renderLimit && isTabMenu) {
		for (auto tick : tickTime) {
			Line(Vec2(toX(Quot(-1, 1)), toY(tick)), Vec2(toX(Quot(1, 1)), toY(tick))).draw(tickLineWidth, Color(tickLineColor));
		}
	}
	//小節線を描画
	for (auto measure : step(measureNum)) {
		if (isDrawMeasureLine[measure]) {
			Line(Vec2(toX(Quot(-1, 1)), toY(imosTime[measure])), Vec2(toX(Quot(1, 1)), toY(imosTime[measure]))).draw(measureLineWidth, Color(measureLineColor));
		}
	}
	//Shiftを押している間絶対位置の線を描画
	if (KeyShift.pressed()) {
		Line(Vec2(toX(Quot(0, 1)), toY(Quot(0, 1))), Vec2(toX(Quot(0, 1)), toY(imosTime.back()))).draw(Color(absoluteLineColor));
	}
	//単発のノーツと装飾を描画
	for (auto measure : step(measureNum)) {
		for (auto lane : step(divNum)) {
			if (KeyLShift.pressed()) {
				//シフトが押されているときに装飾も表示する
				for (auto elm : laneDisplayData[measure][lane]) {
					Quot time = imosTime[measure] + elm.first;
					Quot leftPos = divPos(lane, time);
					Quot rightPos = divPos((lane + 1) % divNum, time);
					if (rightPos < leftPos) {
						rightPos += Quot(1, 1);
					}
					if (elm.second != noColor) {
						//その位置とそれぞれ左右に描画する
						Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(displayThickness, Color(elm.second));
						Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(displayThickness, Color(elm.second));
						Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(displayThickness, Color(elm.second));
						if (isTabMenu && isLaneMode) {
							//タブメニューが開かれていてかつLane modeの時かつShift押下時、装飾の色(#xxxxxx表記)を表示
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos) + toX(rightPos)) / 2, toY(time)), Color(elm.second));
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos - Quot(1, 1)) + toX(rightPos - Quot(1, 1))) / 2, toY(time)), Color(elm.second));
							editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos + Quot(1, 1)) + toX(rightPos + Quot(1, 1))) / 2, toY(time)), Color(elm.second));
						}
					}
					else {
						//noColorも描画する
						//その位置とそれぞれ左右に描画する
						Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(displayThickness, Color(blackString));
						Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(displayThickness, Color(blackString));
						Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(displayThickness, Color(blackString));
						if (isTabMenu && isLaneMode) {
							//タブメニューが開かれていてかつLane modeの時かつShift押下時、noColorを表示
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
					//その位置とそれぞれ左右に描画する
					Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(noteThickness, Color(noteColor.at(elm.second)));
					Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(noteThickness, Color(noteColor.at(elm.second)));
					Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(noteThickness, Color(noteColor.at(elm.second)));
					if (isTabMenu && isLaneMode) {
						//タブメニューが開かれていてかつLane modeの時、ノーツのIDを表示
						editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos) + toX(rightPos)) / 2, toY(time)), Color(noteColor.at(elm.second)));
						editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos - Quot(1, 1)) + toX(rightPos - Quot(1, 1))) / 2, toY(time)), Color(noteColor.at(elm.second)));
						editorFont(elm.second).drawBaseAt(Vec2((toX(leftPos + Quot(1, 1)) + toX(rightPos + Quot(1, 1))) / 2, toY(time)), Color(noteColor.at(elm.second)));
					}
				}
				else {
					//存在しない種類のノーツである
					//エラーのほうが良い？
				}
			}
		}
	}
	//ゴーストノーツを表示
	if (isLaneMode && ghostNoteLane >= 0) {
		Quot leftPos = divPos(ghostNoteLane, ghostNoteTime);
		Quot rightPos = divPos((ghostNoteLane + 1) % divNum, ghostNoteTime);
		if (rightPos < leftPos)rightPos += Quot(1, 1);
		Quot time = ghostNoteTime;
		//その位置とそれぞれ左右に描画する
		if (KeyLShift.pressed()) {
			//装飾
			Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(noteThickness, Color(nowDispString).setA(ghostAlpha));
			Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(noteThickness, Color(nowDispString).setA(ghostAlpha));
			Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(noteThickness, Color(nowDispString).setA(ghostAlpha));
		}
		else {
			//ノーツ
			Line(Vec2(toX(leftPos), toY(time)), Vec2(toX(rightPos), toY(time))).draw(noteThickness, Color(noteColor.at(nowNoteString)).setA(ghostAlpha));
			Line(Vec2(toX(leftPos - Quot(1, 1)), toY(time)), Vec2(toX(rightPos - Quot(1, 1)), toY(time))).draw(noteThickness, Color(noteColor.at(nowNoteString)).setA(ghostAlpha));
			Line(Vec2(toX(leftPos + Quot(1, 1)), toY(time)), Vec2(toX(rightPos + Quot(1, 1)), toY(time))).draw(noteThickness, Color(noteColor.at(nowNoteString)).setA(ghostAlpha));
		}
	}
	//分割線編集線を表示
	if (!isLaneMode) {
		Line(Vec2(toX(Quot(-1, 1)), toY(ghostNoteTime)), Vec2(toX(Quot(2, 1)), toY(ghostNoteTime))).draw(divEditLineWidth, Color(divEditLineColor).setA(ghostAlpha));
		Line(Vec2(toX(Quot(-1, 1)), toY(divEditLineTime)), Vec2(toX(Quot(2, 1)), toY(divEditLineTime))).draw(divEditLineWidth, Color(divEditLineColor));
	}
	//折れ線の節目を描画
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
			//折れ線のつなぎ目の点を描画
			Circle(Vec2(toX(theta), toY(timeBegin)), divJointRadius).draw(c);
			Circle(Vec2(toX(theta + Quot(1, 1)), toY(timeBegin)), divJointRadius).draw(c);
			Circle(Vec2(toX(theta - Quot(1, 1)), toY(timeBegin)), divJointRadius).draw(c);
		}
	}
	//選択している小節範囲を描画
	if (measureEditIndex >= 0 && measureSelectUntilIndex >= 0) {
		Rect(leftMargin, toY(imosTime[measureSelectUntilIndex + 1]), editSpaceWidth, toY(imosTime[measureEditIndex]) - toY(imosTime[measureSelectUntilIndex + 1])).draw(Color(selectMeasureColor).setA(selectMeasureAlpha));
	}
	//左の小節情報の描画
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
	//譜面の最後の線も描画
	Line(Vec2(0, toY(imosTime.back())), Vec2(leftMargin, toY(imosTime.back()))).draw(measureLineWidth, Color(infoColor));
	infoFont(U"End of chart").draw(0, toY(imosTime.back()) - infoFont.ascent() - measureLineWidth);
	//タブメニューの描画
	if (isTabMenu) {
		Rect({ 0, 0 }, { leftMargin, editSpaceHeight }).draw(Color(tabMenuColor));
		//モードを左下に表示する
		infoFont(isLaneMode ? U"LANE MODE" : U"DIV MODE").draw(0, editSpaceHeight - infoFont.ascent(), Color(modeTextColor));

	}
	if (!isTabMenu) {
		//タブメニューがない場合、小節情報編集GUIを追加
		measureEditWindow.draw();
	}
}