#pragma once

#include <Siv3D.hpp>

//SimpleGUIの集合体のクラス
//"string":[textbox]のようなものが複数個ある
//ボタンが複数個ある
//Closeボタンもある
class GUIWindow {
private:
	//テキストボックスのラベル
	Array<String>labelArray;
	HashTable<String, int>labelReverse;
	Array<TextEditState>tesArray;
	//ボタンの文字列
	Array<String>buttonArray;
	HashTable<String, int>buttonFlag;
	//左下位置
	Vec2 position;
	//ウィンドウの大きさ
	Vec2 size;
	//すでに閉じられたウィンドウか
	bool isDead;
	//テキストボックスがアクティブかどうか
	bool textboxActiveFlag;
	//ラベルのフォント
	const Font font = Font(15);
	//ウィンドウの背景色
	const Color windowColor = Palette::Gray;
public:
	GUIWindow(Point position = Point(0, 0)) :position(position) { isDead = true; textboxActiveFlag = false; }

	//ウィンドウの存在フラグを変える
	void deadFlag(bool isDead) {
		this->isDead = isDead;
	}

	//ウィンドウが閉じられているかを返す
	bool isDeadFlag() {
		return isDead;
	}

	//ウィンドウ上のすべてのラベルボックスとボタンを削除する
	void initWindow() {
		labelArray.clear();
		labelReverse.clear();
		tesArray.clear();
		buttonArray.clear();
		buttonFlag.clear();
	}

	//ウィンドウ上にラベルボックスを追加する
	void addLabeledBox(String label) {
		labelReverse[label] = labelArray.size();
		labelArray.push_back(label);
		tesArray.push_back(TextEditState());
	}

	//ラベルボックスに入力された値を取得する
	String getValue(String label) const {
		return tesArray[labelReverse.at(label)].text;
	}

	//ラベルボックスに値を上書きする
	void overrideValue(String label, String str) {
		tesArray[labelReverse.at(label)].text = str;
	}

	//ウィンドウ上にボタンを追加する
	void addButton(String label) {
		buttonArray.push_back(label);
	}

	//ボタンが押されたかを返す
	bool getButtonFlag(String label) const {
		return buttonFlag.at(label);
	}

	//描画する
	void draw() const {
		if (!isDead) {
			Rect(position.x, position.y - size.y, size.x, size.y).draw(windowColor);
			auto ascent = font.ascent();
			int yIndex = 0;
			for (auto info : labelArray) {
				yIndex++;
				font(info + U": ").draw(0, position.y - ascent * yIndex - 2);
			}
		}
	}

	//テキストボックスを更新する
	void update() {
		textboxActiveFlag = false;
		if (!isDead) {
			double magnifier = 0.5;
			const Transformer2D transformer(Mat3x2::Scale(magnifier, magnifier), Mat3x2::Scale(magnifier, magnifier));
			//×ボタンを押されたら閉じる
			if (SimpleGUI::Button(U"×", Vec2(position.x, position.y - size.y) / magnifier)) {
				isDead = true;
			}
			auto ascent = font.ascent();
			int yIndex = 0;
			for (auto info : labelArray) {
				int xPlus = font(info + U": ").region().w;
				SimpleGUI::TextBox(tesArray[yIndex], (position + Vec2(xPlus, -ascent * (yIndex + 1) - 2)) / magnifier, 150);
				if (tesArray[yIndex].active) {
					textboxActiveFlag = true;
				}
				yIndex++;
			}
			const int buttonHeight = 20;
			yIndex = 0;
			for (auto info : buttonArray) {
				yIndex++;
				bool isPushed = SimpleGUI::Button(info, (position + Vec2(0, -size.y + buttonHeight * yIndex)) / magnifier);
				buttonFlag[info] = isPushed;
			}
		}
	}

	//テキストボックスがアクティブかどうかを返す
	bool isTextboxActive() {
		return textboxActiveFlag;
	}

	//左下の位置を変更する
	void changePosition(Vec2 position) {
		this->position = position;
	}

	//ウィンドウの大きさを変更する
	void changeSize(Vec2 size) {
		this->size = size;
	}
};