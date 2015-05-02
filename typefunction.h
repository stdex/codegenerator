#ifndef TYPEFUNCTION_H
#define TYPEFUNCTION_H
#include <QString>
#include <QStringList>

class TypeFunction
{
    //function name
    QString functionName;
    //return type of function
    QString returnType;
    //arguments of function
    QStringList arguments;
    //types of function arguments
    QStringList argumentTypes;
public:
    TypeFunction();
    //Getters/Setters
    void SetFunctionName(QString name);
    QString GetFunctionName();
    void SetFunctionReturnType(QString retType);
    QString GetFunctionReturnType();
    void AddFunctionArgument(QString argument);
    QString getFunctionArgument(int nArg);
    int getArgumentCount();
    void setFunctionArguments(QStringList arguments);
    QStringList getFunctionArguments();

    void AddFunctionArgumentTypes(QString argument_type);
    QString getFunctionArgumentType(int nArg);
    int getArgumentTypeCount();
    void setFunctionArgumentTypes(QStringList argumentTypes);
    QStringList getFunctionArgumentTypes();
};

#endif // TYPEFUNCTION_H
