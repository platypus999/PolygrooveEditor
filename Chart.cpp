
#include "Chart.hpp"

Chart::Chart()
{
	//一時変数をすべて初期化
	scrollTime = 0;
	scrollPos = (Quot(0, 1));
	pixelPerTime = (320);
	isLaneMode = (true);
	isTabMenu = (false);
	granularity = (8);
	nowDispString = (U"#990000");
	nowNoteString = (U"tap");
	ghostNoteLane = (-1);
	divEditLineTime = Quot(0, 1);
	ghostNoteTime = (Quot(0, 1));
	fileName = (U"");
	isPlaying = false;

	//ソフト起動直後のデータ
	measureNum = 10;
	divNum = 16;
	measureAttribute.resize(measureNum);
	laneNoteData.resize(measureNum, std::vector<std::map<Quot, String>>(divNum));
	laneDisplayData.resize(measureNum, std::vector<std::map<Quot, String>>(divNum));
	divPositionData.resize(measureNum, std::vector<std::map<Quot, Quot>>(divNum));
	divVelocityData.resize(measureNum, std::vector<std::map<Quot, Quot>>(divNum));
	divEditIndex = Array<bool>(divNum);
	measureAttribute[0][U"bpm"] = U"180";
	measureAttribute[0][U"barDisplay"] = U"true";
	measureAttribute[0][U"length"] = U"1/1";
	measureAttribute[0][U"innerHS"] = U"1";
	laneDisplayData[0][3][Quot(0, 1)] = U"#ffffff";
	laneDisplayData[0][7][Quot(0, 1)] = U"#ffffff";
	laneDisplayData[0][11][Quot(0, 1)] = U"#ffffff";
	laneDisplayData[0][15][Quot(0, 1)] = U"#ffffff";
	divPositionData[0][0][Quot(0, 1)] = Quot(0, 4);
	divPositionData[0][1][Quot(0, 1)] = Quot(0, 4);
	divPositionData[0][2][Quot(0, 1)] = Quot(0, 4);
	divPositionData[0][3][Quot(0, 1)] = Quot(0, 4);
	divPositionData[0][4][Quot(0, 1)] = Quot(1, 4);
	divPositionData[0][5][Quot(0, 1)] = Quot(1, 4);
	divPositionData[0][6][Quot(0, 1)] = Quot(1, 4);
	divPositionData[0][7][Quot(0, 1)] = Quot(1, 4);
	divPositionData[0][8][Quot(0, 1)] = Quot(2, 4);
	divPositionData[0][9][Quot(0, 1)] = Quot(2, 4);
	divPositionData[0][10][Quot(0, 1)] = Quot(2, 4);
	divPositionData[0][11][Quot(0, 1)] = Quot(2, 4);
	divPositionData[0][12][Quot(0, 1)] = Quot(3, 4);
	divPositionData[0][13][Quot(0, 1)] = Quot(3, 4);
	divPositionData[0][14][Quot(0, 1)] = Quot(3, 4);
	divPositionData[0][15][Quot(0, 1)] = Quot(3, 4);
	for (auto i : step(divNum))
		divVelocityData[0][i][Quot(0, 1)] = Quot(0, 1);
	//ソフト起動直後のテキストボックス
	tesGranularity.text = Format(granularity);
	//デバッグ用ノーツ
	laneNoteData[0][3][Quot(1, 2)] = U"tap";
	laneNoteData[0][7][Quot(5, 8)] = U"tap";
	laneNoteData[0][3][Quot(3, 4)] = U"tap";
	laneNoteData[0][3][Quot(7, 8)] = U"tap";
	laneNoteData[0][7][Quot(7, 8)] = U"tap";
	measureAttribute[1][U"length"] = U"1/2";
	measureAttribute[2][U"length"] = U"3/2";
	measureAttribute[3][U"length"] = U"1/2";
	measureAttribute[5][U"length"] = U"1/1";
	measureAttribute[4][U"barDisplay"] = U"false";
	laneNoteData[0][11][Quot(1, 2)] = U"holdBegin";
	laneNoteData[2][11][Quot(1, 2)] = U"holdEnd";
	laneNoteData[2][15][Quot(1, 4)] = U"holdBegin";
	laneNoteData[2][15][Quot(3, 4)] = U"holdEnd";
	laneNoteData[5][7][Quot(0, 8)] = U"tap";
	laneNoteData[5][7][Quot(3, 8)] = U"tap";
	laneNoteData[5][7][Quot(4, 8)] = U"tap";
	laneNoteData[5][7][Quot(5, 8)] = U"tap";
	laneNoteData[5][15][Quot(1, 8)] = U"holdBegin";
	laneNoteData[5][15][Quot(7, 8)] = U"holdEnd";
	for (auto i : step(divNum)) {
		divVelocityData[2][i][Quot(0, 1)] = Quot(1, 6);
		divVelocityData[2][i][Quot(1, 2)] = -Quot(1, 3);
		divVelocityData[3][i][Quot(0, 1)] = Quot(0, 1);
		divVelocityData[5][i][Quot(0, 1)] = Quot(i % 4, 16);
		divVelocityData[6][i][Quot(0, 1)] = Quot(0, 1);
	}
	for (auto i : step(divNum)) {
		divPositionData[6][i][Quot(0, 1)] = Quot(1, 16) * i;
		laneDisplayData[6][i][Quot(1, 2)] = Array{ U"#ffffff",U"#808080",U"#ffffff",U"#808080" }[i % 4];
	}

	calculate();
}