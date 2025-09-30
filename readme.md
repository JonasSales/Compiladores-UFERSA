comandos para rodar o projeto:

flex lexer.l 

Para criar o arquivo lex.yy.cc é nele em que está a compilação do lexer.l

//Comando para compilar e criar o executavel no linux do código
g++ -o tonto_lexer lex.yy.cc main.cpp


//
./tonto_lexer teste.tonto para fazer a tabela de simbolos


Por fim será feito um arquivo com o nome do arquivo que foi passado.tonto.tok com os simbolos.