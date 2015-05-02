#include "typefunction.h"

TypeFunction::TypeFunction()
{
    this->functionName = "";
    this->returnType = "";
    this->arguments = QStringList();
    this->argumentTypes = QStringList();
}

void TypeFunction::SetFunctionName(QString name)
{
    this->functionName = name;
}

QString TypeFunction::GetFunctionName()
{
    return this->functionName;
}

void TypeFunction::SetFunctionReturnType(QString retType)
{
    this->returnType = retType;
}

QString TypeFunction::GetFunctionReturnType()
{
    return this->returnType;
}

void TypeFunction::AddFunctionArgument(QString argument)
{
    this->arguments<<argument;
}

QString TypeFunction::getFunctionArgument(int nArg)
{
    return this->arguments.at(nArg);
}

int TypeFunction::getArgumentCount()
{
    return this->arguments.size();
}

void TypeFunction::setFunctionArguments(QStringList arguments)
{
    this->arguments = arguments;
}

QStringList TypeFunction::getFunctionArguments()
{
    return this->arguments;
}

void TypeFunction::AddFunctionArgumentTypes(QString argument_type)
{
    this->argumentTypes<<argument_type;
}

QString TypeFunction::getFunctionArgumentType(int nArg)
{
    return this->argumentTypes.at(nArg);
}

int TypeFunction::getArgumentTypeCount()
{
    return this->argumentTypes.size();
}

void TypeFunction::setFunctionArgumentTypes(QStringList argumentTypes)
{
    this->argumentTypes = argumentTypes;
}

QStringList TypeFunction::getFunctionArgumentTypes()
{
    return this->argumentTypes;
}
