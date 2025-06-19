#include "InputController.h"

InputController::InputController(EditorContent &editorContent)
    : editorContent(editorContent) {
    this->mouseDown = false;
    this->shiftPressed = false;
}

void InputController::handleEvents(
    EditorView &textView,
    sf::RenderWindow &window,
    sf::Event &event) {

    this->handleMouseEvents(textView, window, event);
    this->handleKeyPressedEvents(textView, event);
    this->handleKeyReleasedEvents(event);
    this->handleTextEnteredEvent(event);
}

void InputController::handleConstantInput(EditorView &textView,
                                          sf::RenderWindow &window) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                textView.rotateLeft();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                textView.rotateRight();
            }
        }
    }

    if (this->isMouseDown()) {
        auto mousepos = sf::Mouse::getPosition(window);
        auto mousepos_text = window.mapPixelToCoords(mousepos);

        updateCursorInEditor(textView, mousepos_text.x, mousepos_text.y);

        float textViewTop = 0;
        float textViewBottom = window.getView().getSize().y - 5;
        float textViewLeft = 0;
        float textViewRight = window.getView().getSize().x;

        if (mousepos.x < textViewLeft) {
            textView.scrollLeft(window);
        } else if (mousepos.x > textViewRight) {
            textView.scrollRight(window);
        }

        if (mousepos.y < textViewTop) {
            textView.scrollUp(window);
        } else if (mousepos.y > textViewBottom) {
            textView.scrollDown(window);
        }
    }
}

void InputController::handleMouseEvents(
    EditorView &textView,
    sf::RenderWindow &window,
    sf::Event &event) {

    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            if (event.mouseWheelScroll.delta > 0) {
                textView.scrollUp(window);
            } else {
                textView.scrollDown(window);
            }
        } else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel) {
            if (event.mouseWheelScroll.delta > 0) {
                textView.scrollLeft(window);
            } else {
                textView.scrollRight(window);
            }
        }
    }
    if (event.type == sf::Event::MouseButtonPressed) {
        this->editorContent.removeSelections();
        auto mousepos = sf::Mouse::getPosition(window);
        auto mousepos_text = window.mapPixelToCoords(mousepos);

        std::pair<int, int> docCoords = textView.getDocumentCoords(mousepos_text.x, mousepos_text.y);
        this->editorContent.createNewSelection(docCoords.first, docCoords.second);

        this->mouseDown = true;
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        this->mouseDown = false;
    }
}

void InputController::handleKeyPressedEvents(EditorView &textView, sf::Event &event) {
    if (event.type == sf::Event::KeyPressed) {
        bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);

        bool isShiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

        bool isEndPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::End);

        bool isHomePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Home);

        if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
            if (!this->shiftPressed && !isCtrlPressed) {
                this->shiftPressed = true;
             
                this->editorContent.removeSelections();
                this->editorContent.createNewSelectionFromCursor();
                return;
            }
        }

        if (isEndPressed) {
            editorContent.moveCursorToEnd(isShiftPressed);
            return;
        } else if (isHomePressed) {  
            editorContent.moveCursorToStart(isShiftPressed);
            return;
        }

        bool ctrlAndShift = isCtrlPressed && isShiftPressed;

        if (isCtrlPressed) {
            if (event.key.code == sf::Keyboard::D) {
                editorContent.duplicateCursorLine();
            } else if (event.key.code == sf::Keyboard::U) {
                editorContent.deleteSelections();
                sf::String emoji = "\\_('-')_/";
                editorContent.addTextInCursorPos(emoji);
            } else if (event.key.code == sf::Keyboard::C) { 
                this->stringCopied = editorContent.copySelections();
                if (this->stringCopied.isEmpty()) {
                    this->stringCopied = editorContent.getCursorLine();
                }
            } else if (event.key.code == sf::Keyboard::V) { 
                editorContent.addTextInCursorPos(stringCopied);
            } else if (event.key.code == sf::Keyboard::X) {  
                this->stringCopied = editorContent.copySelections();
                editorContent.deleteSelections();
            }
        }

        if (event.key.code == sf::Keyboard::Up) {
            if (ctrlAndShift) {
                editorContent.swapSelectedLines(true);
                editorContent.moveCursorUp(true);
                return;
            } else {
                editorContent.moveCursorUp(this->shiftPressed);
                return;
            }
        }
        if (event.key.code == sf::Keyboard::Down) {
            if (ctrlAndShift) {
                editorContent.swapSelectedLines(false);
                editorContent.moveCursorDown(true);
                return;
            } else {
                editorContent.moveCursorDown(this->shiftPressed);
                return;
            }
        }
        if (event.key.code == sf::Keyboard::Left) {
            editorContent.moveCursorLeft(this->shiftPressed && !isCtrlPressed);
            return;
        }
        if (event.key.code == sf::Keyboard::Right) {
            editorContent.moveCursorRight(this->shiftPressed && !isCtrlPressed);
            return;
        }

        if (event.key.control) {
            if (event.key.code == sf::Keyboard::Add) {
                textView.zoomIn();
                return;
            }
            if (event.key.code == sf::Keyboard::Subtract) {
                textView.zoomOut();
                return;
            }
        }
    }
}

void InputController::handleKeyReleasedEvents(sf::Event &event) {
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
            this->shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
        }
    }
}

void InputController::handleTextEnteredEvent(sf::Event &event) {
    if (event.type == sf::Event::TextEntered) {
        bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
        sf::String input(event.text.unicode);

        if (event.text.unicode == '\b') {
            bool selecionDeleted = editorContent.deleteSelections();
            if (!selecionDeleted) {
                editorContent.deleteTextBeforeCursorPos(1);
            }
        } else if (event.text.unicode == 127) {  
            bool selecionDeleted = editorContent.deleteSelections();
            if (!selecionDeleted) {
                editorContent.deleteTextAfterCursorPos(1);
            }
        } else if (!ctrlPressed) {
            if (event.text.unicode == '\t') {
                std::cerr << "TABS ACTIVADOS " << std::endl;
            }

            editorContent.deleteSelections();
            editorContent.addTextInCursorPos(input);
        }
    }
}

bool InputController::isMouseDown() {
    return this->mouseDown;
}

void InputController::updateCursorInEditor(EditorView &textView, float mouseX, float mouseY) {
    std::pair<int, int> docCoords = textView.getDocumentCoords(mouseX, mouseY);
    int line = docCoords.first;
    int column = docCoords.second;

    this->editorContent.resetCursor(line, column);
    this->editorContent.updateLastSelection(line, column);
}