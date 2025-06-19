#include "EditorContent.h"

constexpr int TAB_WIDTH = 4;

EditorContent::EditorContent(TextDocument &textDocument) :
    document(textDocument) {
    this->cursor = Cursor(0, 0);
}

std::pair<int, int> EditorContent::cursorPosition() {
    int lineN = this->cursor.getLineN();
    int charN = this->cursor.getCharN();
    int column = this->getColumnFromCharN(lineN, charN);

    return std::pair<int, int>(lineN, column);
}

void EditorContent::createNewSelection(int anclaLine, int anclaChar) {
    this->selections.createNewSelection(anclaLine, anclaChar);
}

void EditorContent::createNewSelectionFromCursor() {
    this->createNewSelection(this->cursor.getLineN(), this->cursor.getCharN());
}

void EditorContent::updateLastSelection(int lineN, int charN) {
    this->selections.updateLastSelection(lineN, charN);
}

void EditorContent::removeSelections() {
    this->selections.removeSelections();
}

bool EditorContent::isSelected(int lineNumber, int charIndexInLine) {
    return this->selections.isSelected(lineNumber, charIndexInLine);
}

SelectionData::Selection EditorContent::getLastSelection() {
    return this->selections.getLastSelection();
}

void EditorContent::duplicateCursorLine() {
    this->removeSelections();

    int lineN = this->cursor.getLineN();
    sf::String lineToAdd = this->document.getLine(lineN);
    lineToAdd += '\n';
    this->document.addTextToPos(lineToAdd, lineN + 1, 0);
    this->moveCursorDown();
}

void EditorContent::swapSelectedLines(bool swapWithUp) {
    auto lastSelection = this->getLastSelection();
    if (!lastSelection.activa) {
        this->swapCursorLine(swapWithUp);
        return;
    }
    int rangeStart = SelectionData::getStartLineN(lastSelection);
    int rangeEnd = SelectionData::getEndLineN(lastSelection);

    int startLineN = SelectionData::getStartLineN(lastSelection);
    int startCharN = SelectionData::getStartCharN(lastSelection);
    int endLineN = SelectionData::getEndLineN(lastSelection);
    int endCharN = SelectionData::getEndCharN(lastSelection);

    if (swapWithUp && rangeStart > 0) {
        for (int i = rangeStart; i <= rangeEnd; i++) {
            this->document.swapLines(i, i - 1);
        }
        this->removeSelections();
        this->createNewSelection(startLineN - 1, startCharN);
        this->updateLastSelection(endLineN - 1, endCharN);

    } else if (!swapWithUp && rangeEnd < this->document.getLineCount() - 1) {
        for (int i = rangeEnd; i >= rangeStart; i--) {
            this->document.swapLines(i, i + 1);
        }
        this->removeSelections();
        this->createNewSelection(startLineN + 1, startCharN);
        this->updateLastSelection(endLineN + 1, endCharN);
    }
}

void EditorContent::swapCursorLine(bool swapWithUp) {
    int currentLine = this->cursor.getLineN();
    if (swapWithUp) {
        this->document.swapLines(currentLine, std::max(currentLine - 1, 0));
    } else {
        this->document.swapLines(currentLine, std::min(currentLine + 1, this->document.getLineCount() - 1));
    }
}

bool EditorContent::moveCursorLeft(bool updateActiveSelections) {
    bool moved = (this->cursor.getLineN() != 0)
    || ((this->cursor.getLineN() == 0) && (this->cursor.getCharN() > 0));

    if (this->cursor.getCharN() <= 0) {
        int newCursorLine = std::max(this->cursor.getLineN() - 1, 0);
        int newCursorChar = 0;
        if (this->cursor.getLineN() != 0) {
            newCursorChar = this->document.charsInLine(newCursorLine);
        }
        this->cursor.setPosition(newCursorLine, newCursorChar, true);
    } else {
        this->cursor.moveLeft(true);
    }

    this->handleSelectionOnCursorMovement(updateActiveSelections);

    return moved;
}

void EditorContent::moveCursorRight(bool updateActiveSelections) {
    int charsInLine = this->document.charsInLine(this->cursor.getLineN());
    if (this->cursor.getCharN() >= charsInLine) {
        int newCursorLine = std::min(this->cursor.getLineN() + 1, this->document.getLineCount() - 1);
        if (newCursorLine != this->cursor.getLineN()) {
          this->cursor.setPosition(newCursorLine, 0, true);
        }
    } else {
        this->cursor.moveRight(true);
    }

    this->handleSelectionOnCursorMovement(updateActiveSelections);
}

void EditorContent::moveCursorUp(bool updateActiveSelections) {
    if (this->cursor.getLineN() > 0) {
        int charsInPreviousLine = this->document.charsInLine(this->cursor.getLineN() - 1);
        int currentCharPos = this->cursor.getCharN();

        if (currentCharPos <= charsInPreviousLine && this->cursor.getMaxCharNReached() <= charsInPreviousLine) {
            this->cursor.moveUpToMaxCharN();
        } else {
            this->cursor.setPosition(this->cursor.getLineN() - 1, charsInPreviousLine);
        }
    }

    this->handleSelectionOnCursorMovement(updateActiveSelections);
}

void EditorContent::moveCursorDown(bool updateActiveSelections) {
    if (this->cursor.getLineN() < this->document.getLineCount() - 1) {
        int charsInNextLine = this->document.charsInLine(this->cursor.getLineN() + 1);
        int currentCharPos = this->cursor.getCharN();

        if (currentCharPos <= charsInNextLine && this->cursor.getMaxCharNReached() <= charsInNextLine) {
            this->cursor.moveDownToMaxCharN();
        } else {
            this->cursor.setPosition(this->cursor.getLineN() + 1, charsInNextLine);
        }
    }

    this->handleSelectionOnCursorMovement(updateActiveSelections);
}

void EditorContent::moveCursorToEnd(bool updateActiveSelections) {
    int charsInLine = this->document.charsInLine(this->cursor.getLineN());
    this->cursor.moveToEnd(charsInLine, true);
    this->handleSelectionOnCursorMovement(updateActiveSelections);
}

void EditorContent::moveCursorToStart(bool updateActiveSelections) {
    this->cursor.moveToStart(true);
    this->handleSelectionOnCursorMovement(updateActiveSelections);
}


void EditorContent::deleteTextBeforeCursorPos(int amount) {
    int actuallyMoved = 0;
    for (int i = 0; i < amount; i++) {
        bool moved = this->moveCursorLeft();
        actuallyMoved += moved ? 1 : 0;
    }
    this->deleteTextAfterCursorPos(actuallyMoved);
}

void EditorContent::deleteTextAfterCursorPos(int amount) {
    int newLineN = this->cursor.getLineN();
    int newCharN = this->cursor.getCharN();
    this->document.removeTextFromPos(amount, newLineN, newCharN);
}


void EditorContent::addTextInCursorPos(sf::String text) {
    int textSize = text.getSize();
    int lineN = this->cursor.getLineN();
    int charN = this->cursor.getCharN();

    this->document.addTextToPos(text, lineN, charN);
    for (int i = 0; i < textSize; i++) {
        this->moveCursorRight();
    }
}

bool EditorContent::deleteSelections() {
    SelectionData::Selection lastSelection = this->getLastSelection();
    this->removeSelections();

    if (lastSelection.activa) {
        int startLineN = SelectionData::getStartLineN(lastSelection);
        int startCharN = SelectionData::getStartCharN(lastSelection);
        int endLineN = SelectionData::getEndLineN(lastSelection);
        int endCharN = SelectionData::getEndCharN(lastSelection);

        this->cursor.setPosition(startLineN, startCharN, true);

        int amount = this->document.charAmountContained(startLineN, startCharN, endLineN, endCharN) - 1;
        this->deleteTextAfterCursorPos(amount);
    }

    return lastSelection.activa;
}

sf::String EditorContent::copySelections() {
    SelectionData::Selection lastSelection = this->getLastSelection();

    sf::String copied = "";
    if (lastSelection.activa) {
        int startLineN = SelectionData::getStartLineN(lastSelection);
        int startCharN = SelectionData::getStartCharN(lastSelection);
        int endLineN = SelectionData::getEndLineN(lastSelection);
        int endCharN = SelectionData::getEndCharN(lastSelection);

        this->cursor.setPosition(startLineN, startCharN, true);

        int amount = this->document.charAmountContained(startLineN, startCharN, endLineN, endCharN) - 1;
        copied = this->document.getTextFromPos(amount, startLineN, startCharN);
    }
    return copied;
}

void EditorContent::handleSelectionOnCursorMovement(bool updateActiveSelections) {
    if (updateActiveSelections) {
        this->updateLastSelection(this->cursor.getLineN(), this->cursor.getCharN());
    } else {
        this->removeSelections();
    }
}

int EditorContent::linesCount() {
    return this->document.getLineCount();
}

// TODO: cols != chars
int EditorContent::colsInLine(int line) {
    return this->document.charsInLine(line);
}

sf::String EditorContent::getLine(int line) {
    return this->document.getLine(line);
}

sf::String EditorContent::getCursorLine() {
    return this->getLine(cursor.getLineN());
}

void EditorContent::resetCursor(int line, int column) {
    this->cursor.setPosition(line, column);
    this->cursor.setMaxCharNReached(column);
}

int EditorContent::getCharIndexOfColumn(int lineN, int column) {
    sf::String line = this->getLine(lineN);
    int len = this->colsInLine(lineN);
    int currentCol = 0;
    for (int charN = 0; charN < len; charN++) {

        if (column <= currentCol) {
            return charN;
        }

        if (line[charN] == '\t') {
            currentCol += TAB_WIDTH;
        } else {
            currentCol++;
        }
    }
    return len; 
}

int EditorContent::getColumnFromCharN(int lineN, int charN) {
    sf::String line = this->getLine(lineN);

    int currentCol = 0;
    for (int charNact = 0; charNact < charN; charNact++) {
        if (line[charNact] == '\t') {
            currentCol += 4;
        } else {
            currentCol++;
        }
    }

    return currentCol;
}