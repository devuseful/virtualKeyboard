#include "TextInputKeyboard.h"

//////////////////////////////////////////////////////////////////////////
// local function
//////////////////////////////////////////////////////////////////////////

TextInputKeyboard::TextInputKeyboard() {

	CCLog("TextInputKeyboard()");
	charLimit_ = 20;
	isAction_ = false;
	keyboardScale_ = 1.0f;
	colorKeyboard_ = ccBLACK;
	soundOn_ = true;
	touchShift_ = false;
	keyboadVisible_ = false;

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	textPosition_ = ccp(visibleSize.width/2, visibleSize.height/2);
	
	// загрузить виджет с клавиатурой
	UIWidget* widget = dynamic_cast<Layout*>(GUIReader::shareReader()->widgetFromJsonFile("ui/Keyboard/Keyboard.ExportJson"));

	// создать слой для клавиатуры
	UILayer* keyboardLayer = UILayer::create();
	keyboardLayer->setPosition(ccp(TAG_BEGIN_POSITION_KEYBOARDX, TAG_BEGIN_POSITION_KEYBOARDY));
	keyboardLayer->setVisible(false);
	// Добавить в него виджет клавиатуры
	keyboardLayer->addWidget(widget);
	this->addChild(keyboardLayer, TAG_KEYBOARD_LAYER, TAG_KEYBOARD_LAYER);

	// задать цвет кнопок
	this->setColorKeyboard(colorKeyboard_);

	// создать слой для ввода текста
	UILayer* textLayer = UILayer::create();
	// создать виджет для ввода текста
	UIWidget* widget2 = UIWidget::create();

	// добавить строку теста
	inputLabel_ = LabelBMFont::create();
	inputLabel_->setFntFile("fonts/play_100_0.fnt");
	inputLabel_->setScale(0.5f);
	//inputLabel_->setPosition(textPosition_);
	inputLabel_->setText("1");
	inputLabel_->setTouchEnabled(true);
	inputLabel_->addTouchEventListener(this, toucheventselector(TextInputKeyboard::_onTouchText));
	// добавить надпись в виджет
	widget2->addChild(inputLabel_);
	// добавить виджет в слой
	textLayer->addWidget(widget2);
	this->addChild(textLayer, TAG_INPUTTEXT_LAYER, TAG_INPUTTEXT_LAYER);

	inputLabelDiscribe_ = LabelBMFont::create();
	inputLabelDiscribe_->setFntFile("fonts/play_100_0.fnt");
	//inputLabelDiscribe_->setPosition(ccp(textPosition_.x - 20, textPosition_.y));
	inputLabelDiscribe_->setText("click to change");
	inputLabelDiscribe_->setScale(0.3f);
	inputLabelDiscribe_->setTouchEnabled(true);
	inputLabelDiscribe_->addTouchEventListener(this, toucheventselector(TextInputKeyboard::_onTouchText));
	widget2->addChild(inputLabelDiscribe_);

	this->setPositionTextKeyboard(textPosition_);

	float duration = 0.25f;
	float durationDiscribe = 0.83f;

	// анимация мигания текста
	actionText_ = CCRepeatForever::create(
		CCSequence::create(
			CCFadeOut::create(duration),
			CCFadeIn::create(duration),
			0
		));
	actionText_->retain();

	CCAction *actionTextDiscribe = CCRepeatForever::create(
		CCSequence::create(
			CCFadeOut::create(durationDiscribe),
			CCFadeIn::create(durationDiscribe),
			0
		));
	actionTextDiscribe->retain();
	inputLabelDiscribe_->runAction(actionTextDiscribe);
	isAction_ = false;
    
}


TextInputKeyboard::~TextInputKeyboard() {
	CCLog("~TextInputKeyboard()");
}


void TextInputKeyboard::setTextKeyboard(const char * beginText, int charLimit) {
	inputLabel_->setText(beginText);
	charLimit_ = charLimit;
}


void TextInputKeyboard::setPositionTextKeyboard(CCPoint textPosition) {
	textPosition_ = textPosition;
	inputLabel_->setPosition(textPosition_);
	inputLabelDiscribe_->setPosition(ccp(textPosition_.x, textPosition_.y - (inputLabel_->getContentSize().height/1.4f) * inputLabel_->getScale()));
}

void TextInputKeyboard::setSoundStatus(bool soundOn) {
	soundOn_ = soundOn;
}

void TextInputKeyboard::setScaleKeyboard(float keyboardScale) {
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	
	UILayer* keyboardLayer = (UILayer*)this->getChildByTag(TAG_KEYBOARD_LAYER);
	keyboardLayer->getWidgetByName("Panel_20")->setScale(keyboardScale);

}

void TextInputKeyboard::setColorKeyboard(ccColor3B colorKeyboard) {
	
	colorKeyboard_ = colorKeyboard;
	UILayer *keyboardLayer = (UILayer *)this->getChildByTag(TAG_KEYBOARD_LAYER);
	CCNode *panelSmallKey = keyboardLayer->getWidgetByName("PanelSmallKey");
	panelSmallKey->setVisible(true);

	CCArray *panelSmallKeyArray = panelSmallKey->getChildren();
	int keyCount = panelSmallKeyArray->count();

	for (int i=0; i<keyCount; ++i) {
		ImageView *keyboardKey = (ImageView *)panelSmallKeyArray->objectAtIndex(i);
		keyboardKey->setColor(colorKeyboard);
		keyboardKey->setOpacity(250);
		keyboardKey->setTouchEnabled(true);
		keyboardKey->addTouchEventListener(this, toucheventselector(TextInputKeyboard::_onTouchKeyboardButton));
	}

	CCNode *panelBigKey = keyboardLayer->getWidgetByName("PanelBigKey");
	panelBigKey->setVisible(false);

	CCArray *panelBigKeyArray = panelBigKey->getChildren();
	keyCount = panelBigKeyArray->count();

	for (int i=0; i<keyCount; ++i) {
		ImageView *keyboardKey = (ImageView *)panelBigKeyArray->objectAtIndex(i);
		keyboardKey->setColor(colorKeyboard);
		keyboardKey->setOpacity(250);
		keyboardKey->setTouchEnabled(false);
		keyboardKey->addTouchEventListener(this, toucheventselector(TextInputKeyboard::_onTouchKeyboardButton));
	}
}


void TextInputKeyboard::_onTouchKeyboardButton(CCObject* pSender, TouchEventType type) {
	
	CCSprite* tempKey = (CCSprite*)pSender;
	float tempScale;

    if(type == TOUCH_EVENT_BEGAN) {
		tempScale = tempKey->getScale();
		tempKey->setScale(tempScale * 2);
		tempKey->setColor(ccGRAY);

		CCLog("ZOrder = %d", tempKey->getZOrder());

		tempKey->setZOrder(100);


		if (TAG_KEY_BACKSPASE == tempKey->getTag())
			this->_onTextDelete();
		else if (TAG_KEY_END != tempKey->getTag() && TAG_KEY_SHIFT != tempKey->getTag()) {
			this->_onTextInsert(tempKey->getTag());
			}

		if (soundOn_)
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/event/eventpress.wav");

    }

	if (type == TOUCH_EVENT_ENDED || type == TOUCH_EVENT_CANCELED) {

		tempScale = tempKey->getScale();
		tempKey->setScale(tempScale/2);
		tempKey->setColor(colorKeyboard_);
		tempKey->setZOrder(2);

		if (soundOn_)
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/event/eventunpress.wav");

		if (type == TOUCH_EVENT_ENDED) {
			if (TAG_KEY_SHIFT == tempKey->getTag())
				this->_setTouchShift();

			if (TAG_KEY_END == tempKey->getTag())
				this->_closeKeybord();
		}
	}
}

void TextInputKeyboard::_setTouchShift() {

	UILayer *keyboardLayer = (UILayer *)this->getChildByTag(TAG_KEYBOARD_LAYER);
	CCNode *panelSmallKey = keyboardLayer->getWidgetByName("PanelSmallKey");
	panelSmallKey->setVisible(touchShift_);

	CCArray *panelSmallKeyArray = panelSmallKey->getChildren();
	int keyCount = panelSmallKeyArray->count();

	for (int i=0; i<keyCount; ++i) {
		ImageView *keyboardKey = (ImageView *)panelSmallKeyArray->objectAtIndex(i);
		keyboardKey->setTouchEnabled(touchShift_);
	}

	CCNode *panelBigKey = keyboardLayer->getWidgetByName("PanelBigKey");
	panelBigKey->setVisible(!touchShift_);

	CCArray *panelBigKeyArray = panelBigKey->getChildren();
	keyCount = panelBigKeyArray->count();

	for (int i=0; i<keyCount; ++i) {
		ImageView *keyboardKey = (ImageView *)panelBigKeyArray->objectAtIndex(i);
		keyboardKey->setTouchEnabled(!touchShift_);
	}

	touchShift_ = !touchShift_;

}


void TextInputKeyboard::_onTouchText(CCObject* pSender, TouchEventType type) {
	
	if(type == TOUCH_EVENT_BEGAN) {

		if (!isAction_) {

			if (soundOn_)
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/event/eventpress.wav");

			this->_openKeybord();
		}
		else {

			if (soundOn_)
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/event/eventunpress.wav");

			this->_closeKeybord();

		}
	}
}


void TextInputKeyboard::_openKeybord() {

	inputLabel_->runAction(actionText_);
	isAction_ = true;
	keyboadVisible_ = true;
	
	CCPoint endPos = ccp(TAG_BEGIN_POSITION_KEYBOARDX, 50);
	float duration = 0.2f;
	
	UILayer* keyboardLayer = (UILayer *)this->getChildByTag(TAG_KEYBOARD_LAYER);

	keyboardLayer->setVisible(true);
	CCAction * seq = CCSequence::create(
		CCSpawn::create(
			CCMoveTo::create(duration, endPos),
		0),
		CCCallFuncN::create(this, callfuncN_selector(TextInputKeyboard::_callbackKeybordUpAction)),
		0);

	keyboardLayer->runAction(seq);
}


void TextInputKeyboard::saveName() {
	// save name
}


void TextInputKeyboard::_closeKeybord() {
	
	inputLabel_->stopAction(actionText_);
	inputLabel_->setOpacity(255);
	inputLabelDiscribe_->setOpacity(255);
	isAction_ = false;

	CCPoint endPos = ccp(TAG_BEGIN_POSITION_KEYBOARDX, TAG_BEGIN_POSITION_KEYBOARDY);
	float duration = 0.2f;
	
	UILayer* keyboardLayer = (UILayer *)this->getChildByTag(TAG_KEYBOARD_LAYER);

	keyboardLayer->setVisible(true);
	CCAction * seq = CCSequence::create(
		CCSpawn::create(
			CCMoveTo::create(duration, endPos),
		0),
		CCCallFuncN::create(this, callfuncN_selector(TextInputKeyboard::_callbackKeybordUpAction)),
		0);

	keyboardLayer->runAction(seq);
	keyboadVisible_ = false;

	this->saveName();
}


bool TextInputKeyboard::_onTextInsert(int keyTag)
{

	CCLog("%c", keyTag);

	CCString *tempString = CCString::createWithFormat("%s", inputLabel_->getStringValue());
	tempString->length();

    // if insert enter, treat as default to detach with ime
    if (500 == keyTag)
    {
        return false;
    }
    
    // if the textfield's char count more than m_nCharLimit, doesn't insert text anymore.
	if (tempString->length() >= charLimit_)
    {
        return true;
    }

	inputLabel_->getStringValue();
	CCString *stringText = CCString::createWithFormat("%s%c", inputLabel_->getStringValue(), keyTag);
	inputLabel_->setText(stringText->getCString());

	CCString *string = CCString::createWithFormat("%c", keyTag);
    // create a insert text sprite and do some action
	LabelBMFont *label = LabelBMFont::create();
	label->setFntFile("fonts/play_100_0.fnt");
	label->setText(string->getCString());
	label->setScale(inputLabel_->getScale());

    this->addChild(label);
    ccColor3B color = { 226, 121, 7};
    label->setColor(color);

    // move the sprite from top to position
	CCPoint endPos = inputLabel_->getPosition();
	endPos.x += inputLabel_->getContentSize().width / 2 * inputLabel_->getScale();
    CCSize  inputTextSize = label->getContentSize();

	CCPoint beginPos(endPos.x, inputLabel_->getPositionY() - inputTextSize.height * inputLabel_->getScale()); 

    float duration = 0.5;
    label->setPosition(beginPos);
    label->setScale(8);

    CCAction * seq = CCSequence::create(
        CCSpawn::create(
            CCMoveTo::create(duration, endPos),
            CCScaleTo::create(duration, inputLabel_->getScale()),
            CCFadeOut::create(duration),
        0),
        CCCallFuncN::create(this, callfuncN_selector(TextInputKeyboard::_callbackRemoveNodeWhenDidAction)),
        0);
    label->runAction(seq);
    return false;
}


bool TextInputKeyboard::_onTextDelete() {

	std::string stringTemp = inputLabel_->getStringValue();
	int length = stringTemp.length();

	if (!length)
		return true;


	LabelBMFont *label = LabelBMFont::create();
	label->setFntFile("fonts/play_100_0.fnt");
	label->setText(stringTemp.substr(length-1, 1).c_str());
	label->setScale(inputLabel_->getScale());
    this->addChild(label);

	inputLabel_->setText(stringTemp.substr(0, length-1).c_str());

    // move the sprite to fly out
    CCPoint beginPos = inputLabel_->getPosition();
	CCSize textfieldSize = inputLabel_->getContentSize() * inputLabel_->getScale();
    CCSize labelSize = label->getContentSize() * inputLabel_->getScale();
    beginPos.x += (textfieldSize.width - labelSize.width) / 2.0f;
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	float posY = beginPos.y + (winSize.height / 3 * (-0.5 + (float)rand() / (RAND_MAX)));

	CCPoint endPos(- winSize.width / 4.0f, posY);

    float duration = 1;
    float rotateDuration = 0.2f;
    int repeatTime = 5; 
    label->setPosition(beginPos);

    CCAction * seq = CCSequence::create(
        CCSpawn::create(
            CCMoveTo::create(duration, endPos),
            CCRepeat::create(
                CCRotateBy::create(rotateDuration, (rand()%2) ? 360 : -360),
                repeatTime),
            CCFadeOut::create(duration),
        0),
        CCCallFuncN::create(this, callfuncN_selector(TextInputKeyboard::_callbackRemoveNodeWhenDidAction)),
        0);
    label->runAction(seq);
    return false;
}


void TextInputKeyboard::_callbackRemoveNodeWhenDidAction(CCNode * pNode)
{
    this->removeChild(pNode, true);
}

void TextInputKeyboard::_callbackKeybordUpAction(CCNode * pNode) {
	CCLog("_callbackKeybordUpAction");
}

bool TextInputKeyboard::isKeyboadVisible() {
	return keyboadVisible_;
}