#ifndef __STRING_REPLASER
#define __STRING_REPLASER
#include "MathParser.h"

void forwardOrderCpy(char* dest,char* source,int len)
{
	for(int i=0;i<len;i++)
	{
		dest[i]=source[i];
	}
}

void reverseOrderCpy(char* dest,char* source,uint16_t len)
{
	for(uint16_t i=len;i>0;i--)
	{
		dest[i-1]=source[i-1];
	}
}
/*"A_IN_1>0.2&&D_IN_1==0&&D_IN_2==0&&D_IN_3==0"*/
/*r_04_data.d_out_1 = booleanExpressonResult(3
                                            ,memory.D_OUT_1_Formula
                                            ,"A_IN",r_04_data.a_in_1
                                            ,"D_IN_1",r_04_data.d_in_1
                                            ,"D_IN_2",r_04_data.d_in_2);*/

//аргументы следуют в порядке: строка названия переменной 1 -> переменная 1, строка названия переменной 2 -> переменная 2,
bool booleanExpressonResult(int numOfVar,char* booleanExpression, ... )
{
    char* strVariable="";
    int intVariable=0;
    float floatVariable=0;
    char resultBooleanExpression[60];

    uint16_t booleanExpressionLenght = 0;/*strlen(booleanExpression);*/
    for(uint16_t i=0;booleanExpression[i];i++)
    {
      resultBooleanExpression[booleanExpressionLenght]=booleanExpression[i];
      if(resultBooleanExpression[booleanExpressionLenght]!=' ')booleanExpressionLenght++;
    }
    resultBooleanExpression[booleanExpressionLenght]=0;
    
    uint16_t varNameLenght =0;

    va_list factor;         // указатель на необязательный параметр
    va_start(factor, booleanExpression);   // устанавливаем указатель

    for(uint16_t i=0;i<numOfVar; i++)
    {
      char* strVariable = va_arg(factor, char*);  // получаем значение текущего параметра - название переменной
      varNameLenght =strlen(strVariable);//+1;
      if(strVariable[0]=='A') floatVariable = va_arg(factor, double);//если A_IN_
      else if(strVariable[0]=='D') intVariable = va_arg(factor, int);//если D_IN
      
      for(uint16_t j = 0;j<booleanExpressionLenght-varNameLenght;j++)
      {
        if(memcmp(&resultBooleanExpression[j],strVariable,varNameLenght)==0)
        {
          uint16_t varlen = 0;
          char strTmp[12];
          if(strVariable[0]=='A')varlen = sprintf(strTmp,"%f",floatVariable);
          else if(strVariable[0]=='D')varlen = sprintf(strTmp,"%i",intVariable);

          if(varlen >varNameLenght)
          {
            reverseOrderCpy(&resultBooleanExpression[j+varlen],&resultBooleanExpression[j+varNameLenght],booleanExpressionLenght-(j+varNameLenght));
            booleanExpressionLenght+=(varlen - varNameLenght);
            resultBooleanExpression[booleanExpressionLenght]=0;
            reverseOrderCpy(&resultBooleanExpression[j],strTmp,varlen);
          }
          else if(varlen <varNameLenght)
          {
            forwardOrderCpy(&resultBooleanExpression[j+varlen],&resultBooleanExpression[j+varNameLenght],booleanExpressionLenght-(j+varNameLenght));
            booleanExpressionLenght-=(varNameLenght-varlen);
            resultBooleanExpression[booleanExpressionLenght]=0;
            forwardOrderCpy(&resultBooleanExpression[j],strTmp,varlen);
          }
          else forwardOrderCpy(&resultBooleanExpression[j],strTmp,varlen);//varlen == varNameLenght

          j+=varlen;
          /*break;*/
        }
      }
    }
    va_end(factor);

    uint8_t result=1;
    uint16_t numberByte=0;
    return parse(resultBooleanExpression, &numberByte, -10, &result);
}

#endif
