#include "StatementTable.h"

StatementTable::StatementTable () {

}

Node* StatementTable::getStatement(int statementNum) {
	return table[statementNum];
}

void StatementTable::addStatement(int statementNum, Node* statementNode) {
	table[statementNum] = statementNode;
}