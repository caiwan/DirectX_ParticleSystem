#include "Input.h"

Input::Input() : _point(), _rect(){
	_mouseXabsolute = _mouseYabsolute = 0;
	_mouseZabsolute = 1;
	_mouseXrelative = _mouseXrelative = _mouseXrelative = 0;
}

Input::~Input(){
	unload();
}

void Input::unload(){

}

bool Input::initialize(HINSTANCE hInstance, HWND hWnd, int screenWidth, int screenHeight){
	return true;
}

void Input::updateInput(){

}

BYTE* Input::getKeyboardState(){
	return _keyboardState;
}

LONG Input::getMouseXRelative(){
	return _mouseXrelative;
}

LONG Input::getMouseYRelative(){
	return _mouseYrelative;
}

LONG Input::getMouseZRelative(){
	return _mouseZrelative;
}

LONG Input::getMouseXAbsolute(){
	return _mouseXabsolute;
}

LONG Input::getMouseYAbsolute(){
	return _mouseYabsolute;
}

LONG Input::getMouseZAbsolute(){
	return _mouseZabsolute/120;
}

BYTE Input::getLeftMouseClick(){
	return _leftMouseButton;
}

BYTE Input::getRightMouseClick(){
	return _rightMouseButton;
}

