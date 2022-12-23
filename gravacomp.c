/* Eric Leao 2110694 3WB */
/* Pedro Machado Peçanha 2110535 3WB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// bit mais significativo = cont
// depois do cont = tipo
// depois do tipo = tamanho

// tamanho, tipo, cont
int powQueFunciona(int valor, int potencia);

int gravacomp(int nstructs, void* valores, char* descritor, FILE* arquivo) {
	unsigned char* percorreValores = valores;
	unsigned char numeroStructs = nstructs, cabecalho;
	int tamanhoStruct = 0, checaUltimo, tamanhoString, comprime, erro;
	erro = fwrite(&numeroStructs, 1, 1, arquivo);
	if (erro != 1) {
		printf("erro ao escrever arquivo.");
		return 1;
	}
	for (; nstructs > 0; nstructs--) {
		char *percorreDescritor = descritor;
		while (*percorreDescritor != '\0') {
			if (*percorreDescritor == 's') {
				//analisa s
				checaUltimo = (*(percorreDescritor + 3) == '\0');
				tamanhoString = strlen(percorreValores);
				//perguntar se pode usar strlen 
				//cabecalho
				cabecalho = 0;
				cabecalho ^= tamanhoString;
				if (checaUltimo)
					// precisamos colocar o 1 bit como "1", caso seja o último
					cabecalho ^= 128;
				// 0b10000000 -> quando compara, primeiro bit 0 ou 1
				cabecalho ^= 64;
				// colocando 1 no bit 6 (pedido no enunciado)
				erro = fwrite(&cabecalho, 1, 1, arquivo); // colocando o cabeçalho no arquivo
				if (erro != 1) {
					printf("erro ao escrever arquivo.");
					return 1;
				}
				for (int i = 0; i < tamanhoString; i++) {
					erro = fwrite(percorreValores + i, 1, 1, arquivo); // colocando no arquivo a string, sem o \0  
					if (erro != 1) {
						printf("erro ao escrever arquivo.");
						return 1;
					}
				}
				tamanhoStruct += ((*(percorreDescritor + 1) - '0') * 10 + *(percorreDescritor + 2) - '0');
				// aumentando o tamanho struct usando os "numeros" depois do s (sNN)
				percorreValores += ((*(percorreDescritor + 1) - '0') * 10 + *(percorreDescritor + 2) - '0');
				// andando o tamanho da string na struct original
				percorreDescritor += 3;
				// pulando o SNN no descritor
			}
			else if (*percorreDescritor == 'i') {
				//analisa i
				if (tamanhoStruct % 4) {
					percorreValores += 4 - tamanhoStruct % 4;
					tamanhoStruct += 4 - tamanhoStruct % 4;
					//pulando bytes de padding
				}
				//cabecalho
				checaUltimo = (*(percorreDescritor + 1) == '\0');
				cabecalho = 0;
				if (checaUltimo)
					// precisamos colocar o 1 bit como "1", caso seja o último
					cabecalho ^= 128;
				cabecalho ^= 32;
				// o 5 byte precisa ser 1, no caso do signed int
				comprime = comprime_valor_signed(percorreValores);
				// retorna quantidade de bytes necessária para armazenar o número
				cabecalho ^= comprime; // colocamos essa quantidade no cabecalho
				erro = fwrite(&cabecalho, 1, 1, arquivo); // escrevemos o cabecalho no arquivo
				if (erro != 1) {
					printf("erro ao escrever arquivo.");
					return 1;
				}
				//conteudo
				for (; comprime != 0; comprime--) {
					erro = fwrite(percorreValores + comprime - 1, 1, 1, arquivo);
					/* colocando apenas os bytes necessarios (o -1 é necessário pela)
					contagem começar no 0*/
					if (erro != 1) {
						printf("erro ao escrever arquivo.");
						return 1;
					}
				}
				percorreDescritor += 1;
				percorreValores += 4;
				tamanhoStruct += 4;
			}
			else {
				//analisa u
				if (tamanhoStruct % 4) {
					percorreValores += 4 - tamanhoStruct % 4;
					tamanhoStruct += 4 - tamanhoStruct % 4;
					//pulando bytes de padding
				}
				//cabecalho
				checaUltimo = (*(percorreDescritor + 1) == '\0');
				cabecalho = 0;
				if (checaUltimo)
					cabecalho ^= 128;
				// precisamos colocar o 1 bit como "1", caso seja o último
				comprime = comprime_valor(percorreValores);
				// retorna o tamanho comprimido do unsigned
				cabecalho ^= comprime;  // colocamos esse valor no cabecalho
				erro = fwrite(&cabecalho, 1, 1, arquivo);  //colocando o cabecalho no arquivo
				if (erro != 1) {
					printf("erro ao escrever arquivo.");
					return 1;
				}
				//conteudo
				for (; comprime != 0; comprime--) {
					fwrite(percorreValores + comprime - 1, 1, 1, arquivo);
					/* colocando apenas os bytes necessarios (o -1 é necessário pela)
					contagem começar no 0*/
					if (erro != 1) {
						printf("erro ao escrever arquivo.");
						return 1;
					}
				}
				percorreDescritor += 1;
				percorreValores += 4;
				tamanhoStruct += 4;

			}
		}
		//padding no final da struct, se precisar
		if ((strchr(descritor, 'i') != NULL || strchr(descritor, 'u') != NULL) && tamanhoStruct % 4) {
			percorreValores += 4 - tamanhoStruct % 4;
			tamanhoStruct += 4 - tamanhoStruct % 4;
		}
	}
	return 0;
}

int comprime_valor(unsigned int *n) {
	int a = 4;
	unsigned char * k = n;
	// k irá servir como auxiliar e caminhara pelos bytes de n
	k = k + 3;
	//posicionaremos ela no fim do numero (lembrar que é little-endian)
	for (int c = 0; *k == 0 && c <= 3; c++) {
		// enquanto o byte for 0, vamos reduzindo a qtd de bytes que precisamos armazenar
		// assim que ele possuir algum conteudo, saimos do for
		a--;
		k = k - 1;
	}
	return a;
}

int comprime_valor_signed(int *n) {
	int a = 4;
	unsigned char * k = n;
	k = k + 3;
	// aqui o algoritmo eh semelhante, porem não sabemos se o numero
	// eh negativo ou positivo
	// para poder cortar bytes, o primeiro tem que ser 0 ou 255 (FF)
	if (*k == 0) {

		for (int c = 0; (*k == 0) && ((*(k - 1)) & 128) == 0 && c <= 2; c++) {
			k--;
			a--;
		}
	}
	else if (*k == 255)
		for (int c = 0; (*k == 255) && ((*(k - 1)) & 128) && c <= 2; c++) {
			k--;
			a--;
		}
	if (a == 1 && *k == 0)
		a--;
	return a;
}

void mostracomp(FILE *arquivo) {
	int contador_estruturas = 0, flag_estruturas = 0;
	int tamanho, signedInteiro, qntd_estruturas;
	unsigned int unsignedInteiro;
	unsigned char percorreArquivo;
	percorreArquivo = fgetc(arquivo);
	// pegamos o primeiro byte, que contem a qtd de estruturas
	printf("Estruturas: %x\n\n", percorreArquivo);
	qntd_estruturas = percorreArquivo;
	while (contador_estruturas < qntd_estruturas) {
		// utilizaremos um comparador para entender quando parar o while
		percorreArquivo = fgetc(arquivo);
		if (percorreArquivo & 128) {
			contador_estruturas++;
			flag_estruturas++;
			// checando se eh o ultimo item da estrutura
			// se sim, aumentamos o contador e incrementamos uma flag
			// que sera usada para printar um \n no final 
		}
		//string
		if (percorreArquivo & 64) {
			// 6 byte = 1 -> string
			printf("(str) ");
			tamanho = (percorreArquivo & 63);
			for (int c = 0; c < tamanho; c++) {
				percorreArquivo = fgetc(arquivo);

				printf("%c", percorreArquivo);
			}
			puts("");
		}
		//signed
		else if (percorreArquivo & 32) {
			// 5 byte = 1 -> signed
			printf("(int) ");
			int c;
			signedInteiro = 0;
			tamanho = (percorreArquivo & 31);
			for (c = tamanho; c > 0; c--) {
				percorreArquivo = fgetc(arquivo);
				signedInteiro += percorreArquivo * (powQueFunciona(256, (c - 1)));
			}
			signedInteiro <<= 8 * (4 - tamanho);
			signedInteiro >>= 8 * (4 - tamanho);
			printf("%d (%08x)\n", signedInteiro, signedInteiro);
		}
		//unsigned
		else {
			// se nao for signed nem string, eh unsigned
			printf("(uns) ");
			int c;
			unsignedInteiro = 0;
			tamanho = (percorreArquivo & 31);
			for (c = tamanho; c > 0; c--) {
				percorreArquivo = fgetc(arquivo);
				unsignedInteiro += percorreArquivo * (powQueFunciona(256, c - 1));
			}
			printf("%u (%08x)\n", unsignedInteiro, unsignedInteiro);
		}

		if (flag_estruturas) {
			flag_estruturas = 0;
			puts("");
		}

	}

	return;
}

int powQueFunciona(int valor, int potencia) {
	if (potencia == 0) {
		return 1;
	}
	return valor * powQueFunciona(valor, potencia - 1);
}