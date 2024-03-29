/*
Group - 29

Shaurya Marwah - 2019B3A70459P
Hari Sankar - 2019B3A70564P
Ruchir Kumbhare - 2019B5A70650P
Ashwin Murali - 2019B2A70957P
Dilip Venkatesh - 2020A7PS1203P

*/

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "lexerDef.h"
#include "tree.h"
#include "parser.h"
#include "parserDef.h"
#include "ast.h"
#include "symbolTableDef.h"
#include "astDef.h"
#include "typeCheckerDef.h"
#include "symTableUtil.h"
#include "intermedCodeGen.h"
#include "intermCodeGenDef.h"
#include <string.h>

int labelCount = 0;
int tempCount = 0;

quadruple *globalHead = NULL;

int newTemp()
{
    return ++tempCount;
}

int newLabel()
{
    return ++labelCount;
}

quadruple *stmtsCodeGen(astNode *stmts, SymTablePointer *symTable);

quadruple *initQuadruple()
{
    quadruple *qp = malloc(sizeof(quadruple));
    qp->next = NULL;
    memset(qp->operand1, '\0', sizeof(qp->operand1));
    memset(qp->operand2, '\0', sizeof(qp->operand2));
    memset(qp->resultant, '\0', sizeof(qp->resultant));
    qp->op = -1;
    qp->offsetOperand1 = -1;
    qp->offsetOperand2 = -1;
    qp->offsetRes = -1;
    return qp;
}

// void getAttributeCodeExpressions(astNode *node, SymTablePointer *symTable)
// {
//     switch (node->label)
//     {
//     case AST_PLUS:
//     case AST_MINUS:
//     case AST_DIV:
//     case AST_MUL:
//     {
//         int temp = newTemp();
//         snprintf(node->name, "0temp%d", temp);
//         // symbol table??
//         getAttributeCodeExpressions(node->leftChild, symTable);
//         getAttributeCodeExpressions(node->leftChild->nextSibling, symTable);
//         // append the codes together
//         node->leftChild->code->next = node->leftChild->nextSibling->code;
//         quadruple *qp = initQuadruple();
//         strcpy(qp->operand1, node->leftChild->name);
//         strcpy(qp->operand2, node->leftChild->nextSibling->name);
//         strcpy(qp->resultant, node->name);
//         node->leftChild->nextSibling->code->next = qp;
//         qp->next = NULL;
//         if (node->label == AST_PLUS)
//         {
//             qp->op = OP_PLUS;
//         }
//         else if (node->label == AST_MINUS)
//         {
//             qp->op = OP_MINUS;
//         }
//         else if (node->label == AST_MUL)
//         {
//             qp->op = OP_MUL;
//         }
//         else
//         {
//             qp->op = OP_DIV;
//         }
//         return;
//     }
//     case AST_ID:
//     {
//         strcpy(node->name, node->tk->str);
//         return;
//     }
//     case AST_SIGNED:
//     {
//         getAttributeCodeExpressions(node->leftChild->nextSibling);
//         if (node->leftChild->tk->token == MINUS)
//         {
//             int temp = newTemp();
//             snprintf(node->name, "0temp%d", temp);
//             quadruple *qp = initQuadruple();
//             qp->op = U_MINUS;
//             strcpy(qp->operand1, node->leftChild->nextSibling->name);
//             strcpy(qp->resultant, node->name);
//             node->leftChild->nextSibling->code->next = qp;
//             qp->next = NULL;
//             node->code = node->leftChild->nextSibling->code;
//         }
//         else
//         {
//             strcpy(node->name, node->leftChild->nextSibling->name);
//             node->code = node->leftChild->nextSibling->code;
//         }
//         return;
//     }
//     case AST_NUM:
//     {
//         snprintf(node->name, "%d", node->tk->integer);
//         return;
//     }
//     case AST_RNUM:
//     {
//         snprintf(node->name, "%ld", node->tk->rnum);
//         return;
//     }
//     case AST_BOOL:
//     {
//         sprintf(node->name, "%s", node->tk->str);
//         return;
//     }
//     }
// }

quadruple *appendAtEnd(quadruple *head, quadruple *append)
{
    if (head == NULL)
    {
        return append;
    }
    quadruple *temp = head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = append;
    return head;
}

SymTablePointer *getModulesSymTable(SymTablePointer *symTable)
{
    while (symTable->typeST != MODULEST)
    {
        symTable = symTable->parentHashTable;
    }
    return symTable;
}

int updateOffsets(char *str, SymTablePointer *symTable, types type)
{
    if (existsInAnySymTable(symTable, str))
    {
        return getFromAnySymTable(symTable, str)->offset;
    }
    SymTablePointer *ptrnewT = initSymTablePointer();
    ptrnewT->str = str;
    SymTablePointer *modulesSymTable = getModulesSymTable(symTable);
    int offset = modulesSymTable->activationRecordSize;
    ptrnewT->offset = offset;
    insertSymTable(symTable->corrHashtable, ptrnewT);
    if (type == TYPE_INTEGER)
    {
        ptrnewT->width = INT_WIDTH;
    }
    else if (type == TYPE_BOOLEAN)
    {
        ptrnewT->width = BOOL_WIDTH;
    }
    else if (type == TYPE_REAL)
    {
        ptrnewT->width = REAL_WIDTH;
    }
    modulesSymTable->activationRecordSize += ptrnewT->width;
    return ptrnewT->offset;
}

void getShortCircuitCode(astNode *expr, SymTablePointer *symTable)
{
    switch (expr->label)
    {
    }
}

void getExpressionsCode(astNode *expr, SymTablePointer *symTable)
{
    switch (expr->label)
    {
    case AST_OR:
    {
        astNode *b1 = expr->leftChild;
        astNode *b2 = expr->leftChild->nextSibling;
        b1->trueCase = expr->trueCase;
        b1->falseCase = newLabel();
        b2->trueCase = expr->trueCase;
        b2->falseCase = expr->falseCase;
        getExpressionsCode(b1, symTable);
        getExpressionsCode(b2, symTable);
        expr->code = b1->code;
        quadruple *qp = initQuadruple();
        qp->op = LABEL;
        sprintf(qp->operand1, "label%d", b1->falseCase);
        expr->code = appendAtEnd(expr->code, qp);
        qp->next = b2->code;
        return;
    }
    case AST_AND:
    {
        astNode *b1 = expr->leftChild;
        astNode *b2 = b1->nextSibling;
        b1->trueCase = newLabel();
        b1->falseCase = expr->falseCase;
        b2->trueCase = expr->trueCase;
        b2->falseCase = expr->falseCase;
        getExpressionsCode(b1, symTable);
        getExpressionsCode(b2, symTable);
        expr->code = b1->code;
        quadruple *qp = initQuadruple();
        qp->op = LABEL;
        sprintf(qp->operand1, "label%d", b1->trueCase);
        expr->code = appendAtEnd(expr->code, qp);
        qp->next = b2->code;
        return;
    }
    case AST_GE:
    case AST_LE:
    case AST_LT:
    case AST_GT:
    case AST_NE:
    case AST_EQ:
    {
        astNode *e1 = expr->leftChild;
        astNode *e2 = e1->nextSibling;
        getExpressionsCode(expr->leftChild, symTable);
        getExpressionsCode(expr->leftChild->nextSibling, symTable);
        expr->code = appendAtEnd(expr->code, expr->leftChild->code);
        expr->code = appendAtEnd(expr->code, expr->leftChild->nextSibling->code);
        int temp = newTemp();
        quadruple *qp = initQuadruple();
        sprintf(qp->resultant, "temp%d", temp);
        int offset = updateOffsets(qp->resultant, symTable, TYPE_BOOLEAN);
        qp->offsetRes = offset;
        sprintf(qp->operand1, "%s", e1->name);
        if (e1->label != AST_NUM && e1->label != AST_RNUM && e1->label != AST_BOOL)
        {
            qp->offsetOperand1 = getFromAnySymTable(symTable, e1->name)->offset;
            qp->op1Ptr = getFromAnySymTable(symTable, e1->name);
        }
        sprintf(qp->operand2, "%s", e2->name);
        if (e2->label != AST_NUM && e2->label != AST_RNUM && e2->label != AST_BOOL)
        {
            qp->offsetOperand2 = getFromAnySymTable(symTable, e2->name)->offset;
            qp->op2Ptr = getFromAnySymTable(symTable, e2->name);
        }
        if (expr->label == AST_EQ)
        {
            qp->op = OP_EQ;
        }
        else if (expr->label == AST_NE)
        {
            qp->op = OP_NE;
        }
        else if (expr->label == AST_GE)
        {
            qp->op = OP_GE;
        }
        else if (expr->label == AST_GT)
        {
            qp->op = OP_GT;
        }
        else if (expr->label == AST_LE)
        {
            qp->op = OP_LE;
        }
        else if (expr->label == AST_LT)
        {
            qp->op = OP_LT;
        }
        expr->code = appendAtEnd(expr->code, qp);
        // e2->code->next = qp;
        quadruple *qp1 = initQuadruple();
        qp1->op = JUMPIFTRUE;
        sprintf(qp1->operand1, "temp%d", temp);
        sprintf(qp1->operand2, "label%d", expr->trueCase);
        // qp->next = qp1;
        qp1->offsetOperand1 = getFromAnySymTable(symTable, qp1->operand1)->offset;
        qp1->op1Ptr = getFromAnySymTable(symTable, qp1->operand1);
        expr->code = appendAtEnd(expr->code, qp1);
        quadruple *qp2 = initQuadruple();
        qp2->op = JUMP;
        sprintf(qp2->operand1, "label%d", expr->falseCase);
        // qp1->next = qp2;
        expr->code = appendAtEnd(expr->code, qp2);
        return;
    }
    case AST_BOOL:
    {
        expr->code = initQuadruple();
        expr->code->op = JUMP;
        if (expr->tk->token == TRUE)
        {
            sprintf(expr->code->operand1, "label%d", expr->trueCase);
        }
        else
        {
            sprintf(expr->code->operand1, "label%d", expr->falseCase);
        }
        return;
    }
    case AST_ID:
    {
        strcpy(expr->name, expr->tk->str);

        return;
    }
    case AST_NUM:
    {
        sprintf(expr->name, "%d", expr->tk->integer);
        return;
    }
    case AST_RNUM:
    {
        sprintf(expr->name, "%f", expr->tk->rnum);
        return;
    }
    case AST_SIGNED:
    {
        getExpressionsCode(expr->leftChild->nextSibling, symTable);

        if (expr->leftChild->tk->token == MINUS)
        {
            quadruple *head = initQuadruple();
            head->op = OP_MINUS;
            sprintf(head->operand1, "%d", 0);
            sprintf(head->operand2, "%s", expr->leftChild->nextSibling->name);
            if (expr->leftChild->nextSibling->label != AST_NUM && expr->leftChild->nextSibling->label != AST_RNUM && expr->leftChild->nextSibling->label != AST_BOOL)
            {
                head->offsetOperand2 = updateOffsets(head->operand2, symTable, TYPE_INTEGER);
            }
            int newTem = newTemp();
            sprintf(head->resultant, "temp%d", newTem);
            head->offsetRes = updateOffsets(head->resultant, symTable, TYPE_INTEGER);
            sprintf(expr->name, "%s", head->resultant);
            expr->code = head;
        }
        else
            sprintf(expr->name, "%s", expr->leftChild->nextSibling->name);
        return;
    }
    case AST_PLUS:
    case AST_MINUS:
    case AST_MUL:
    case AST_DIV:
    {
        int newT = newTemp();
        sprintf(expr->name, "temp%d", newT);
        int offset = updateOffsets(expr->name, symTable, expr->type);
        astNode *e1 = expr->leftChild;
        astNode *e2 = e1->nextSibling;
        getExpressionsCode(e1, symTable);
        getExpressionsCode(e2, symTable);
        expr->code = appendAtEnd(expr->code, e1->code);
        expr->code = appendAtEnd(expr->code, e2->code);
        quadruple *qp = initQuadruple();
        if (expr->label == AST_PLUS)
            qp->op = OP_PLUS;
        else if (expr->label == AST_MINUS)
            qp->op = OP_MINUS;
        else if (expr->label == AST_MUL)
            qp->op = OP_MUL;
        else
            qp->op = OP_DIV;
        strcpy(qp->resultant, expr->name);
        qp->offsetRes = offset;
        qp->resPtr = getFromAnySymTable(symTable, qp->resultant);
        strcpy(qp->operand1, e1->name);
        if (expr->leftChild->label != AST_NUM && expr->leftChild->label != AST_RNUM && expr->leftChild->label != AST_BOOL)
        {
            qp->offsetOperand1 = getFromAnySymTable(symTable, qp->operand1)->offset;
            qp->op1Ptr = getFromAnySymTable(symTable, qp->operand1);
        }
        strcpy(qp->operand2, e2->name);
        if (expr->leftChild->nextSibling->label != AST_NUM && expr->leftChild->nextSibling->label != AST_RNUM && expr->leftChild->nextSibling->label != AST_BOOL)

        {
            qp->offsetOperand2 = getFromAnySymTable(symTable, qp->operand2)->offset;
            qp->op2Ptr = getFromAnySymTable(symTable, qp->operand2);
        }
        expr->code = appendAtEnd(expr->code, qp);
        return;
    }
    case AST_ARRAY_ELEMENT:
    {
        getExpressionsCode(expr->leftChild->nextSibling, symTable);
        quadruple *head = expr->leftChild->nextSibling->code;
        // i-low
        quadruple *first = initQuadruple();
        first->op = OP_MINUS;
        int firstTemp = newTemp();
        sprintf(first->resultant, "temp%d", firstTemp);
        first->offsetRes = updateOffsets(first->resultant, symTable, TYPE_INTEGER);
        sprintf(first->operand1, "%s", expr->leftChild->nextSibling->name);
        if (expr->leftChild->nextSibling->label != AST_NUM && expr->leftChild->nextSibling->label != AST_RNUM && expr->leftChild->nextSibling->label != AST_BOOL)
        {
            first->offsetOperand1 = updateOffsets(first->operand1, symTable, TYPE_INTEGER);
        }
        sprintf(first->operand2, "%d", getFromAnySymTable(symTable, expr->leftChild->tk->str)->typeIfArray.low);
        head = appendAtEnd(head, first);

        //(width)*firstTemp
        quadruple *second = initQuadruple();
        int secondTemp = newTemp();
        second->op = OP_MUL;
        sprintf(second->resultant, "temp%d", secondTemp);
        second->offsetRes = updateOffsets(second->resultant, symTable, TYPE_INTEGER);
        sprintf(second->operand1, "%d", INT_WIDTH);
        strcpy(second->operand2, first->resultant);
        second->offsetOperand2 = updateOffsets(second->operand2, symTable, TYPE_INTEGER);
        head = appendAtEnd(head, second);

        //+1
        quadruple *third = initQuadruple();
        int thirdTemp = newTemp();
        third->op = OP_PLUS;
        sprintf(third->resultant, "temp%d", thirdTemp);
        third->offsetRes = updateOffsets(third->resultant, symTable, TYPE_INTEGER);
        sprintf(third->operand1, "%s", second->resultant);
        third->offsetOperand1 = updateOffsets(third->operand1, symTable, TYPE_INTEGER);
        sprintf(third->operand2, "%d", 1);
        head = appendAtEnd(head, third);

        // add with base pointer's offset
        quadruple *fourth = initQuadruple();
        int fourthTemp = newTemp();
        fourth->op = OP_PLUS;
        sprintf(fourth->resultant, "temp%d", fourthTemp);
        fourth->offsetRes = updateOffsets(fourth->resultant, symTable, TYPE_INTEGER);
        sprintf(fourth->operand1, "%s", third->resultant);
        fourth->offsetOperand1 = updateOffsets(fourth->operand1, symTable, TYPE_INTEGER);
        sprintf(fourth->operand2, "%d", getFromAnySymTable(symTable, expr->leftChild->tk->str)->offset);
        head = appendAtEnd(head, fourth);

        // load from the array
        quadruple *fifth = initQuadruple();
        int fifthTemp = newTemp();
        fifth->op = LW;
        sprintf(fifth->resultant, "temp%d", fifthTemp);
        fifth->offsetRes = updateOffsets(fifth->resultant, symTable, getFromAnySymTable(symTable, expr->leftChild->tk->str)->typeIfArray.type);
        sprintf(fifth->operand1, "%s", expr->leftChild->tk->str);
        fifth->offsetOperand1 = updateOffsets(fifth->operand1, symTable, TYPE_INTEGER);
        sprintf(fifth->operand2, "%s", fourth->resultant);
        fifth->offsetOperand2 = updateOffsets(fifth->operand2, symTable, TYPE_INTEGER);
        head = appendAtEnd(head, fifth);

        sprintf(expr->name, "temp%d", fifthTemp);
        expr->code = head;
        return;
    }
    }
}

quadruple *generateWhileLoopCode(astNode *stmts)
{
    quadruple *head = initQuadruple();
    int label1 = newLabel();
    head->op = LABEL;
    sprintf(head->operand1, "label%d", label1);
    astNode *exprOnTop = stmts->leftChild;
    int label2 = newLabel();
    int label3 = newLabel();
    exprOnTop->trueCase = label2;
    exprOnTop->falseCase = label3;
    getExpressionsCode(exprOnTop, stmts->symTable);
    head->next = exprOnTop->code;
    quadruple *qp1 = initQuadruple();
    qp1->op = LABEL;
    sprintf(qp1->operand1, "label%d", label2);
    head = appendAtEnd(head, qp1);
    // stmts here
    quadruple *qp2 = initQuadruple();
    qp2->op = JUMP;
    sprintf(qp2->operand1, "label%d", label1);
    head = appendAtEnd(head, qp2);
    quadruple *qp3 = initQuadruple();
    qp3->op = LABEL;
    sprintf(qp3->operand1, "label%d", label3);
    head = appendAtEnd(head, qp3);
    return head;
}

quadruple *generateForLoopCode(astNode *stmts)
{
    quadruple *head = initQuadruple();
    SymTablePointer *symTable = stmts->symTable;
    astNode *loopVar = stmts->leftChild;
    astNode *range = stmts->leftChild->nextSibling;
    head->op = OP_ASSIGN;
    strcpy(head->resultant, loopVar->tk->str);
    head->offsetRes = updateOffsets(head->resultant, symTable, TYPE_INTEGER);
    // if signed??
    if (range->leftChild->isNegative)
    {
        snprintf(head->operand1, 25, "-%d", range->leftChild->tk->integer);
    }
    else
    {
        snprintf(head->operand1, 25, "%d", range->leftChild->tk->integer);
    }
    int label1 = newLabel();
    quadruple *tempQ1 = initQuadruple();
    tempQ1->op = LABEL;
    snprintf(tempQ1->operand1, 25, "label%d", label1);
    head = appendAtEnd(head, tempQ1);
    int temp0 = newTemp();
    quadruple *tempQ = initQuadruple();
    tempQ->op = OP_LE;
    strcpy(tempQ->operand1, loopVar->tk->str);
    tempQ->offsetOperand1 = updateOffsets(tempQ->operand1, symTable, TYPE_INTEGER);
    if (range->leftChild->nextSibling->isNegative)
    {
        snprintf(tempQ->operand2, 25, "-%d", range->leftChild->nextSibling->tk->integer);
    }
    else
    {
        snprintf(tempQ->operand2, 25, "%d", range->leftChild->nextSibling->tk->integer);
    }
    snprintf(tempQ->resultant, 25, "temp%d", temp0);
    // tempQ1->next = tempQ;
    head = appendAtEnd(head, tempQ);
    quadruple *tempQ2 = initQuadruple();
    tempQ2->op = JUMPIFTRUE;
    int label2 = newLabel();
    snprintf(tempQ2->operand1, 25, "temp%d", temp0);
    snprintf(tempQ2->operand2, 25, "label%d", label2);
    // tempQ->next = tempQ2;
    head = appendAtEnd(head, tempQ2);
    quadruple *tempQ3 = initQuadruple();
    tempQ3->op = JUMP;
    int label3 = newLabel();
    snprintf(tempQ3->operand1, 25, "label%d", label3);
    // tempQ2->next = tempQ3;
    head = appendAtEnd(head, tempQ3);
    quadruple *tempQ4 = initQuadruple();
    tempQ4->op = LABEL;
    snprintf(tempQ4->operand1, 25, "label%d", label2);
    // tempQ3->next = tempQ4;
    head = appendAtEnd(head, tempQ4);
    // get ready for statements
    astNode *forLoopStmts = stmts->leftChild->nextSibling->nextSibling->leftChild;
    // get code for stmts
    // printf("%s\n", EnumToASTString(forLoopStmts->label));
    quadruple *stmtsHead = stmtsCodeGen(forLoopStmts, symTable);

    appendAtEnd(head, stmtsHead);
    quadruple *tempQ5 = initQuadruple();
    strcpy(tempQ5->resultant, loopVar->tk->str);
    strcpy(tempQ5->operand1, loopVar->tk->str);
    tempQ5->offsetOperand1 = updateOffsets(tempQ5->operand1, symTable, TYPE_INTEGER);
    tempQ5->offsetRes = tempQ5->offsetOperand1;
    tempQ5->op = OP_PLUS;
    snprintf(tempQ5->operand2, 25, "%d", 1);
    appendAtEnd(head, tempQ5);
    quadruple *tempQ6 = initQuadruple();
    tempQ6->op = JUMP;
    snprintf(tempQ6->operand1, 25, "label%d", label1);
    appendAtEnd(head, tempQ6);
    quadruple *tempQ7 = initQuadruple();
    tempQ7->op = LABEL;
    snprintf(tempQ7->operand1, 25, "label%d", label3);
    appendAtEnd(head, tempQ7);
    return head;
}

// void getArrayCode(astNode *node, SymTablePointer *symTable)
// {
//     SymTablePointer *ptr = getFromAnySymTable(symTable, node->leftChild->tk->str);
//     types type = ptr->typeIfArray.type;
//     int width = -1;
//     if (type == TYPE_INTEGER)
//     {
//         width = INT_WIDTH;
//     }
//     else if (type == TYPE_REAL)
//     {
//         width = REAL_WIDTH;
//     }
//     else
//     {
//         width = BOOL_WIDTH;
//     }
//     int temp = newTemp();
//     sprintf(node->name, "temp%d", temp);
//     // get expression code
//     astNode *expr = node->leftChild->nextSibling;
//     getExpressionsCode(expr);
//     quadruple *qp = initQuadruple();
//     qp->op = OP_MUL;
//     strcpy(qp->resultant, node->name);
//     strcpy(qp->operand1, expr->name);
//     sprintf(qp->operand2, "%d", width);
//     expr->code = appendAtEnd(expr->code, qp);
//     node->code = expr->code;
//     return;
// }

void getGen(astNode *node, SymTablePointer *symTable)
{
    if (node->label == AST_ID)
    {
        strcpy(node->name, node->tk->str);
        return;
    }
    else if (node->label == AST_NUM)
    {
        sprintf(node->name, "%d", node->tk->integer);
        return;
    }
    else if (node->label == AST_RNUM)
    {
        sprintf(node->name, "%lf", node->tk->rnum);
        return;
    }
    else if (node->label == AST_BOOL)
    {
        strcpy(node->name, node->tk->str);
        return;
    }
    // else
    // {
    //     getArrayCode(node, symTable);
    //     // globalHead = appendAtEnd(globalHead, node->code);
    //     quadruple *qp = initQuadruple();
    //     qp->op = LW;
    //     int temp = newTemp();
    //     sprintf(qp->resultant, "temp%d", temp);
    //     strcpy(qp->operand1, node->leftChild->tk->str);
    //     strcpy(qp->operand2, node->name); // offset
    //     sprintf(node->name, "temp%d", temp);
    //     node->code = appendAtEnd(node->code, qp);
    //     return;
    // }
}

quadruple *generateSwitchCaseCode(astNode *stmts)
{
    quadruple *head = initQuadruple();
    SymTablePointer *symTable = stmts->symTable;
    head->op = JUMP;
    int label0 = newLabel();
    snprintf(head->operand1, 25, "label%d", label0);

    astNode *switchExpr = stmts->leftChild;
    astNode *caseStmts = switchExpr->nextSibling;
    // get code for case statements
    astNode *case1 = caseStmts->leftChild;
    astNode *def = caseStmts->nextSibling;
    // check if boolean or integer switch
    if (def->leftChild == NULL)
    {
        bool *casevalues = malloc(sizeof(bool) * 2);
        int *caselabels = malloc(sizeof(int) * 2);
        int labelexit = newLabel();
        quadruple *tempQ1 = initQuadruple();
        int label1 = newLabel();
        tempQ1->op = LABEL;
        snprintf(tempQ1->operand1, 25, "label%d", label1);
        caselabels[0] = label1;
        casevalues[0] = true;
        head = appendAtEnd(head, tempQ1);

        // printf("in true: %s\n", EnumToASTString(case1->leftChild->nextSibling->leftChild->label));
        quadruple *stmtsHead = stmtsCodeGen(case1->leftChild->nextSibling->leftChild, symTable);
        // printf("REACHED HERE!!!\n\n");
        // head = appendAtEnd(head, stmtsHead);
        quadruple *tempQ2 = initQuadruple();
        tempQ2->op = JUMP;
        snprintf(tempQ2->operand1, 25, "label%d", labelexit);
        head = appendAtEnd(head, tempQ2);

        quadruple *tempQ3 = initQuadruple();
        int label2 = newLabel();
        tempQ3->op = LABEL;
        snprintf(tempQ3->operand1, 25, "label%d", label2);
        caselabels[1] = label2;
        casevalues[1] = false;
        head = appendAtEnd(head, tempQ3);

        // printf("REACHED HERE2!!!\n\n");
        // printf("in false: %s\n", EnumToASTString(case1->nextSibling->leftChild->nextSibling->leftChild->label));
        quadruple *stmtsHeadFalse = stmtsCodeGen(case1->nextSibling->leftChild->nextSibling->leftChild, symTable);
        head = appendAtEnd(head, stmtsHeadFalse);
        quadruple *tempQ4 = initQuadruple();
        tempQ4->op = JUMP;
        snprintf(tempQ4->operand1, 25, "label%d", labelexit);
        head = appendAtEnd(head, tempQ4);

        quadruple *tempQ5 = initQuadruple();
        tempQ5->op = LABEL;
        snprintf(tempQ5->operand1, 25, "label%d", label0);
        head = appendAtEnd(head, tempQ5);

        quadruple *tempQ6 = initQuadruple();
        tempQ6->op = JUMPIFTRUE;
        strcpy(tempQ6->operand1, switchExpr->tk->str);
        snprintf(tempQ6->operand2, 25, "label%d", label1);
        head = appendAtEnd(head, tempQ6);

        quadruple *tempQ7 = initQuadruple();
        tempQ7->op = JUMP;
        snprintf(tempQ7->operand1, 25, "label%d", label2);
        head = appendAtEnd(head, tempQ7);

        quadruple *tempQ8 = initQuadruple();
        tempQ8->op = LABEL;
        snprintf(tempQ8->operand1, 25, "label%d", labelexit);
        head = appendAtEnd(head, tempQ8);

        free(casevalues);
        free(caselabels);
    }
    else
    {
        int *casevalues = malloc(sizeof(int) * 25);
        int *caselabels = malloc(sizeof(int) * 25);
        int size = 25;
        int i = 0; // tracks number of cases
        int labelexit = newLabel();
        while (case1 != NULL)
        {
            quadruple *tempQ1 = initQuadruple();
            // create a label for each case
            int label1 = newLabel();
            tempQ1->op = LABEL;
            snprintf(tempQ1->operand1, 25, "label%d", label1);
            caselabels[i] = label1;
            casevalues[i] = case1->leftChild->tk->integer;

            head = appendAtEnd(head, tempQ1);
            quadruple *stmtsHead = stmtsCodeGen(case1->leftChild->nextSibling->leftChild, symTable);
            head = appendAtEnd(head, stmtsHead);
            quadruple *tempQ2 = initQuadruple();
            tempQ2->op = JUMP;
            snprintf(tempQ2->operand1, 25, "label%d", labelexit);
            head = appendAtEnd(head, tempQ2);
            case1 = case1->nextSibling;
            i++;

            // keep reallocating whenever we reach half of the allocated space
            if (i >= size / 2)
            {
                size = size * 2;
                casevalues = realloc(casevalues, sizeof(int) * size);
                caselabels = realloc(caselabels, sizeof(int) * size);
            }
        }
        // note the default checks are redundant, but I'm keeping them for now
        int labeldef = newLabel();
        if (def->leftChild != NULL)
        {
            quadruple *tempQ3 = initQuadruple();
            tempQ3->op = LABEL;
            snprintf(tempQ3->operand1, 25, "label%d", labeldef);
            head = appendAtEnd(head, tempQ3);
            quadruple *stmtsHead = stmtsCodeGen(def->leftChild->leftChild, symTable);
            head = appendAtEnd(head, stmtsHead);
            quadruple *tempQ4 = initQuadruple();
            tempQ4->op = JUMP;
            snprintf(tempQ4->operand1, 25, "label%d", labelexit);
            head = appendAtEnd(head, tempQ4);
        }
        quadruple *tempQ5 = initQuadruple();
        tempQ5->op = LABEL;
        snprintf(tempQ5->operand1, 25, "label%d", label0);
        head = appendAtEnd(head, tempQ5);
        for (int j = 0; j < i; j++)
        {
            quadruple *tempQ6 = initQuadruple();
            int tempcmp = newTemp();
            tempQ6->op = OP_EQ;
            strcpy(tempQ6->operand1, switchExpr->tk->str);
            snprintf(tempQ6->operand2, 25, "%d", casevalues[j]);
            snprintf(tempQ6->resultant, 25, "temp%d", tempcmp);
            head = appendAtEnd(head, tempQ6);

            quadruple *tempQ7 = initQuadruple();
            tempQ7->op = JUMPIFTRUE;
            snprintf(tempQ7->operand1, 25, "temp%d", tempcmp);
            snprintf(tempQ7->operand2, 25, "label%d", caselabels[j]);
            head = appendAtEnd(head, tempQ7);
        }
        if (labeldef != -1)
        {
            quadruple *tempQ7 = initQuadruple();
            tempQ7->op = JUMP;
            snprintf(tempQ7->operand1, 25, "label%d", labeldef);
            head = appendAtEnd(head, tempQ7);
        }
        quadruple *tempQ8 = initQuadruple();
        tempQ8->op = LABEL;
        snprintf(tempQ8->operand1, 25, "label%d", labelexit);
        head = appendAtEnd(head, tempQ8);
        free(casevalues);
        free(caselabels);
    }
    return head;
}

void getPtrs(SymTablePointer *ptr, quadruple *head)
{
    while (head != NULL)
    {
        if (head->offsetOperand1 != -1)
        {
            head->op1Ptr = getFromAnySymTable(ptr, head->operand1);
        }
        else if (head->offsetOperand2 != -1)
        {
            head->op2Ptr = getFromAnySymTable(ptr, head->operand2);
        }
        else if (head->offsetRes != -1)
        {
            head->resPtr = getFromAnySymTable(ptr, head->resultant);
        }
        head = head->next;
    }
}

quadruple *stmtsCodeGen(astNode *stmts, SymTablePointer *symTable)
{
    quadruple *tempHead = NULL;
    while (stmts != NULL)
    {
        if (stmts->label == AST_FOR)
        {
            quadruple *head1 = generateForLoopCode(stmts);
            tempHead = appendAtEnd(tempHead, head1);
            // stmts->code = head1;
        }
        else if (stmts->label == AST_WHILE)
        {
            quadruple *head1 = generateWhileLoopCode(stmts);
            tempHead = appendAtEnd(tempHead, head1);
        }
        else if (stmts->label == AST_SWITCH_CASE)
        {
            quadruple *head1 = generateSwitchCaseCode(stmts);
            tempHead = appendAtEnd(tempHead, head1);
        }
        else if (stmts->label == AST_PRINT)
        {
            // printf("here\n");
            quadruple *head = initQuadruple();
            head->op = OP_PRINT;
            getGen(stmts->leftChild, symTable);
            tempHead = appendAtEnd(tempHead, stmts->leftChild->code);
            strcpy(head->operand1, stmts->leftChild->name);
            if (stmts->leftChild->label == AST_ID)
            {
                head->offsetOperand1 = getFromAnySymTable(symTable, stmts->leftChild->tk->str)->offset;
                head->op1Ptr = getFromAnySymTable(symTable, stmts->leftChild->tk->str);
            }
            getPtrs(symTable, head);
            tempHead = appendAtEnd(tempHead, head);
        }
        else if (stmts->label == AST_GETVALUE)
        {

            quadruple *head = initQuadruple();
            head->op = OP_GETVALUE;
            getGen(stmts->leftChild, symTable);
            strcpy(head->operand1, stmts->leftChild->name);
            head->offsetOperand1 = getFromAnySymTable(symTable, head->operand1)->offset;
            getPtrs(symTable, head);
            tempHead = appendAtEnd(tempHead, head);
        }
        else if (stmts->label == AST_ASSIGNOP)
        {

            if (stmts->leftChild->nextSibling->type == TYPE_BOOLEAN)
            {

                int labelTrue = newLabel();
                int labelFalse = newLabel();
                int labelExit = newLabel();
                quadruple *headTrue = initQuadruple();
                headTrue->op = LABEL;
                sprintf(headTrue->operand1, "label%d", labelTrue);
                // qp if returns true
                quadruple *qp1 = initQuadruple();
                qp1->op = OP_ASSIGN;
                strcpy(qp1->resultant, stmts->leftChild->tk->str);
                qp1->offsetRes = getFromAnySymTable(symTable, qp1->resultant)->offset;
                qp1->resPtr = getFromAnySymTable(symTable, qp1->resultant);
                strcpy(qp1->operand1, "true");
                // after this needs to jump to the end of this construct
                quadruple *qp2 = initQuadruple();
                qp2->op = JUMP;
                sprintf(qp2->operand1, "label%d", labelExit);
                // qp if returs false
                quadruple *headFalse = initQuadruple();
                headFalse->op = LABEL;
                sprintf(headFalse->operand1, "label%d", labelFalse);

                quadruple *qp3 = initQuadruple();
                qp3->op = OP_ASSIGN;
                strcpy(qp3->resultant, stmts->leftChild->tk->str);
                qp3->offsetRes = getFromAnySymTable(symTable, qp3->resultant)->offset;
                qp3->resPtr = getFromAnySymTable(symTable, qp3->resultant);
                strcpy(qp3->operand1, "false");
                // jump to exit
                quadruple *qp4 = initQuadruple();
                qp4->op = JUMP;
                sprintf(qp4->operand1, "label%d", labelExit);
                // generate expression's code
                stmts->leftChild->nextSibling->falseCase = labelFalse;
                stmts->leftChild->nextSibling->trueCase = labelTrue;
                getExpressionsCode(stmts->leftChild->nextSibling, symTable);
                // generate exit label
                quadruple *qp5 = initQuadruple();
                qp5->op = LABEL;
                sprintf(qp5->operand1, "label%d", labelExit);
                tempHead = appendAtEnd(tempHead, stmts->leftChild->nextSibling->code);
                tempHead = appendAtEnd(tempHead, headTrue);
                tempHead = appendAtEnd(tempHead, qp1);
                tempHead = appendAtEnd(tempHead, qp2);
                tempHead = appendAtEnd(tempHead, headFalse);
                tempHead = appendAtEnd(tempHead, qp3);
                tempHead = appendAtEnd(tempHead, qp4);

                tempHead = appendAtEnd(tempHead, qp5);
            }
            else
            {
                quadruple *head = initQuadruple();
                head->op = OP_ASSIGN;
                getExpressionsCode(stmts->leftChild->nextSibling, symTable);
                tempHead = appendAtEnd(tempHead, stmts->leftChild->nextSibling->code);
                strcpy(head->operand1, stmts->leftChild->nextSibling->name);
                if (stmts->leftChild->nextSibling->label != AST_RNUM && stmts->leftChild->nextSibling->label != AST_NUM && stmts->leftChild->nextSibling->label != AST_BOOL && stmts->leftChild->nextSibling->label != AST_SIGNED)
                {
                    head->offsetOperand1 = getFromAnySymTable(symTable, head->operand1)->offset;
                    head->op1Ptr = getFromAnySymTable(symTable, head->operand1);
                }
                else if (stmts->leftChild->nextSibling->label == AST_SIGNED)
                {
                    astNode *signedNode = stmts->leftChild->nextSibling;
                    if (signedNode->leftChild->nextSibling->label != AST_NUM && signedNode->leftChild->nextSibling->label != AST_RNUM && signedNode->leftChild->nextSibling->label != AST_BOOL)
                    {
                        head->offsetOperand1 = getFromAnySymTable(symTable, head->operand1)->offset;
                        head->op1Ptr = getFromAnySymTable(symTable, head->operand1);
                    }
                }
                if (stmts->leftChild->label == AST_ID)
                {
                    strcpy(head->resultant, stmts->leftChild->tk->str);
                    head->offsetRes = getFromAnySymTable(symTable, head->resultant)->offset;
                    head->resPtr = getFromAnySymTable(symTable, head->resultant);
                }
                tempHead = appendAtEnd(tempHead, head);
            }
        }
        stmts = stmts->nextSibling;
    }
    return tempHead;
}

void startIntermCodeGen(astNode *root)
{
    astNode *mdls = root->leftChild;

    while (mdls != NULL)
    {

        switch (mdls->label)
        {
        case AST_DRIVERMODULE:
        {
            SymTablePointer *symTable = mdls->symTable;
            astNode *stmts = mdls->leftChild->leftChild->leftChild;
            quadruple *qp = stmtsCodeGen(stmts, symTable);
            quadruple *qp1 = initQuadruple();
            qp1->op = LABEL;
            strcpy(qp1->operand1, "driver");
            globalHead = appendAtEnd(globalHead, qp1);
            globalHead = appendAtEnd(globalHead, qp);
            quadruple *qp2 = initQuadruple();
            qp2->op = LABEL;
            strcpy(qp2->operand1, "exit_driver");
            globalHead - appendAtEnd(globalHead, qp2);
            mdls = mdls->nextSibling;
            break;
        }
        case AST_OTHERMODULES:
        {
            astNode *separateMdls = mdls->leftChild;
            // while (separateMdls != NULL)
            // {
            //     SymTablePointer *symTable = separateMdls->symTable;
            //     astNode *stmts = separateMdls->leftChild->nextSibling->leftChild;
            //     //
            //     while (stmts != NULL)
            //     {
            //         if (stmts->label == AST_FOR)
            //         {
            //             generateForLoopCode(stmts);
            //         }
            //     }
            //     separateMdls = separateMdls->nextSibling;
            // }

            mdls = mdls->nextSibling;
        }
        }
    }
}

char *EnumToOperatorString(operators nt)
{
    int i = 0;
    FILE *fp = fopen("operators.txt", "r");
    char *buffer = malloc(sizeof(char) * 100);
    while (fgets(buffer, 100, fp) != NULL)
    {
        buffer[strlen(buffer) - 3] = '\0';
        // printf("%s\n", buffer);

        if (nt == i)
            return buffer;
        i++;
    }
    fclose(fp);
}

// int main()
// {
//     globalSymbolTable = initSymTablePointer();
//     globalSymbolTable->typeST = GLOBALST;
//     globalSymbolTable->parentHashTable = NULL;
//     hashtable ht1 = initHashtable();
//     globalSymbolTable->corrHashtable = &ht1;
//     FILE *fp = fopen("random4.txt", "r");
//     twinbuffer *tb = twinbuffer_init(fp, 256);
//     fill_grammar(fopen("Grammar.txt", "r"));
//     hashtable ht = initHashtable();
//     populate_hashtable(&ht);
//     populateParseTable();
//     treenode *root = parseInputSourceCode(fp, tb, ht);
//     astNode *astRoot = constructAST(root);
//     inorder_ast(astRoot);
//     populateGlobalSymbolTable(globalSymbolTable, astRoot, 0);
//     // if (semanticallyCorrect)
//     typeCheck(astRoot);
//     if (semanticallyCorrect && semanticRulesPassed)
//     {
//         startIntermCodeGen(astRoot);
//     }
//     while (globalHead != NULL)
//     {
//         printf("%20s %20s %20s %20s\n", EnumToOperatorString(globalHead->op), globalHead->operand1, globalHead->operand2, globalHead->resultant);
//         globalHead = globalHead->next;
//     }
// }
