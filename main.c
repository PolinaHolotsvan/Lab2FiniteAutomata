//Розробіть алгоритм та реалізуйте програму, що моделює роботу недетермінованого скінченного автомата.
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

struct TransRule {
    char letter;
    struct State *nextState;
};

struct State {
    char name;
    int ruleNumber;
    struct TransRule *rules;
};

struct State *findState(struct State *nodes, int nodesNumber, char name) {
    for (int i = 0; i < nodesNumber; i++) {
        if (nodes[i].name == name) return &nodes[i];
    }

    return NULL;
}

int getChars(const char *line, char *arr) {
    int number = 0;

    for (int i = 0; i < strlen(line) && line[i] != '\0'; i++) {
        if (line[i] != ' ' && line[i] != '\n') {
            arr[number++] = line[i];
        }
    }

    return number;
}

bool simulateNFA(struct State *currentState, char *word, char *alphabet, char *finalStates, int finalStatesNumber) {
    if (*word == '\0') {
        for (int i = 0; i < finalStatesNumber; i++) {
            if (finalStates[i] == currentState->name) {
                return true;
            }
        }
        return false;
    }

    for (int i = 0; i < currentState->ruleNumber; i++) {
        struct TransRule rule = currentState->rules[i];
        if (rule.letter == *word || rule.letter == alphabet[26]) {
            if (simulateNFA(rule.nextState, word + 1, alphabet, finalStates, finalStatesNumber)) {
                return true;
            }
        }
    }

    return false;
}

bool isLetterInAlphabet(char letter, char *alphabet) {
    while (*alphabet != '\0') {
        if (*alphabet == letter) {
            return true;
        }
        alphabet++;
    }
    return false;
}

int main() {
    FILE *file;
    int bufferLength = 100;
    char line[bufferLength];

    char alphabet[27];
    char stateNames[bufferLength / 2];
    int statesNumber;
    char startStateName;
    char finalStateNames[bufferLength / 2];
    int finalStateNumber;
    char ruleChars[3];

    char word[100];

    file = fopen("file.txt", "r+");
    if (file == NULL) {
        printf("File opening error\n");
        return 1;
    }


    //зчитування алфавіту
    fgets(line, bufferLength, file);
    getChars(line, alphabet);


    //зчитування назв станів та їхнє створення
    fgets(line, bufferLength, file);
    statesNumber = getChars(line, stateNames);

    struct State states[statesNumber];
    for (int i = 0; i < statesNumber; i++) {
        struct State newNode = {.name = stateNames[i], .ruleNumber=0};
        states[i] = newNode;
    }


    //зчитування початкового стану
    fgets(line, bufferLength, file);
    startStateName = line[0];
    if(!isLetterInAlphabet(startStateName, stateNames)){
        printf("Invalid start state\n");
        return 1;
    }


    //зчитування кінцевих станів
    fgets(line, bufferLength, file);
    finalStateNumber = getChars(line, finalStateNames);

    for(int i=0; i<finalStateNumber; i++){
        if(!isLetterInAlphabet(finalStateNames[i], stateNames)){
            printf("Invalid final states\n");
            return 1;
        }
    }


    //зчитування правил
    while (fgets(line, bufferLength, file)) {
        getChars(line, ruleChars);

        struct State *nextState = findState(states, statesNumber, ruleChars[2]);
        struct TransRule newRule = {.letter=ruleChars[1], .nextState=nextState};
        struct State *prevState = findState(states, statesNumber, ruleChars[0]);

        if (prevState != NULL && nextState != NULL && isLetterInAlphabet(ruleChars[1], alphabet)) {
            prevState->rules = (struct TransRule *) realloc(prevState->rules,
                                                            (prevState->ruleNumber + 1) * sizeof(struct TransRule));
            if (prevState->rules == NULL) {
                printf("Memory allocation failed\n");
                exit(1);
            }
        } else {
            printf("Invalid rule\n");
            exit(1);
        }

        prevState->rules[prevState->ruleNumber] = newRule;
        prevState->ruleNumber++;
    }


    printf("Enter your word\n");
    scanf("%s", word);


    struct State *startState = findState(states, statesNumber, startStateName);


    bool result = simulateNFA(startState, word, alphabet, finalStateNames, finalStateNumber);

    if (result == false) {
        printf("The word was rejected\n");
    } else {
        printf("The word was accepted\n");
    }


    for (int i = 0; i < statesNumber; i++) {
        free(states[i].rules);
    }

    fclose(file);

    return 0;
}
