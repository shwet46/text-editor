#ifndef SelectionData_H
#define SelectionData_H

#include <iostream>
#include <vector>
#include "TextDocument.h"

class SelectionData {
   private:
    struct Extremo {
        Extremo() : lineN(-1), charN(-1) {}
        Extremo(int lineN, int charN) : lineN(lineN), charN(charN){};
        int lineN;
        int charN;

        bool operator<(const Extremo &ex) const {
            return lineN < ex.lineN || (lineN <= ex.lineN && charN < ex.charN);
        }
    };

   public:
    SelectionData();

    struct Selection {
        Selection() : activa(false), ancla(), extremo() {}
        Selection(int anclaLine, int anclaChar) : activa(false), ancla(anclaLine, anclaChar), extremo() {}

        bool activa;
        SelectionData::Extremo ancla;
        SelectionData::Extremo extremo;
    };

    void createNewSelection(int anclaLine, int anclaChar);
    void updateLastSelection(int extremoLine, int extremoChar);

    void removeSelections();
    bool isSelected(int lineN, int charN) const;

    Selection getLastSelection();

    void moveSelectionsRight(int charAmount, const TextDocument &doc);
    void moveSelectionsLeft(int charAmount, const TextDocument &doc);

    static int getStartLineN(Selection &selection);
    static int getStartCharN(Selection &selection);
    static int getEndLineN(Selection &selection);
    static int getEndCharN(Selection &selection);

   private:
    std::vector<Selection> selections;
    int lastSelectionIndex;

    int getLastAnclaLine();
    int getLastAnclaChar();
    int getLastIsActive();

    bool validIndex(int index);
    void removeSelection(int index);
};

#endif