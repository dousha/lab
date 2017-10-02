// calc.c
// author: dousha
// It works anyway, no 5h17 given about performance.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>

int scan(const char* exp){
	// this is about to scan if there's any lexical error
	// returns 1 if found any
	int count = 0;
	for(int i = 0; i < strlen(exp); i++){
		if(exp[i] == '('){
			++count;
			continue;
		}
		if(exp[i] == ')'){
			--count;
			continue;
		}
	}
	return count; // should be zero if nothing goes wrong
}

double calc(const char op, double left, double right){
	switch(op){
		case '+':
			return left + right;
		case '-':
			return left - right;
		case '*':
			return left * right;
		case '/':
			return left / right;
		default:
			return NAN;
	}
}

void strrcpy(const char* src, char* dest, size_t from, size_t to){
	if(from >= to) return;
	if(to > strlen(src)) to = strlen(src);
	for(size_t i = from; i < to; i++){
		dest[i - from] = src[i];
	}
	dest[to - from] = 0;
}

uint8_t isNumber(const char* str, size_t from, size_t to){
	// matches:
	// [+-]?\.?([0-9]+)(\.?[0-9]+)?([eE]?[+-]?[0-9]+)?
	if(strlen(str) == 0) return 0;
	size_t i = 0;
	uint8_t hasE = 0, hasSign = 0, hasPrefix = 0, hasDecimal = 0;
	if(to > strlen(str)) to = strlen(str);
	if(str[from] == '+' || str[from] == '-') hasPrefix = 1;
	else if(str[from] == '.') hasDecimal = 1;
	else if(!isdigit(str[from])) return 0;
	for(i = 1; i < to; i++){
		if(str[i] == '.'){
			if(hasDecimal) return 0;
			else hasDecimal = 1;
		}else if(str[i] == 'e' || str[i] == 'E'){
			if(hasE) return 0;
			else hasE = 1;
		}else if(str[i] == '+' || str[i] == '-'){
			if(hasE && (str[i - 1] == 'e' || str[i - 1] == 'E')) hasSign = 1;
			else return 0;
		}else if(!isdigit(str[i])) return 0;
	}
	return 1;
}

double eval(const char* exp){
	// dbg
	printf("Evaluating: %s\n", exp);
	// number literal?
	if(isNumber(exp, 0, strlen(exp))){
		return strtod(exp, NULL);
	}
	// continue parsing
	size_t cpl = 0, cpr = strlen(exp), count = 0;
	uint8_t hasBracket = 0;
	if(exp[0] == '(' && exp[cpr - 1] == ')'){
		// test if it has redundant outer bracket
		size_t count = 0, i = 0;
		for(i = cpl; i < cpr; i++) {
			if (exp[i] == '(') {
				++count;
				continue;
			}
			if (exp[i] == ')') {
				if (--count == 0) {
					break;
				}
			}
		}
		if(i == cpr - 1){
			// strip out outer bracket
			// dbg
			printf("Stripped outer bracket.\n");
			++cpl;
			--cpr;
			// number literal?
			if (isNumber(exp, cpl, cpr)) {
				char *trimmed = malloc(sizeof(char) * strlen(exp));
				strrcpy(exp, trimmed, cpl, cpr);
				double res = strtod(trimmed, NULL);
				free(trimmed);
				return res;
			}
		}else {
			printf("Not a simple expression.\n");
		}
	}
	// not a number literal
	// then, get the +-
	size_t cpm = cpl;
	for(cpm = cpl; cpm < cpr; cpm++){
		// skip blocks
		if(exp[cpm] == '('){
			size_t count = 0;
			for(; cpm < cpr; cpm++){
				if(exp[cpm] == '('){
					++count;
					continue;
				}
				if(exp[cpm] == ')'){
					if(--count == 0) break;
				}
			}
		}
		if(exp[cpm] == '+' || exp[cpm] == '-'){
			char* lstr = malloc(sizeof(char) * cpm);
			char* rstr = malloc(sizeof(char) * (cpr - cpm + 1));
			strrcpy(exp, lstr, cpl, cpm);
			strrcpy(exp, rstr, cpm + 1, cpr);
			double lval = eval(lstr);
			double rval = eval(rstr);
			free(lstr);
			free(rstr);
			return calc(exp[cpm], lval, rval);
		}
	}
	// again, for * and / this time
	for(cpm = cpl; cpm < cpr; cpm++){
		// skip blocks
		if(exp[cpm] == '('){
			size_t count = 0;
			for(; cpm < cpr; cpm++){
				if(exp[cpm] == '('){
					++count;
					continue;
				}
				if(exp[cpm] == ')'){
					if(--count == 0) break;
				}
			}
		}
		if(exp[cpm] == '*' || exp[cpm] == '/'){
			char* lstr = malloc(sizeof(char) * cpm);
			char* rstr = malloc(sizeof(char) * (cpr - cpm + 1));
			strrcpy(exp, lstr, cpl, cpm);
			strrcpy(exp, rstr, cpm + 1, cpr);
			double lval = eval(lstr);
			double rval = eval(rstr);
			free(lstr);
			free(rstr);
			return calc(exp[cpm], lval, rval);
		}
	}
	// something's wrong
	return NAN;
}

int main(){
	const char separator = ',';
	char* input = malloc(sizeof(char) * 128);
	printf("Input an expression:\n");
	fgets(input, 128, stdin);
	if(scan(input)){
		printf("Invalid expression. Exiting.");
		free(input);
	}else{
		char *exp = malloc(sizeof(char) * 128);
		for (int i = 0, j = 0; i < strlen(input); i++) {
			if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n' || input[i] == separator) continue;
			exp[j] = input[i];
			++j;
		} // this filter out whitespaces and separators
		free(input);
		printf("Answer is %lf", eval(exp));
		free(exp);
	}
	return 0;
}
