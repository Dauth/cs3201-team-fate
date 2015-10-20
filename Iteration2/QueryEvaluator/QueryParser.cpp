#include "stdafx.h"
#include <ctype.h>
#include <regex>
#include <fstream>
#include "QueryParser.h"
#include "Symbol.h"

using namespace std;

namespace TOKEN{
	enum Value { SELECT, RESULT_CL, WITH_CL, SUCHTHAT_CL, PATTERN_CL, AND_END, QUERY_CL };
}

TOKEN::Value currToken = TOKEN::SELECT;
TOKEN::Value previousToken = TOKEN::SELECT;

bool expectingThat;
bool resultBool;
bool tupleError;
bool nonExistantSyn;
bool suchThatQueryExist;
bool patternExist;
bool suchThatQueryPass;
bool patternPass;
bool nonQueryToken;
bool invalidSuchThat;
bool existClauses;
bool validWith;
bool existWith;
bool synonymError;
bool completeSelectStmt;

//QueryTree* rootTree;
Symbol* newSymbol;
Query_Object qo;

string concatStmt;
string errorMsg = "ERROR(s) : \n";

//regex stmtRef_old("(Parent|Parent\\*|Affects|Affects\\*|Follows|Follows\\*)\\((([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+),(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)\\)");

// INDENT | "_" | INTEGER
regex stmtRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)");
regex lineRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+)");

// INDENT | "_"| "INDENT" | INTEGER
regex entRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|(\\d)+|\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\")");

// INDENT | "_" | "INDENT"
regex varRef("(([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*|_|\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\")");

// INDENT'.'attrName | "INDENT" | INTEGER | synonym
regex attrRef("([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\.(procName|varName|value|stmt#)");
regex synonym ("([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*");
regex integerRegex ("(\\d)+");
regex quotesIndent("\"([a-zA-Z])+(([a-zA-Z])|#|(\\d)+)*\"");

regex expressionSpec("\"(([a-zA-Z])+(([a-zA-Z])|(\\d)+)*|\\d+)((\\+|\\-|\\*)(([a-zA-Z])+(([a-zA-Z])|(\\d)+)*|\\d+))*\"|_");
regex underScoresBothSides("_\"(([a-zA-Z])+(([a-zA-Z])|(\\d)+)*|\\d+)((\\+|\\-|\\*)(([a-zA-Z])+(([a-zA-Z])|(\\d)+)*|\\d+))*\"_");
regex underLine ("_");
QueryParser::QueryParser () {

}

vector<string> split(const char *str, char c = ' ')
{
	vector<string> result;

	do
	{
		const char *begin = str;

		while(*str != c && *str)
			str++;

		result.push_back(string(begin, str));
	} while (0 != *str++);

	return result;
}
void removeCharsFromString( string &str, char* charsToRemove ) {
	for ( unsigned int i = 0; i < strlen(charsToRemove); ++i ) {
		str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
	}
}

bool verifyCorrectParameters(SyntType currentSyn, string firstParam, string secondParam, string thirdParam)
{
	bool valid = false;
	regex underScore ("_");
	if (currentSyn == SyntType::assignment)
	{
		if (regex_match(firstParam,varRef))
		{
			if (regex_match(secondParam,expressionSpec) || regex_match(secondParam,underScoresBothSides))
			{

				valid = true;
			}
			else
			{
				errorMsg += "- Invalid second parameter of the pattern.";
			}
		}
		else
		{
			errorMsg += "- Invalid first parameter of the pattern.";
		}
	}
	else if (currentSyn == SyntType::whileLoop)
	{
		if (regex_match(firstParam,varRef) && regex_match(secondParam, underScore))
		{
			valid = true;
		}
	}
	else if (currentSyn == SyntType::ifelse)
	{
		if (regex_match(firstParam,varRef) && regex_match(secondParam,underScore) && regex_match(thirdParam, underScore))
		{
			valid = true;
		}
	}
	return valid;
}
bool verifyCorrectParameters(QueryType queryClause,string firstParam, string secondParam)
{
	bool valid = false;
	if (queryClause == QueryType::modifies)
	{
		if (regex_match(firstParam,entRef) && regex_match(secondParam,entRef))
		{
			if (!regex_match(firstParam,underLine) && regex_match(secondParam,entRef))
			{
				valid = true;
			}
		}
	}
	else if (queryClause == QueryType::uses)
	{
		if (regex_match(firstParam,entRef) && regex_match(secondParam,varRef))
		{
			if (!regex_match(firstParam,underLine) && regex_match(secondParam,entRef))
			{
				valid = true;
			}
		}
	}
	else if (queryClause == QueryType::calls)
	{
		if (regex_match(firstParam,entRef) && regex_match(secondParam,entRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::callsStar)
	{
		if (regex_match(firstParam,entRef) && regex_match(secondParam,entRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::parent)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::parentStar)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::follows)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::followsStar)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::next)
	{
		if (regex_match(firstParam,lineRef) && regex_match(secondParam,lineRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::nextStar)
	{
		if (regex_match(firstParam,lineRef) && regex_match(secondParam,lineRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::affects)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	else if (queryClause == QueryType::affectsStar)
	{
		if (regex_match(firstParam,stmtRef) && regex_match(secondParam,stmtRef))
		{
			valid = true;
		}
	}
	return valid;
}
bool isCorrectWithClause(string leftString, string rightString)
{
	SyntType leftSynt, rightSynt;
	AttrType leftAttr, rightAttr;
	bool leftValid= false;
	bool rightValid = false;
	bool isValid = false;

	// 1a. Check if leftHand matches synonym.attributeName
	if (regex_match(leftString, attrRef))
	{
		std::vector<char> writable(leftString.begin(), leftString.end());
		writable.push_back('\0');
		vector<string> attrVariables = split(&writable[0],'.');

		// 1ai. Check if leftHand's synonym exists and then get the SyntType
		if(newSymbol->exists(attrVariables[0]))
		{
			leftSynt =  newSymbol->getSyntType(attrVariables[0]);
			// 1aii. Check if leftHand's synonym is a statement, then check if leftHand attribute is stmt#
			if (leftSynt == SyntType::statementList||leftSynt == SyntType::statement || leftSynt == SyntType::assignment || leftSynt == SyntType::call || leftSynt == SyntType::whileLoop || leftSynt == SyntType::ifelse)
			{
				// left hand pass
				const char * leftHandChar = attrVariables[1].c_str();
				if (strcmp(leftHandChar,"stmt#") == 0)
				{
					// pass ( .stmt#) and set AttrType to integer
					leftAttr = AttrType::integerType;
					leftValid = true;
				}
				else
				{
					// fail (this statement has a dot(.) something else besides stmt#)
				}
			}
			// 1aiii. Check if leftHand's synonym is constant/variable, then check if leftHand attribute is value
			else if (leftSynt == SyntType::constant)
			{
				const char * leftHandChar = attrVariables[1].c_str();
				if (strcmp(leftHandChar,"value") == 0)
				{
					// pass ( .value) and set AttrType to integer
					leftAttr = AttrType::integerType;
					leftValid = true;
				}
				else
				{
					// fail (this statement has a dot(.) something else besides value)
				}
			}
			else if (leftSynt == SyntType::procedure || leftSynt == SyntType::call)
			{
				const char * leftHandChar = attrVariables[1].c_str();
				if (strcmp(leftHandChar, "procName") == 0)
				{
					leftAttr = AttrType::stringType;
					leftValid = true;
				}
				else
				{
					// fail (this statement has a dot(.) something else besides procName
				}
			}
			else if (leftSynt == SyntType::variable)
			{				
				const char * leftHandChar = attrVariables[1].c_str();
				if (strcmp(leftHandChar, "varName") == 0)
				{
					leftAttr = AttrType::stringType;
					leftValid = true;
				}
				else
				{
					// fail (this statement has a dot(.) something else besides procName
				}

			}
		} 
		else
		{
			//Synonym does not exist in Symbol table, fail!
		}

	}// end of checking left hand for synonym.attributeName



	// 1b). Check if leftHand matches synonym (and it has to be progline)
	else if (regex_match(leftString, synonym))
	{
		if(newSymbol->getSyntType(leftString) == SyntType::progline)
		{
			// pass
			leftAttr = AttrType::integerType;
			leftValid = true;
			leftSynt = SyntType::progline;
		}
		else 
		{
			// fail
		}
	}
	// 1c)
	else if (regex_match(leftString, integerRegex))
	{
		// pass and store type for comparison
		leftAttr = AttrType::integerType;
		leftValid = true;
		leftSynt = SyntType::integer;
	}
	// 1d)
	else if (regex_match(leftString,quotesIndent))
	{
		// pass and store type for comparison
		leftAttr = AttrType::stringType;
		leftValid = true;
		leftSynt = SyntType::expression;
		removeCharsFromString(leftString, "\"");
	}
	// 1e) FAILS
	else
	{
		//fail 
	}


	// 2a. 
	if (regex_match(rightString, attrRef))
	{
		std::vector<char> writable(rightString.begin(), rightString.end());
		writable.push_back('\0');
		vector<string> attrVariables = split(&writable[0],'.');

		// 1ai. Check if rightHand's synonym exists and then get the SyntType
		if(newSymbol->exists(attrVariables[0]))
		{
			rightSynt =  newSymbol->getSyntType(attrVariables[0]);
			// 2aii. Check if rightHand's synonym is a statement, then check if rightHand attribute is stmt#
			if (rightSynt == SyntType::statementList||rightSynt == SyntType::statement || rightSynt == SyntType::assignment || rightSynt == SyntType::call || rightSynt == SyntType::whileLoop || rightSynt == SyntType::ifelse)
			{
				// right hand pass
				const char * rightHandChar = attrVariables[1].c_str();
				if (strcmp(rightHandChar,"stmt#") == 0)
				{
					// pass ( .stmt#) and set AttrType to integer
					rightAttr = AttrType::integerType;
					rightValid = true;
				}
				else
				{
					// fail (this statement has a dot(.) something else besides stmt#)
				}
			}
			// 2aiii. Check if leftHand's synonym is constant/variable, then check if leftHand attribute is value
			else if (rightSynt == SyntType::constant)
			{
				const char * rightHandChar = attrVariables[1].c_str();
				if (strcmp(rightHandChar,"value") == 0)
				{
					// pass ( .value) and set AttrType to integer
					rightAttr = AttrType::integerType;
					rightValid = true;
				}
				else
				{
					// fail (this statement has a dot(.) something else besides value)
				}
			}
			else if (rightSynt == SyntType::procedure || rightSynt == SyntType::call)
			{
				const char * rightHandChar = attrVariables[1].c_str();
				if (strcmp(rightHandChar, "procName") == 0)
				{
					rightAttr = AttrType::stringType;
					rightValid = true;
				}
				else
				{
					// fail (this statement has a dot(.) something else besides procName
				}
			}
			else if (rightSynt == SyntType::variable)
			{				
				const char * rightHandChar = attrVariables[1].c_str();
				if (strcmp(rightHandChar, "varName") == 0)
				{
					rightAttr = AttrType::stringType;
					rightValid = true;
				}
				else
				{
					// fail (this statement has a dot(.) something else besides procName
				}

			}
		} 
		else
		{
			//Synonym does not exist in Symbol table, fail!
		}

	}// end of checking left hand for synonym.attributeName



	// 2b). Check if rightHand matches synonym (and it has to be progline)
	else if (regex_match(rightString, synonym))
	{
		if(newSymbol->getSyntType(rightString) == SyntType::progline)
		{
			// pass
			rightAttr = AttrType::integerType;
			rightValid = true;
			rightSynt = SyntType::progline;
		}
		else 
		{
			// fail
		}
	}
	// 1c)
	else if (regex_match(rightString, integerRegex))
	{
		// pass and store type for comparison
		rightAttr = AttrType::integerType;
		rightValid = true;
		rightSynt = SyntType::integer;
	}
	// 1d)
	else if (regex_match(rightString,quotesIndent))
	{
		// pass and store type for comparison
		rightAttr = AttrType::stringType;
		rightValid = true;
		rightSynt = SyntType::expression;
		removeCharsFromString(rightString, "\"");
	}
	// 1e) FAILS
	else
	{
		//fail 
	}

	if (leftValid == true && rightValid == true)
	{
		if (leftAttr == rightAttr)
		{
			isValid = true;
			ParamNode* leftParamNode  = new ParamNode(leftSynt,leftAttr,leftString);
			ParamNode* rightParamNode = new ParamNode(rightSynt,rightAttr,rightString);
			QueryPart* qp = new QueryPart(QueryType::with,leftParamNode,rightParamNode,NULL);
			qo.queryVec.push_back(qp);
		}
	}
	return isValid;
}


SyntType getSynType (string synType)
{
	regex doubleQuotes ("\"[^\"]+\"");
	regex underScoreBothSides("_\"[^\"]+\"_");
	regex underScoreOnly("_");

	regex integer ("\\d+");
	SyntType toReturn = SyntType::synError;

	if (regex_match(synType,doubleQuotes)|| regex_match(synType,underScoreBothSides))
	{
		toReturn = SyntType::expression;
	}
	else if (regex_match(synType,underScoreOnly))
	{
		toReturn = SyntType::expression;
	}
	else if (regex_match(synType,integer))
	{
		toReturn = SyntType::integer;
	}
	else
	{
		if(newSymbol->exists(synType))
		{
			toReturn = newSymbol->getSyntType(synType);
		}
	}
	return toReturn;
}
AttrType getAttrType(SyntType syntType)
{
	if (syntType == SyntType::integer || syntType == SyntType::progline || syntType == SyntType::constant)
	{
		return AttrType::integerType;
	}
	else 
	{
		return AttrType::stringType;
	}
}
QueryType getType (char* queryType)
{
	if (strcmp(queryType,"Uses") == 0)
	{
		return QueryType::uses;
	}
	else if (strcmp(queryType,"Modifies") == 0)
	{
		return QueryType::modifies;
	}
	else if (strcmp(queryType,"Calls") == 0)
	{
		return QueryType::calls;
	}
	else if (strcmp(queryType,"Calls*") == 0)
	{
		return QueryType::callsStar;
	}
	else if (strcmp(queryType,"Parent") == 0)
	{
		return QueryType::parent;
	}
	else if (strcmp(queryType,"Parent*") == 0)
	{
		return QueryType::parentStar;
	}
	else if (strcmp(queryType,"Follows") == 0)
	{
		return QueryType::follows;
	}
	else if (strcmp(queryType, "Follows*") == 0)
	{
		return QueryType::followsStar;
	}
	else if (strcmp(queryType, "Next") == 0)
	{
		return QueryType::next;
	}
	else if (strcmp(queryType, "Next*") == 0)
	{
		return QueryType::nextStar;
	}
	else if (strcmp(queryType, "Affects") == 0)
	{
		return QueryType::affects;
	}
	else if (strcmp(queryType, "Affects*") == 0)
	{
		return QueryType::affectsStar;
	}
	else if (strcmp(queryType, "with") == 0)
	{
		return QueryType::with;
	}
	else return QueryType::error;
}






void ProcessEachToken(char *currentToken)
{
	if (strcmp(currentToken,"Select") == 0)
	{
		currToken = TOKEN::RESULT_CL;
	}
	else if (expectingThat && strcmp(currentToken, "that") == 0)
	{
		currToken = TOKEN::SUCHTHAT_CL;
		existClauses = true;
		expectingThat = false;
	}
	else if (strcmp(currentToken,"with") == 0 && (currToken == TOKEN::AND_END || currToken == TOKEN::RESULT_CL))
	{
		currToken = TOKEN::WITH_CL;
		existWith = true;
	}
	else if (strcmp(currentToken,"pattern") == 0)
	{
		if (currToken != TOKEN::RESULT_CL)
		{
			concatStmt = "";
		}
		currToken = TOKEN::PATTERN_CL;
		existClauses = true;
	}
	else if (strcmp(currentToken,"such") == 0)
	{
		expectingThat = true;
	}
	else if (expectingThat == true && strcmp(currentToken,"that") != 0)
	{
		invalidSuchThat = true;
		existClauses = true;
	}
	else{

		switch(currToken)
		{
		case TOKEN::WITH_CL:
			{
				concatStmt.append(currentToken);
				regex leftHand ("[^\.]+\.[^\.]+=[^\.]+\.[^\.]+");
				regex withClause ("[^=]+=[^=]+");

				if (regex_match(concatStmt,withClause))
				{
					std::vector<char> writable(concatStmt.begin(), concatStmt.end());
					writable.push_back('\0');
					vector<string> attrVariables = split(&writable[0],'=');
					string leftAtt = attrVariables[0];
					string rightAtt = attrVariables[1];
					concatStmt = "";
					if (isCorrectWithClause(leftAtt,rightAtt) == true)
					{
						validWith = true;	
						currToken = TOKEN::AND_END;
						previousToken = TOKEN::WITH_CL;
					};
				}

			}break;
		case TOKEN::AND_END:
			{
				if (strcmp(currentToken,"and") == 0)
				{
					currToken = previousToken ;
					// For future Iteration
				}
				else if (strcmp(currentToken,"such")==0)
				{
					expectingThat = true;
				}
				else if (strcmp(currentToken,"with")==0)
				{
					currToken = TOKEN::WITH_CL;
				}
				else
				{
					nonQueryToken = true;
				}

			}break;
		case TOKEN::PATTERN_CL: 
			{  // assign a;
				// Select a pattern a(_, _"x + 1"_)
				patternExist = true;
				patternPass = false;
				regex fullPattern("([^\\(]+\\(([^\\)]+|[^\,]+)(\,([^\\)]+|[^\,]+)\\))+)");

				concatStmt.append(currentToken);

				if (regex_match(concatStmt,fullPattern))
				{
					string supposedSynonym = "";
					SyntType patternSyn;
					string firstParameter = "";
					string secondParameter = "";
					string thirdParameter = "";

					bool inParameters = false;
					bool passedFirstParam = false;
					bool passedSecondParam = false;
					bool ifSyn = false;

					for(std::string::size_type i = 0; i < concatStmt.size(); ++i) {
						if (concatStmt[i] == '(')
						{
							// Begin getting the parameters
							bool isSynonymExist = newSymbol->exists(supposedSynonym);

							if (!isSynonymExist)
							{
								//TODO THROW ERROR because no such synonym in symbol table
								nonExistantSyn = true;
								break;
							}
							else
							{
								patternSyn = newSymbol->getSyntType(supposedSynonym);
								if (patternSyn != SyntType::assignment || patternSyn != SyntType::whileLoop || patternSyn != SyntType::ifelse )
								{
									// TODO THROW ERROR because pattern only accepts three types of synonyms
								}
								if (patternSyn == SyntType::ifelse)
								{
									ifSyn = true;
								}
							}
							inParameters = true;
						}
						else if (concatStmt[i] == ')')
						{
							concatStmt = "";
						}
						else if (inParameters && concatStmt[i] != ',')
						{
							if (!ifSyn)
							{
								if(passedFirstParam) 
								{
									secondParameter += concatStmt[i];
								}
								else {firstParameter += concatStmt[i]; }
							}
							else
							{
								if(!passedFirstParam)
								{
									firstParameter += concatStmt[i];
								}
								else if (!passedSecondParam)
								{
									secondParameter += concatStmt[i];
								}
								else 
								{
									thirdParameter += concatStmt[i];
								}

							}
						}
						else if (concatStmt[i] == ',')
						{
							if (!passedFirstParam)
							{
								passedFirstParam = true;
							}
							else
							{
								passedSecondParam = true;
							}
						}
						else if (!inParameters)
						{
							supposedSynonym += concatStmt[i]; 
						}
					}

					if (verifyCorrectParameters(patternSyn,firstParameter,secondParameter,thirdParameter))
					{
						//ParamNode* leftParamNode = new ParamNode(patternSyn,supposedSynonym);
						AttrType leftString = getAttrType(patternSyn);
						ParamNode* leftParamNode = new ParamNode(patternSyn, leftString, supposedSynonym);
						SyntType rightParamSynType = getSynType(firstParameter);
						SyntType lastParamSynType = getSynType(secondParameter);
						removeCharsFromString( secondParameter, "\"" );
						removeCharsFromString(firstParameter, "\""); 
						AttrType rightAttr = getAttrType(rightParamSynType);
						AttrType lastAttr = getAttrType(lastParamSynType);
						//ParamNode* middleParamNode = new ParamNode (rightParamSynType, firstParameter);
						//ParamNode* rightParamNode = new ParamNode (lastParamSynType,secondParameter);
						ParamNode* rightParamNode = new ParamNode (rightParamSynType,rightAttr,firstParameter);
						ParamNode* lastParamNode = new ParamNode (lastParamSynType,lastAttr, secondParameter);
						//PatternNode* newPattern = new PatternNode(leftParamNode,rightParamNode, lastParamNode);
						//rootTree->addPattern(newPattern);
						QueryPart* newQueryPart = new QueryPart(QueryType::pattern,leftParamNode,rightParamNode,lastParamNode);
						qo.queryVec.push_back(newQueryPart);
						patternPass = true;
						currToken = TOKEN::AND_END;
						previousToken = TOKEN::PATTERN_CL;
					}
				}
				else
				{
					// TODO THROW ERROR because pattern is in the wrong syntax (doesn't match regex)
				}


			}
			break;
		case TOKEN::RESULT_CL: 
			{

				regex tuple("<[^>]+>");
				regex boolean("BOOLEAN");
				// 
				if (regex_match(currentToken,tuple))
				{
					string strToken = currentToken;
					removeCharsFromString( strToken, "<>" );
					std::vector<char> writable(strToken.begin(), strToken.end());
					writable.push_back('\0');
					vector<string> variablesTuple = split(&writable[0],',');

					// do for loop for each variable to see if it exists in the symbol table;
					// code over here
					for(std::vector<string>::iterator i = variablesTuple.begin(); i != variablesTuple.end(); i++)
					{
						if (!newSymbol->exists(*i)) {
							tupleError = true; break;
						} // TODO THROW ERROR because variable doesn't exist
						else {
							SyntType newSyntSymbol = newSymbol->getSyntType(*i);
							if (newSyntSymbol != SyntType::synError)
							{
								ParamNode* newResultSynonym = new ParamNode(newSyntSymbol,getAttrType(newSyntSymbol),*i);
								qo.resultVec.push_back(newResultSynonym);
								//newSymbol->setResult(newSymbol->getIndex(currentToken));
							}
						}
					}
					concatStmt = "";

				}
				else if (regex_match(currentToken, boolean))
				{
					resultBool = true;
					concatStmt = "";
					//qo.resultVec.push_back(new ParamNode());
				}
				else if (regex_match(currentToken,attrRef))
				{
					std::string currString = std::string(currentToken);
					std::vector<char> writable(currString.begin(), currString.end());
					writable.push_back('\0');
					vector<string> attrVariables = split(&writable[0],'.');

					SyntType currSynt = SyntType::synError;
					AttrType currAttr = AttrType::integerType;

					// 1ai. Check if leftHand's synonym exists and then get the SyntType
					if(newSymbol->exists(attrVariables[0]))
					{
						currSynt =  newSymbol->getSyntType(attrVariables[0]);
						// 1aii. Check if leftHand's synonym is a statement, then check if leftHand attribute is stmt#
						if (currSynt == SyntType::statementList||currSynt == SyntType::statement || currSynt == SyntType::assignment || currSynt == SyntType::call || currSynt == SyntType::whileLoop || currSynt == SyntType::ifelse)
						{
							// left hand pass
							const char * leftHandChar = attrVariables[1].c_str();
							if (strcmp(leftHandChar,"stmt#") == 0)
							{
								// pass ( .stmt#) and set AttrType to integer
								currAttr = AttrType::integerType;
								ParamNode* newParamNode = new ParamNode(currSynt,currAttr,leftHandChar);
								qo.resultVec.push_back(newParamNode);
							}
							else
							{
								nonExistantSyn = true;
								// fail (this statement has a dot(.) something else besides stmt#)
							}
						}
						// 1aiii. Check if leftHand's synonym is constant/variable, then check if leftHand attribute is value
						else if (currSynt == SyntType::constant)
						{
							const char * leftHandChar = attrVariables[1].c_str();
							if (strcmp(leftHandChar,"value") == 0)
							{
								// pass ( .value) and set AttrType to integer
								currAttr = AttrType::integerType;
								ParamNode* newParamNode = new ParamNode(currSynt,currAttr,leftHandChar);
								qo.resultVec.push_back(newParamNode);
							}
							else
							{
								nonExistantSyn = true;
								// fail (this statement has a dot(.) something else besides value)
							}
						}
						else if (currSynt == SyntType::procedure || currSynt == SyntType::call)
						{
							const char * leftHandChar = attrVariables[1].c_str();
							if (strcmp(leftHandChar, "procName") == 0)
							{
								currAttr = AttrType::stringType;
								ParamNode* newParamNode = new ParamNode(currSynt,currAttr,leftHandChar);
								qo.resultVec.push_back(newParamNode);
							}
							else
							{
								nonExistantSyn = true;
								// fail (this statement has a dot(.) something else besides procName
							}
						}
						else if (currSynt == SyntType::variable)
						{				
							const char * leftHandChar = attrVariables[1].c_str();
							if (strcmp(leftHandChar, "varName") == 0)
							{
								currAttr = AttrType::stringType;
								ParamNode* newParamNode = new ParamNode(currSynt,currAttr,leftHandChar);
								qo.resultVec.push_back(newParamNode);
							}
							else
							{
								nonExistantSyn = true;
								// fail (this statement has a dot(.) something else besides procName
							}

						}

					} 
					else
					{
						nonExistantSyn = true;
						//Synonym does not exist in Symbol table, fail!
					}

				}// end of checking left hand for synonym.attributeName
				else
				{
					if (!newSymbol->exists(currentToken)) 
					{ 
						nonExistantSyn = true; 
						concatStmt = "";
						errorMsg += ("- Synonym \"%s\" after Select statement is not declared." , currentToken);
					} //TODO THROW ERROR because variable doesn't exist
					else
					{							
						SyntType newSyntSymbol = newSymbol->getSyntType(currentToken);
						if (newSyntSymbol != SyntType::synError)
						{
							ParamNode* newResultSynonym = new ParamNode(newSyntSymbol,getAttrType(newSyntSymbol),currentToken);
							qo.resultVec.push_back(newResultSynonym);
						}
					}
				}


				completeSelectStmt = true;
			}break;

		case TOKEN::SUCHTHAT_CL: 
			{ // 
				regex fullPattern("([^\\(]+\\(([^\\)]+|[^\,]+)\,([^\\)]+|[^\,]+)\\))");
				suchThatQueryExist = true;
				suchThatQueryPass = false;

				concatStmt.append(currentToken);

				if (regex_match(concatStmt,fullPattern))
				{
					// means Modifies(..,...)
					string supposedRelationEntity = "";
					QueryType queryClause;
					string firstParameter = "";
					string secondParameter = "";

					bool inParameters = false;
					bool passedFirstParam = false;

					for(std::string::size_type i = 0; i < concatStmt.size(); ++i) {
						if (concatStmt[i] == '(')
						{
							// Begin getting the parameters
							std::vector<char> writable(supposedRelationEntity.begin(), supposedRelationEntity.end());
							writable.push_back('\0');
							queryClause = getType(&writable[0]);
							if (queryClause == QueryType::error)
							{
								//TODO THROW ERROR because no such such that clause (e.g. Modifies/Calls mispelled)
								break;
							}
							inParameters = true;
						}
						else if (concatStmt[i] == ')')
						{
							concatStmt = "";
						}
						else if (inParameters && concatStmt[i] != ',')
						{
							if(passedFirstParam) 
							{
								secondParameter += concatStmt[i];
							}
							else {firstParameter += concatStmt[i]; }
						}
						else if (concatStmt[i] == ',')
						{
							passedFirstParam = true;
						}
						else if (!inParameters)
						{
							supposedRelationEntity += concatStmt[i]; 
						}
					}
					// Now check if first param and second param are valid.
					// And create a querynode and add to the query tree
					if (verifyCorrectParameters(queryClause,firstParameter,secondParameter))
					{
						SyntType firstSyn = getSynType(firstParameter);
						SyntType secondSyn = getSynType(secondParameter);
						if (firstSyn != SyntType::synError && secondSyn != SyntType::synError)
						{
							removeCharsFromString(firstParameter, "\"");
							removeCharsFromString(secondParameter, "\"");
							AttrType firstAttr = getAttrType(firstSyn);
							AttrType secondAttr = getAttrType(secondSyn);
							ParamNode* firstParamNode = new ParamNode(firstSyn,firstAttr,firstParameter);
							ParamNode* secondParamNode = new ParamNode(secondSyn,secondAttr,secondParameter);
							ParamNode* thirdParamNode = NULL;
							//QueryNode* newQuery = new QueryNode(queryClause,firstParamNode, secondParamNode);
							QueryPart* newQueryPart = new QueryPart(queryClause,firstParamNode,secondParamNode,thirdParamNode);
							qo.queryVec.push_back(newQueryPart);
							//rootTree->addQuery(newQuery);
							suchThatQueryPass = true;
							currToken = TOKEN::AND_END;
							previousToken = TOKEN::SUCHTHAT_CL;
						}			

					}

				}
				// Else the such that clause doesn't match the regex, thus THROW ERROR
				else
				{
					//TODO THROW ERROR because the such that clause doesn't match the regex
				}

			}break;

		} // end of switch
	}

}

void Match (char *c)
{
	regex re(" *(procedure|stmtLst|stmt|assign|call|while|if|variable|constant|prog_line) +([a-zA-Z])+(\\d|#)*( *, *[a-zA-Z]+(\\d|#)*)* *");
	regex sel(" *Select *.*");

	if (regex_match(c,re))
	{
		int count =0;
		char *pch;
		char *end_token;
		pch = strtok_s (c," ,",&end_token);
		bool entityTypeFound = false;
		SyntType newSyntType = SyntType::synError;

		while (pch != NULL)
		{
			//printf ("Split design entity ',' : %s\n",pch);

			if (!entityTypeFound)
			{
				if (strcmp(pch,"while") == 0)
				{
					newSyntType= SyntType::whileLoop;
				}
				else if (strcmp(pch,"assign") == 0)
				{
					newSyntType = SyntType::assignment;
				}
				else if (strcmp(pch,"procedure") == 0)
				{
					newSyntType = SyntType::procedure;
				}
				else if (strcmp(pch,"stmtLst") == 0)
				{
					newSyntType = SyntType::statementList;
				}
				else if (strcmp(pch,"stmt") == 0 )
				{
					newSyntType = SyntType::statement;
				}
				else if (strcmp(pch,"call") == 0)
				{
					newSyntType = SyntType::call;
				}
				else if (strcmp(pch,"if") == 0)
				{
					newSyntType = SyntType::ifelse;
				}
				else if (strcmp(pch,"variable") == 0)
				{
					newSyntType = SyntType::variable;
				}
				else if (strcmp(pch,"constant") == 0)
				{
					newSyntType = SyntType::constant;
				}
				else if (strcmp(pch,"prog_line") == 0)
				{
					newSyntType = SyntType::progline;
				}
				entityTypeFound = true;
			}
			else
			{
				++count;
				// store each variable in the symbol table
				newSymbol->setVar(pch,newSyntType);

			}
			pch = strtok_s (NULL, " ,",&end_token);
		}
		--count;
		// count number of variables
	}
	else if (regex_match(c,sel))
	{
		char *pch;
		char *end_token;
		pch = strtok_s (c," ",&end_token);

		while (pch != NULL)
		{
			//printf ("For each word in select statement : %s\n",pch);
			ProcessEachToken(pch);
			pch = strtok_s (NULL, " ",&end_token);
		}
	}
	else
	{
		synonymError = true;
		errorMsg += "Syntax error in synonym declaration!;\n";
	}

}

Query_Object QueryParser::getQueryObject(std::string i){
	const char* s = ";";
	char *end_str;
	char *token;
	//string concatStmt = "";
	//you need to reset the original global variable
	concatStmt = "";
	qo.queryVec.clear();
	qo.resultVec.clear();

	//rootTree = new QueryTree();
	newSymbol = new Symbol();
	expectingThat = false;
	resultBool = false;
	tupleError = false;
	nonExistantSyn = false;
	suchThatQueryExist = false;
	patternExist = false;
	suchThatQueryPass = false;
	patternPass = false;
	patternPass = false;
	nonQueryToken = false;
	existClauses = false;
	validWith = false;
	existWith = false;
	synonymError = false;
	completeSelectStmt = false;

	char *a=new char[i.size()+1];
	a[i.size()]=0;
	memcpy(a,i.c_str(),i.size());

	token = strtok_s(a,s,&end_str);

	/* walk through other tokens */

	while( token != NULL ) 
	{
		Match(token);
		token = strtok_s(NULL, s,&end_str);
		if (tupleError)
		{
			break;
		}
	}
	if (completeSelectStmt == false ||synonymError||(existWith && validWith == false)||nonQueryToken||tupleError||nonExistantSyn||(suchThatQueryPass == false && suchThatQueryExist) || (patternPass == false && patternExist)|| (existClauses && (!suchThatQueryExist && !patternExist)))
	{
		qo.isValid = false;
		return qo;
	}
	else // means there were no errors
	{
		qo.isValid = true;
		return qo;
	}

}

/*
int main ()
{
	QueryParser qp;
	string query;
	while (query!= "1")
	{
		getline (cin,query);

		Query_Object qs = qp.getQueryObject(query);
	}
}
*/



