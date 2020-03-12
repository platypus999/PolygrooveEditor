#pragma once
#include <Siv3D.hpp>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include "Rational.hpp"
#include "JsonWriter.hpp"
#include "GUIWindow.h"

//譜面データのクラス
class Chart {
public:

	//----------------------------------------
	//定数
	//----------------------------------------
	//最大何小節までの譜面を作れるか
	static const int maxNumOfMeasures = 1024;
	//左のバーのピクセル数
	static const int leftMargin = 160;
	//編集画面の横のピクセル数
	static const int editSpaceWidth = 640;
	//編集画面の高さのピクセル数
	static const int editSpaceHeight = 600;
	//描画ループ数の限界
	static const int renderLimit = 100;
	//スクロールのスピード
	const Quot scrollSpeed = Quot(1, 32);
	//横軸のスクロールのスピード
	const Quot scrollShiftSpeed = Quot(1, 24);
	//最大の縦の縮尺比
	const int maxPixelPerTime = 1800;
	//最小の縦の縮尺比
	const int minPixelPerTime = 40;
	//縮尺比を変化させるスピード
	const double pixelPerTimeSpeed = 0.05;
	//最小何分音符を置けるかどうか
	const int maxGranularity = 256;
	//bpm*msの定数
	const int MillisecBPMProduct = 60 * 1000 * 4;
	//画面中央の多角形の中心からのピクセル数
	const double polygonRadius = 100;
	//一小節に動くノーツの速さ(ピクセル/小節)
	const double noteSpeed = 400;

	//数字キーの分割数ショートカット
	const std::map<Key, int> numberShortcut = {
		{Key1,4},
		{Key2,8},
		{Key3,16},
		{Key4,24},
		{Key5,32},
		{Key6,48}
	};

	//色が存在しないときのカラーコード
	const String noColor = U"false";
	//noColorは実際にエディタ上でどう表示されるかのカラーコード
	const String blackString = U"#000000";
	//ホールドノーツ開始の文字列
	const String longNoteString[2] = { U"holdBegin", U"holdEnd" };
	//ホールドノーツの途中の色
	const std::map<String, String> holdColor = {
		{longNoteString[0],U"#FFD700"},
		{longNoteString[1],noColor},
	};
	//スライドノーツの種類とスライド方向(角度)
	const std::pair<String, Quot> swipeNoteString[4] = {
		{U"Left", Quot(3,4)},
		{U"Right", Quot(1,4)},
		{U"Out", Quot(0,4)},
		{U"In", Quot(2,4)}
	};
	//種類ごとのノーツのエディタ表示色
	const std::map<String, String> noteColor = {
		{U"tap",U"#FFFFFF"},
		{longNoteString[0],U"#FFFF00"},
		{longNoteString[1],U"#FFFF00"},
		{U"swipeLeft",U"#00BFFF"},
		{U"swipeRight",U"#00BFFF"},
		{U"swipeOut",U"#00BFFF"},
		{U"swipeIn",U"#00BFFF"},
		{U"swipeTapLeft",U"#B0C4DE"},
		{U"swipeTapRight",U"#B0C4DE"},
		{U"swipeTapOut",U"#B0C4DE"},
		{U"swipeTapIn",U"#B0C4DE"},
	};
	//分割線の色
	const String divLineColor = U"#404040";
	//分割線の太さ
	const double divLineWidth = 2.0;
	//小節線の色
	const String measureLineColor = U"#C0C0C0";
	//小節線の太さ
	static inline const double measureLineWidth = 3.0;
	//一拍ごとの線の色
	const String tickLineColor = U"#C0C0C0";
	//一拍ごとの線の太さ
	const double tickLineWidth = 1.0;
	//分割線編集線の色
	const String divEditLineColor = U"#FF0000";
	//分割線編集線の太さ
	const double divEditLineWidth = 1.5;
	//分割線の折れ線のつなぎ目の円の半径
	const double divJointRadius = 8.0;
	//ノーツの分厚さ
	const double noteThickness = 5.0;
	//装飾の分厚さ
	const double displayThickness = 3.0;
	//装飾の透明度
	const uint32 displayAlpha = 127;
	//ロングノーツの透明度
	const uint32 longNoteAlpha = 255;
	//ゴーストノーツの透明度
	const uint32 ghostAlpha = 127;
	//左の小節情報の背景の色
	const String leftMarginColor = U"#000000";
	//左のタブメニューの背景の色
	const String tabMenuColor = U"#303030";
	//絶対位置のマーカーの線の色
	const String absoluteLineColor = U"#FF0000";
	//左の小節情報の描画色
	const String infoColor = U"#FFFFFF";
	//左の小節情報のフォント
	const Font infoFont = Font(15);
	//テキストボックスのフォント
	const Font boxFont = Font(25);
	//右のエディタ画面内に表示されるテキスト
	const Font editorFont = Font(10);
	//左下のモードの文字の色
	const String modeTextColor = U"#00FF00";
	//プレイ画面のノーツの太さ(+-delta)
	const double notePlayDelta = 5;
	//選択されている小節の色
	const String selectMeasureColor = U"#1E90FF";
	//選択されている小節の色の透明度
	const uint32 selectMeasureAlpha = 31;
	//小節ウィンドウのボタンの文字列
	const String buttonStringCopy = U"Copy";
	const String buttonStringCut = U"Cut";
	const String buttonStringPaste = U"Paste";
	const String buttonStringClear = U"Clear";


private:

	//----------------------------------------
	//変数
	//----------------------------------------

	//スクロールされた縦軸の量
	Quot scrollTime;
	//スクロールされた横軸の量
	Quot scrollPos;
	//縦の縮尺比
	int pixelPerTime;
	//lane modeかdiv modeか
	bool isLaneMode;
	//タブメニューを開いているかどうか
	bool isTabMenu;
	//最小単位が何分音符か
	int granularity;
	//テキストボックス
	TextEditState tesGranularity;
	TextEditState tesNowNoteString;
	TextEditState tesNowDispString;
	//小節情報のGUIウィンドウ
	GUIWindow measureEditWindow;
	//編集している小節情報の番号
	int measureEditIndex;
	//編集している小節情報の選択範囲にかぶる一番最後の番号
	int measureSelectUntilIndex;
	
	//クリックすると配置される装飾の文字列
	String nowDispString;
	//クリックすると配置されるノーツの文字列
	String nowNoteString;
	//ゴーストノーツの縦軸位置(時間)
	Quot ghostNoteTime;
	//ゴーストノーツのレーン番号
	int ghostNoteLane;
	//分割線の編集線の縦軸位置(時間)
	Quot divEditLineTime;
	//保存するファイルの名前(==パス)
	String fileName;
	//譜面を再生中かどうか
	bool isPlaying;
	//再生している場所
	double currentPlayClock;
	//再生開始した場所
	double startPlayClock;
	//ストップウォッチ
	Stopwatch stopwatch;
	//コピーした小節情報のデータ
	std::vector<std::map<String, String>>copyboardMeasureAttribute;
	std::vector<std::vector<std::map<Quot, String>>>copyboardLaneNoteData;
	std::vector<std::vector<std::map<Quot, String>>>copyboardLaneDisplayData;
	std::vector<std::vector<std::map<Quot, Quot>>>copyboardDivPositionData;
	std::vector<std::vector<std::map<Quot, Quot>>>copyboardDivVelocityData;

	//----------------------------------------
	//直接的なデータ
	//----------------------------------------

	//分割線の個数
	int divNum;

	//小節数
	int measureNum;

	//譜面全体に関するデータ
	std::map<String, String>musicAttribute;

	//一小節ごとに関するデータ
	//[小節番号]["ID"]="value"
	std::vector<std::map<String, String>>measureAttribute;

	//レーン上にあるノーツのデータ
	//[小節番号][何レーン目][何拍目]="value"
	std::vector<std::vector<std::map<Quot, String>>>laneNoteData;

	//レーン上にある装飾のデータ
	//[小節番号][何レーン目][何拍目]="value"
	std::vector<std::vector<std::map<Quot, String>>>laneDisplayData;

	//分割線の位置のデータ
	//[小節番号][何レーン目][何拍目]=位置
	std::vector<std::vector<std::map<Quot, Quot>>>divPositionData;

	//分割線の速度のデータ
	//[小節番号][何レーン目][何拍目]=速度
	std::vector<std::vector<std::map<Quot, Quot>>>divVelocityData;


	//----------------------------------------
	//間接的なデータ
	//----------------------------------------
	//各小節目の先頭時間
	std::vector<Quot>imosTime;
	//各小節目の内部ハイスピ込みの先頭時間
	std::vector<Quot>imosHSTime;
	//各小節のHSを格納した配列
	std::vector<Quot>HSArray;
	//各小節目の先頭時間(ミリ秒)とBPMのペア
	std::vector<std::pair<double, double>>imosClock;
	//各分割線の横軸位置 [何レーン目][意味のないindex] = {time,{pos,speed}}
	//timeでソート済み
	std::vector<std::vector<std::pair<Quot, std::pair<Quot, Quot>>>>splitVector;
	//各レーンの背景(ロングノーツ+装飾) [何レーン目][意味のないindex] = {time,{colorLong,colorDisp}}
	//timeでソート済み
	std::vector<std::vector<std::pair<Quot, std::pair<String, String>>>>laneVector;
	//各小節について小節線を描画するか
	std::vector<bool>isDrawMeasureLine;
	//ノーツや分割点などが置けるtimeを昇順にまとめた配列
	std::vector<Quot>tickTime;
	//ノーツや分割点などが置けるtimeを昇順にまとめた配列(double版)
	std::vector<double>tickTimeReal;

	//分割線のうち選択されているものはtrue、されていないものはfalse
	Array<bool> divEditIndex;


	//直接的なデータと分割数に関する配列をすべて初期化(引数のサイズに変更)
	void initAllDirectData(int size = 0, int secondSize = 0) {
		measureAttribute.clear();
		laneNoteData.clear();
		laneDisplayData.clear();
		divPositionData.clear();
		divVelocityData.clear();
		measureAttribute.resize(size);
		laneNoteData.resize(size, std::vector<std::map<Quot, String>>(secondSize));
		laneDisplayData.resize(size, std::vector<std::map<Quot, String>>(secondSize));
		divPositionData.resize(size, std::vector<std::map<Quot, Quot>>(secondSize));
		divVelocityData.resize(size, std::vector<std::map<Quot, Quot>>(secondSize));
	}

	//時間timeでの分割線indexの横軸位置
	Quot divPos(int index, Quot time) const {
		//二分探索で位置を割り出す
		auto itr = std::upper_bound(splitVector[index].begin(), splitVector[index].end(), std::pair<Quot, std::pair<Quot, Quot>>{ Quot(time), { Quot(2,1),Quot() } });
		if (itr != splitVector[index].begin()) {
			--itr;
		}
		return (itr->second.first + (time - itr->first) * itr->second.second).loop();
	}
	//時間timeでの分割線indexの横軸位置(double版)
	double divPosReal(int index, double time) const {
		//二分探索で位置を割り出す
		auto itr = std::upper_bound(splitVector[index].begin(), splitVector[index].end(),
			std::pair<double, std::pair<Quot, Quot>>{ (time), { Quot(2,1),Quot() } },
			[&](const std::pair<double, std::pair<Quot, Quot>>& val, const std::pair<Quot, std::pair<Quot, Quot>>& emp) {
				auto fps = std::pair<double, std::pair<Quot, Quot>>(emp.first.real(), emp.second);
				return val < fps;
			});
		if (itr != splitVector[index].begin()) {
			--itr;
		}
		auto tmp = (itr->second.first.real() + (time - itr->first.real()) * itr->second.second.real());
		return tmp - floor(tmp);
	}
	//時間timeでの分割線indexの横軸速度
	Quot divSpeed(int index, Quot time) const {
		//二分探索で位置を割り出す
		auto itr = std::upper_bound(splitVector[index].begin(), splitVector[index].end(), std::pair<Quot, std::pair<Quot, Quot>>{ Quot(time), { Quot(2,1),Quot() } });
		if (itr != splitVector[index].begin()) {
			--itr;
		}
		return itr->second.second;
	}

	//横軸(角度単位)をピクセルに変換
	inline double toX(Quot angle) const {
		return leftMargin + double(Quot(editSpaceWidth) * (angle + scrollPos));
	}
	//縦軸(時間単位)をピクセルに変換
	inline double toY(Quot time) const {
		return editSpaceHeight - double((time - scrollTime) * pixelPerTime);
	}
	//ピクセルのX成分から横軸(角度単位)を計算
	inline double rawFromX(double pixelX) {
		double solution = (pixelX - leftMargin) / double(editSpaceWidth) - scrollPos.real() + 1.0;
		return solution - floor(solution);
	}
	//ピクセルのY成分から縦軸(時間単位)を計算
	inline double rawFromY(double pixelY) const {
		return (editSpaceHeight - pixelY) / pixelPerTime + scrollTime.real();
	}

	//長さwidthの区間が0度以上180度未満の区間かをチェック
	inline bool isOk(Quot width) const {
		return width >= Quot(0, 1) && width < Quot(1, 2);
	}
	//文字列がnoColorもしくはvalidなカラーコード(#xxxxxx)かどうかをチェック
	inline bool isValidColorString(const String& str) const {
		if (str == noColor)return true;
		if (str.length() != 7)return false;
		for (int i = 1; i <= 6; ++i) {
			if (!(U'0' <= str[i] && str[i] <= U'9') && !(U'a' <= str[i] && str[i] <= U'f'))
				return false;
		}
		return str[0] == U'#';
	}

	//時間をミリ秒に直す
	inline double timeToClock(Quot time) const {
		//何小節目かを計算
		int measure = upper_bound(imosTime.begin(), imosTime.end(), time) - imosTime.begin() - 1;
		//小節先頭からのミリ秒数を計算
		double jikan = MillisecBPMProduct / imosClock[measure].second * (time - imosTime[measure]).real<double>();
		return imosClock[measure].first + jikan;
	}
	//ミリ秒を時間(ただしdouble型)に直す
	inline double clockToRealTime(double clock) const {
		int measure = upper_bound(imosClock.begin(), imosClock.end(), std::make_pair(clock, DBL_MAX)) - imosClock.begin() - 1;

		double shousetsu = (clock - imosClock[measure].first) / (MillisecBPMProduct / imosClock[measure].second);
		return imosTime[measure].real() + shousetsu;
	}
	//極座標をピクセルに直す(原点は画面の中心)
	inline Point polToRec(double radius, double theta) const {
		return Point((int)(radius * Math::Cos(theta)), (int)(radius * Math::Sin(theta))) + Window::ClientCenter();
	}
	//時間をHS込みの時間に変更する
	inline Quot timeToHSTime(Quot time) const {
		int index = upper_bound(imosTime.begin(), imosTime.end(), time) - imosTime.begin() - 1;
		if (index == measureNum)return imosHSTime.back();
		return imosHSTime[index] - (imosTime[index] - time) * HSArray[index];
	}
	//時間をHS込みの時間に変更する(double版)
	inline double realTimeToRealHSTime(double time) const {
		int index = upper_bound(imosTime.begin(), imosTime.end(), time, [&](const double& d,const Quot& q) {return d < q.real(); }) - imosTime.begin() - 1;
		if (index == measureNum)return imosHSTime.back().real();
		return imosHSTime[index].real() - (imosTime[index].real() - time) * HSArray[index].real();
	}
public:
	//コンストラクタ
	Chart();

	//データから必要な情報をParseする
	void calculate();

	//画面を表示する
	void Show() const;

	//エディタ画面を表示する
	void ShowEditor() const;

	//再生画面を表示する
	void ShowPlayer() const;

	//処理を行う
	void Update();

	//エディター画面の処理を行う(再生中は呼ばない)
	void UpdateEditor();

	//再生画面の処理を行う
	void UpdatePlayer();

	//ファイルに保存する
	void SaveToFile();

	//ファイルを開く
	void ReadFromFile();
};

