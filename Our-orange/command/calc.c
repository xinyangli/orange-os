#include "stdio.h"
#include "type.h"

#define NUM 0
#define PLUS_OP 1
#define MUL_OP 2
#define BRACK 3
#define INVALID 4
#define LEN 100 // maxlen
#define INF 2147483647

typedef struct token
{
    int type;
    char str[10];
    int num;
} Token;

void clear_rec(Token *rec)
{
    int k, m;
    for (k = 0; k < LEN; k++)
    {
        rec[k].type = INVALID;
        for (m = 0; m < 10; m++)
        {
            rec[k].str[m] = '\0';
        }
    }
}

int str2num(char *s)
{
    int num = 0, i = 0;
    for (i=0;i<strlen(s);i++)
    {
        if (s[i]<'0' || s[i]>'9'){
            return -INF;
        }
        num *= 10;
        num += s[i] - '0';
    }
    return num;
}

int calculate(Token *rec, int len)
{
    int left = 0;
    Token c[LEN];
    int index = 0;
    while (left < len)
    {
        if (rec[left].str[0] == '(')
        {
            int right = left + 1;
            int match = 1;
            while (match != 0)
            {
                if (rec[right].str[0] == '(')
                    match++;
                else if (rec[right].str[0] == ')')
                    match--;
                right++;
            }
            int tmp = calculate(&rec[left + 1], right - left - 2);
            if (tmp == INF)
            {
                return INF;
            }
            c[index].type = NUM;
            c[index].num = tmp;
            left = right;
        }
        else if (rec[left].type == MUL_OP || rec[left].type == PLUS_OP)
        {
            c[index].type = rec[left].type;
            c[index].str[0] = rec[left].str[0];
            left++;
        }
        else if (rec[left].type == NUM)
        {
            c[index].type = NUM;
            c[index].num = str2num(rec[left].str);
            left++;
        }
        if (c[index].type == NUM && index > 0 && c[index - 1].type == MUL_OP) //做乘除
        {
            int num1 = c[index - 2].num;
            int num2 = c[index].num;
            if (c[index - 1].str[0] == '*')
            {
                num1 *= num2;
            }
            else
            {
                if (num2 == 0)
                {
                    printf("divide zero!\n");
                    return INF;
                }
                num1 /= num2;
            }
            c[index - 2].num = num1;
            index--;
        }
        else
        {
            index++;
        }
    }
    int i = 0, ans = 0;
    int flag = 1;
    for (i = 0; i < index; i++)
    {
        if (c[i].type == PLUS_OP)
        {
            flag = c[i].str[0] == '+' ? 1 : -1;
        }
        else
        {
            ans += flag * c[i].num;
        }
    }
    return ans;
}

int main(int argc,char *argv[])
{
    char s[LEN];
    int k, j, index, flag, len;
    Token rec[LEN];
    int count = str2num(argv[1]);
    if (count == -INF){
        printf("argv[1] should be a number!\n");
        return -1;
    }
    for (k = 0; k < count; k++)
    {
        printf("Enter your expression: ");
        for (j = 0; j < LEN; j++)
        {
            s[j] = '\0';
        }
        clear_rec(rec);
        flag = 1; //标记是否有效
        index = 0;
        j = 0;
        len=read(0 ,s ,100);
        while (j < len) //词法分析
        {
            if (s[j] >= '0' && s[j] <= '9') // num
            {
                rec[index].type = NUM;
                int begin = j;
                while (j < len && s[j] >= '0' && s[j] <= '9')
                {
                    rec[index].str[j - begin] = s[j];
                    j++;
                }
                j--;
            }
            else if (s[j] == '+' || s[j] == '-')
            {
                rec[index].type = PLUS_OP;
                rec[index].str[0] = s[j];
            }
            else if (s[j] == '*' || s[j] == '/')
            {
                rec[index].type = MUL_OP;
                rec[index].str[0] = s[j];
            }
            else if (s[j] == '(' || s[j] == ')')
            {
                rec[index].type = BRACK;
                rec[index].str[0] = s[j];
            }
            else if (s[j] == ' ')
            {
                index -= 1;
            }
            else
            {
                flag = 0;
                break;
            }
            j++;
            index++;
        }
        if (!flag)
        {
            printf("invalid char %c\n", s[j]);
        }
        else
        {
            // 检查语法正确性
            int pre = -1;
            int brack_cnt = 0;
            flag = 1;
            for (j = 0; j < index && flag; j++)
            {
                if (rec[j].type == NUM)
                {
                }
                else if (rec[j].type == MUL_OP || rec[j].type == PLUS_OP)
                {
                    if (pre == MUL_OP || pre == PLUS_OP)
                    {
                        flag = 0;
                    }
                }
                else if (rec[j].type == BRACK)
                {
                    if (rec[j].str[0] == '(')
                    {
                        if (pre == NUM)
                        {
                            flag = 0;
                        }
                        brack_cnt++;
                    }
                    else
                    {
                        if (pre == MUL_OP || pre == PLUS_OP || pre == BRACK && rec[j - 1].str[0] == '(')
                        {
                            flag = 0;
                        }
                        brack_cnt--;
                    }
                    if (brack_cnt < 0)
                    {
                        flag = 0;
                    }
                }
                pre = rec[j].type;
            }
            if (brack_cnt != 0 || pre == MUL_OP || pre == PLUS_OP)
            {
                flag = 0;
            }
            if (!flag)
            {
                printf("Invalid grammer!\n");
            }
            else
            {
                int ans = calculate(rec, index);
                if (ans != INF)
                    printf("The answer is %d\n", ans);
                else
                    printf("Fail to calculate...\n");
            }
        }
    }
    return 0;
}
