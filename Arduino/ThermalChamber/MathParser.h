#ifndef __MATH_PARSER_H
#define __MATH_PARSER_H
#include <Arduino.h>

float unaryOperation(char* mathOperator, float arg, uint8_t* result)
{
	*result=1;
	if(mathOperator[0]=='-')return - arg;
	else if(mathOperator[0]=='!')return !arg;
	else if(mathOperator[0]=='+')return arg;
	*result=0;//ошибка - оператора нет в списке
	return arg;
	//return 0;
}

float binaryOperation(char* mathOperator, float arg1,float arg2, uint8_t* result)
{
	*result=1;
	if(memcmp(mathOperator,"&&",2)==0)return arg1&&arg2;
	else if(memcmp(mathOperator,"||",2)==0)return arg1||arg2;
	else if(memcmp(mathOperator,"==",2)==0)return arg1==arg2;
	else if(memcmp(mathOperator,">=",2)==0)return arg1>=arg2;
	else if(memcmp(mathOperator,"<=",2)==0)return arg1<=arg2;
	else if(memcmp(mathOperator,"!=",2)==0)return arg1!=arg2;
	else if(mathOperator[0]=='>')return arg1>arg2;
	else if(mathOperator[0]=='<')return arg1<arg2;
	else if(mathOperator[0]=='+')return arg1+arg2;
	else if(mathOperator[0]=='-')return arg1-arg2;
	else if(mathOperator[0]=='*')return arg1*arg2;
	else if(mathOperator[0]=='/')return arg1/arg2;
	*result=0;//ошибка - оператора нет в списке

}

float parse(char* str,uint16_t* numberByte,int previousOperatorPriority, uint8_t* result)
{
	char mathOperator_1[3]={0,0,0};
	int operatorPriority_1=0;
	char* endPtr;	
	float arg1,arg2;	
	
	//оператор или число,в начале выражения оператор может быть или ! или + или -  ноль здесь не проверяется, ноль проверяется в конце
	//следом должно идти число и оператор можно применить сразу, приоритет значения не имеет
	if(str[*numberByte]=='+'){operatorPriority_1=2;memcpy(mathOperator_1,"+",2);*numberByte+=1;}
	else if(str[*numberByte]=='-'){operatorPriority_1=2;memcpy(mathOperator_1,"-",2);*numberByte+=1;}
	else if(str[*numberByte]=='!'){operatorPriority_1=5;memcpy(mathOperator_1,"!",2);*numberByte+=1;}
	//else if(str[*numberByte]=='('){operatorPriority_1=-10;memcpy(mathOperator_1,"(",2);*numberByte+=1;}
	
	if(str[*numberByte]=='(')
	{
		*numberByte+=1;
		arg1 = parse(str,numberByte,-10,result);
	
	    if(mathOperator_1[0])//есть ли какая то унарная операция, если есть - выполняем
	    {
	        arg1=unaryOperation(mathOperator_1,arg1,result);
		    *result=0;

	        if(!result)return 0;
	    }
	    return arg1;
	}
	else
	{
	  arg1 = strtof (&str[*numberByte], &endPtr);		
	  if(endPtr==&str[*numberByte])//не число, значит ошибка
  	{
	  	*result=0;
		  return 0;
	  }
  }
	
	if(mathOperator_1[0])//есть ли какая то унарная операция, если есть - выполняем
	{
		arg1=unaryOperation(mathOperator_1,arg1,result);
	    if(!result)return 0;
	}
	
	*numberByte = endPtr-str;
	if(str[*numberByte]==')')
	{
		// *numberByte+=1;
		return arg1;
	}
	else if(str[*numberByte]==0)return arg1;
	
	do
	{
		//дальше должен быть оператор между аргументами
	  uint16_t endArg1Byte =*numberByte;
	  if(memcmp(&str[*numberByte],"&&",2)==0){operatorPriority_1=-2;memcpy(mathOperator_1,"&&",3);*numberByte+=2;}
	  else if(memcmp(&str[*numberByte],"||",2)==0){operatorPriority_1=-3;memcpy(mathOperator_1,"||",3);*numberByte+=2;}
	  else if(memcmp(&str[*numberByte],"==",2)==0){operatorPriority_1=-1;memcpy(mathOperator_1,"==",3);*numberByte+=2;}
	  else if(memcmp(&str[*numberByte],">=",2)==0){operatorPriority_1=-1;memcpy(mathOperator_1,">=",3);*numberByte+=2;}
	  else if(memcmp(&str[*numberByte],"<=",2)==0){operatorPriority_1=-1;memcpy(mathOperator_1,"<=",3);*numberByte+=2;}
	  else if(memcmp(&str[*numberByte],"!=",2)==0){operatorPriority_1=-1;memcpy(mathOperator_1,"!=",3);*numberByte+=2;}
	  else if(str[*numberByte]=='>'){operatorPriority_1=-1;memcpy(mathOperator_1,">",2);*numberByte+=1;}
	  else if(str[*numberByte]=='<'){operatorPriority_1=-1;memcpy(mathOperator_1,"<",2);*numberByte+=1;}
	  else if(str[*numberByte]=='+'){operatorPriority_1=2;memcpy(mathOperator_1,"+",2);*numberByte+=1;}
	  else if(str[*numberByte]=='-'){operatorPriority_1=2;memcpy(mathOperator_1,"-",2);*numberByte+=1;}
	  else if(str[*numberByte]=='*'){operatorPriority_1=3;memcpy(mathOperator_1,"*",2);*numberByte+=1;}
	  else if(str[*numberByte]=='/'){operatorPriority_1=3;memcpy(mathOperator_1,"/",2);*numberByte+=1;}
	  //else if(str[*numberByte]=='('){operatorPriority_1=-10;memcpy(mathOperator_1,"(",2);*numberByte+=1;}
	  else//на месте оператора какой то фуфел
	  {
		  *result=0;
		  return 0;
	  }
	
	  if(previousOperatorPriority>=operatorPriority_1)
	  {
		  *numberByte=endArg1Byte;//если начал сворачиваться - надо сворачиваться влево до конца
		  return arg1;
	  }
		
	  if(str[*numberByte]=='(')
	  {
		  *numberByte+=1;
		  arg2 = parse(str,numberByte,-10,result);
		  arg1 = binaryOperation(mathOperator_1,arg1,arg2,result);

		  if(str[*numberByte]==0)return arg1;
			
		   continue;
		  //return arg1;
	  }
	  
	  arg2 = parse(str,numberByte,operatorPriority_1,result);
		
	  arg1 = binaryOperation(mathOperator_1,arg1,arg2,result);
	}
  while(str[*numberByte]!=0&&str[*numberByte]!=')');
	
	if(str[*numberByte]==')')*numberByte+=1;
	
	return arg1;
	return 0;
}


#endif
