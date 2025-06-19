#include "EditorView.h"

EditorView::EditorView(
    const sf::RenderWindow &window,
    const sf::String &workingDirectory,
    EditorContent &editorContent)
    : content(editorContent),
      camera(sf::FloatRect(-50, 0, window.getSize().x, window.getSize().y)),
      deltaScroll(20), deltaRotation(2), deltaZoomIn(0.8f), deltaZoomOut(1.2f) {
    this->font.loadFromFile(workingDirectory + "fonts/DejaVuSansMono.ttf");

    this->bottomLimitPx = 1;
    this->rightLimitPx = 1;

    this->setFontSize(18);  

    this->marginXOffset = 45;
    this->colorMargin = sf::Color(32, 44, 68);

    this->colorChar = sf::Color::White;
    this->colorSelection = sf::Color(106, 154, 232);
}

void EditorView::setFontSize(int fontSize) {
    this->fontSize = fontSize;
    this->lineHeight = fontSize;
    sf::Text tmpText;
    tmpText.setFont(this->font);
    tmpText.setCharacterSize(this->fontSize);
    tmpText.setString("_");
    float textwidth = tmpText.getLocalBounds().width;
    this->charWidth = textwidth;
}

float EditorView::getRightLimitPx() {
    return this->rightLimitPx;
}

float EditorView::getBottomLimitPx() {
    return this->bottomLimitPx;
}

int EditorView::getLineHeight() {
    return this->lineHeight;
}

int EditorView::getCharWidth() {
    return this->charWidth;
}

void EditorView::draw(sf::RenderWindow &window) {
    this->drawLines(window);

    for (int lineNumber = 1; lineNumber <= this->content.linesCount(); lineNumber++) {
        int lineHeight = 1;

        int blockHeight = lineHeight * this->fontSize;

        sf::Text lineNumberText;
        lineNumberText.setFillColor(sf::Color::White);
        lineNumberText.setFont(this->font);
        lineNumberText.setString(std::to_string(lineNumber));
        lineNumberText.setCharacterSize(this->fontSize - 1);
        lineNumberText.setPosition(-this->marginXOffset, blockHeight * (lineNumber - 1));

        sf::RectangleShape marginRect(sf::Vector2f(this->marginXOffset - 5, blockHeight));
        marginRect.setFillColor(this->colorMargin);
        marginRect.setPosition(-this->marginXOffset, blockHeight * (lineNumber - 1));

        window.draw(marginRect);
        window.draw(lineNumberText);
    }

    this->drawCursor(window);
}

int colsOf(sf::String &currentLineText) {
    int cols = 0;
    for (char c : currentLineText) {
        if (c == '\t') {
            cols += 4;
        } else {
            cols++;
        }
    }
    return cols;
}

void EditorView::drawLines(sf::RenderWindow &window) {
    this->bottomLimitPx = this->content.linesCount() * this->fontSize;

    for (int lineNumber = 0; lineNumber < this->content.linesCount(); lineNumber++) {
        sf::String line = this->content.getLine(lineNumber);
        sf::String currentLineText = "";
        this->rightLimitPx = std::max((int)this->rightLimitPx, (int)(this->charWidth * line.getSize()));

        float offsetx = 0;
        bool previousSelected = false;

        for (int charIndexInLine = 0; charIndexInLine <= (int)line.getSize(); charIndexInLine++) {
            bool currentSelected = content.isSelected(lineNumber, charIndexInLine);
            if (currentSelected != previousSelected || charIndexInLine == (int)line.getSize()) {
                sf::Text texto;
                texto.setFillColor(this->colorChar);
                texto.setFont(font);
                texto.setString(currentLineText);
                texto.setCharacterSize(this->fontSize);
                texto.setPosition(offsetx, lineNumber * this->fontSize);

                if (previousSelected) {
                    int currentColsAmount = colsOf(currentLineText);
                    sf::RectangleShape selectionRect(
                        sf::Vector2f(this->charWidth * currentColsAmount, this->fontSize));
                    selectionRect.setFillColor(this->colorSelection);
                    selectionRect.setPosition(offsetx, 2 + lineNumber * this->fontSize);
                    window.draw(selectionRect);
                }

                window.draw(texto);

                previousSelected = currentSelected;
                offsetx += this->charWidth * colsOf(currentLineText);
                currentLineText = "";
            }
            currentLineText += line[charIndexInLine];
        }
    }
}

void EditorView::drawCursor(sf::RenderWindow &window) {
    int offsetY = 2;
    int cursorDrawWidth = 2;

    int charWidth = getCharWidth();
    int lineHeight = getLineHeight();

    std::pair<int, int> cursorPos = this->content.cursorPosition();
    int lineN = cursorPos.first;
    int column = cursorPos.second;

    sf::RectangleShape cursorRect(sf::Vector2f(cursorDrawWidth, lineHeight));
    cursorRect.setFillColor(sf::Color::White);

    cursorRect.setPosition(
        column * charWidth,
        (lineN * lineHeight) + offsetY);

    window.draw(cursorRect);
}

std::pair<int, int> EditorView::getDocumentCoords(
    float mouseX, float mouseY) {

    int lineN = mouseY / this->getLineHeight();
    int charN = 0;

    int lastLine = this->content.linesCount() - 1;

    if (lineN < 0) {
        lineN = 0;
        charN = 0;
    } else if (lineN > lastLine) {
        lineN = lastLine;
        charN = this->content.colsInLine(lineN);
    } else {
        lineN = std::max(lineN, 0);
        lineN = std::min(lineN, lastLine);
        int column = std::round(mouseX / this->getCharWidth());
        charN = this->content.getCharIndexOfColumn(lineN, column);
        charN = std::max(charN, 0);
        charN = std::min(charN, this->content.colsInLine(lineN));
    }

    return std::pair<int, int>(lineN, charN);
}

void EditorView::scrollUp(sf::RenderWindow &window) {
    float height = window.getView().getSize().y;
    auto camPos = this->camera.getCenter();
    if (camPos.y - height / 2 > 0) {
        this->camera.move(0, -this->deltaScroll);
    }
}

void EditorView::scrollDown(sf::RenderWindow &window) {
    float height = window.getView().getSize().y;
    float bottomLimit = std::max(this->getBottomLimitPx(), height);
    auto camPos = this->camera.getCenter();
    if (camPos.y + height / 2 < bottomLimit + 20) {
        this->camera.move(0, this->deltaScroll);
    }
}

void EditorView::scrollLeft(sf::RenderWindow &window) {
    float width = window.getView().getSize().x;
    auto camPos = this->camera.getCenter();
    if (camPos.x - width / 2 > -this->marginXOffset) {
        this->camera.move(-this->deltaScroll, 0);
    }
}

void EditorView::scrollRight(sf::RenderWindow &window) {
    float width = window.getView().getSize().x;
    float rightLimit = std::max(this->getRightLimitPx(), width);
    auto camPos = this->camera.getCenter();
    if (camPos.x + width / 2 < rightLimit + 20) {
        this->camera.move(this->deltaScroll, 0);
    }
}

void EditorView::rotateLeft() {
    this->camera.rotate(this->deltaRotation);
}

void EditorView::rotateRight() {
    this->camera.rotate(-this->deltaRotation);
}

void EditorView::zoomIn() {
    this->camera.zoom(this->deltaZoomIn);
}

void EditorView::zoomOut() {
    this->camera.zoom(this->deltaZoomOut);
}

void EditorView::setCameraBounds(int width, int height) {
    this->camera = sf::View(sf::FloatRect(-50, 0, width, height));
}

sf::View EditorView::getCameraView() {
    return this->camera;
}

void EditorView::setDeltaScroll(float delta) {
    this->deltaScroll = delta;
}

void EditorView::setDeltaRotation(float delta) {
    this->deltaRotation = delta;
}