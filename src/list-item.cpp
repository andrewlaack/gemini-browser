#include "../include/list-item.hpp"

ListItem::ListItem(std::string input)  : text(input) {}

std::string ListItem::textToDraw() {
    return text + "\n";
}

int ListItem::getColor() {
    return 15;
}

LineType ListItem::type() {
    return LIST_ITEM;
}

bool ListItem::isBold() {
    return false;
}
