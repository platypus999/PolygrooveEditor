#pragma once

#include "Chart.hpp"
#include <utility>

//SimpleGUIの集合体のクラス
//"string":[textbox]のようなものが複数個ある
//Closeボタンもある
class GUIWindow {
private:
	Array<String>labelArray;
	HashTable<String, int>labelReverse;
	Array<TextEditState>tes;
	//左下位置
	Point position;
	//すでに閉じられたウィンドウか
	bool isDead;
	//ラベルのフォント
	const Font font = Font(10);
public:
	GUIWindow(Point position) :position(position) {}

	void addLabeledBox(String label) {
		labelReverse[label] = labelArray.size();
		labelArray.push_back(label);
		tes.push_back(TextEditState());
	}

	String getValue(String label) const {
		return tes[labelReverse.at(label)].text;
	}

	void Update() {
		if (!isDead) {
			auto ascent = font.ascent();
			int yIndex = 0;
			for (auto info : labelArray) {
				yIndex++;
				font(info + U": ").draw(0, position.y - ascent * yIndex - Chart::measureLineWidth);
			}
		}
	}

	void Update(Point position) {
		this->position = position;
		Update();
	}
};