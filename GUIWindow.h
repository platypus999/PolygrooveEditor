#pragma once

#include <Siv3D.hpp>

//SimpleGUI�̏W���̂̃N���X
//"string":[textbox]�̂悤�Ȃ��̂���������
//�{�^������������
//Close�{�^��������
class GUIWindow {
private:
	//�e�L�X�g�{�b�N�X�̃��x��
	Array<String>labelArray;
	HashTable<String, int>labelReverse;
	Array<TextEditState>tesArray;
	//�{�^���̕�����
	Array<String>buttonArray;
	HashTable<String, int>buttonFlag;
	//�����ʒu
	Vec2 position;
	//�E�B���h�E�̑傫��
	Vec2 size;
	//���łɕ���ꂽ�E�B���h�E��
	bool isDead;
	//�e�L�X�g�{�b�N�X���A�N�e�B�u���ǂ���
	bool textboxActiveFlag;
	//���x���̃t�H���g
	const Font font = Font(15);
	//�E�B���h�E�̔w�i�F
	const Color windowColor = Palette::Gray;
public:
	GUIWindow(Point position = Point(0, 0)) :position(position) { isDead = true; textboxActiveFlag = false; }

	//�E�B���h�E�̑��݃t���O��ς���
	void deadFlag(bool isDead) {
		this->isDead = isDead;
	}

	//�E�B���h�E�������Ă��邩��Ԃ�
	bool isDeadFlag() {
		return isDead;
	}

	//�E�B���h�E��̂��ׂẴ��x���{�b�N�X�ƃ{�^�����폜����
	void initWindow() {
		labelArray.clear();
		labelReverse.clear();
		tesArray.clear();
		buttonArray.clear();
		buttonFlag.clear();
	}

	//�E�B���h�E��Ƀ��x���{�b�N�X��ǉ�����
	void addLabeledBox(String label) {
		labelReverse[label] = labelArray.size();
		labelArray.push_back(label);
		tesArray.push_back(TextEditState());
	}

	//���x���{�b�N�X�ɓ��͂��ꂽ�l���擾����
	String getValue(String label) const {
		return tesArray[labelReverse.at(label)].text;
	}

	//���x���{�b�N�X�ɒl���㏑������
	void overrideValue(String label, String str) {
		tesArray[labelReverse.at(label)].text = str;
	}

	//�E�B���h�E��Ƀ{�^����ǉ�����
	void addButton(String label) {
		buttonArray.push_back(label);
	}

	//�{�^���������ꂽ����Ԃ�
	bool getButtonFlag(String label) const {
		return buttonFlag.at(label);
	}

	//�`�悷��
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

	//�e�L�X�g�{�b�N�X���X�V����
	void update() {
		textboxActiveFlag = false;
		if (!isDead) {
			double magnifier = 0.5;
			const Transformer2D transformer(Mat3x2::Scale(magnifier, magnifier), Mat3x2::Scale(magnifier, magnifier));
			//�~�{�^���������ꂽ�����
			if (SimpleGUI::Button(U"�~", Vec2(position.x, position.y - size.y) / magnifier)) {
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

	//�e�L�X�g�{�b�N�X���A�N�e�B�u���ǂ�����Ԃ�
	bool isTextboxActive() {
		return textboxActiveFlag;
	}

	//�����̈ʒu��ύX����
	void changePosition(Vec2 position) {
		this->position = position;
	}

	//�E�B���h�E�̑傫����ύX����
	void changeSize(Vec2 size) {
		this->size = size;
	}
};