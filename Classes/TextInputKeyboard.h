#ifndef __TextInputKeyboard_H__
#define __TextInputKeyboard_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;


/**
@brief    TextInputKeyboard for retain prev, reset, next, main menu buttons.
*/
class TextInputKeyboard : public CCLayer
{

	enum enumTextInputKeyboard {
    TAG_KEYBOARD_LAYER = 1,
	TAG_INPUTTEXT_LAYER,
	TAG_KEY_BACKSPASE = 127,
	TAG_KEY_END = 500,
	TAG_KEY_SHIFT = 501,
	TAG_BEGIN_POSITION_KEYBOARDY = -400,
	TAG_BEGIN_POSITION_KEYBOARDX = 0,

}; 

public:
    TextInputKeyboard();

	void setTextKeyboard(const char * beginText, int charLimit);
	void setPositionTextKeyboard(CCPoint textPosition);
	void setColorKeyboard(ccColor3B	colorKeyboard);
	void setSoundStatus(bool soundOn);
	void setScaleKeyboard(float keyboardScale);
	void saveName();
	bool isKeyboadVisible();


protected:
	// ��������� ������� �� ����������
	void _onTouchKeyboardButton(CCObject* pSender, TouchEventType type);
	// ��������� ������� �� ������ ������
	void _onTouchText(CCObject* pSender, TouchEventType type);
	// ���������� ����� � ������
	bool _onTextInsert(int keyTag);
	// �������� ����� �� ������
	bool _onTextDelete();
	// �������� ����� ����� �������� ����
	void _callbackRemoveNodeWhenDidAction(CCNode * pNode);
	// �������� ����� ����� ��������� ����������
	void _callbackKeybordUpAction(CCNode * pNode);

	// �������� ����������
	void _openKeybord();
	// ������ ����������
	void _closeKeybord();

	void _setTouchShift();

	virtual ~TextInputKeyboard();

protected:
	LabelBMFont				*inputLabel_;
	LabelBMFont				*inputLabelDiscribe_;
	float					keyboardScale_;
	bool					isAction_;
	CCAction				*actionText_;
	//CCAction				*actionTextDiscribe_;
	unsigned int			charLimit_;       // the textfield max char limit
	CCPoint					textPosition_;
	bool					soundOn_;

	ccColor3B				colorKeyboard_;
	bool					touchShift_;

	bool					keyboadVisible_;


};
#endif    // __TextInputKeyboard_H__
