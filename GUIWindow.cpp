#pragma once

#include "Chart.hpp"
#include <utility>

//SimpleGUI�̏W���̂̃N���X
//"string":[textbox]�̂悤�Ȃ��̂���������
//Close�{�^��������
class GUIWindow {
private:
	Array<String>labelArray;
	HashTable<String, int>labelReverse;
	Array<TextEditState>tes;
	//�����ʒu
	Point position;
	//���łɕ���ꂽ�E�B���h�E��
	bool isDead;
	//���x���̃t�H���g
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