//Exemplo de uso da funcao SHA256 da biblioteca Open SSL
// Este código requer a instalação do pacote libssl-dev 
// Para compilar este código use gcc hash.c -lcrypto

#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h" //arquivo de definição necessário função SHA256
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"

typedef struct BlocoNaoMinerado{
  unsigned short numero;
  unsigned int nonce;
  unsigned char data[184];
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
}BlocoNaoMinerado;


void printHash(unsigned char hash[], int length)
{
  int i;
  
  for(i=0;i<length;++i)
    printf("%02x", hash[i]);

  printf("\n");
}
 
void preencheBlocoTeste(BlocoNaoMinerado *primeiroBloco){

  //PREENCHENDO VETOR DATA COM ZEROS
  for(int i =0; i<184;i++){
      primeiroBloco->data[i] = 0;
  }

  //PREENCHENDO VETOR DE HASH ANTERIOR COM ZEROS, POIS ESSE É O PRIMEIRO BLOCO
  for(int i=0; i<(SHA256_DIGEST_LENGTH); i++){
    primeiroBloco->hashAnterior[i] = 0;
  }

  //COLOCANDO A FRASE DO BLOCO GÊNESIS
  char* str = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
  strcpy(primeiroBloco->data, str);

  //DEFININDO NONCE COMO 0 E NUMERO DO BLOCO COMO 1, COMO DITO NO ENUNCIADO
  primeiroBloco->nonce = 0;
  primeiroBloco->numero = 1;


  //PROCESSO PARA DEFINIR NUMERO ALETORIO:
  MTRand r = seedRand(1234567);
  primeiroBloco->data[183] = genRandLong(&r)%256;
  
  
        
}



int main(int argc, char *argv[])
{
 

  BlocoNaoMinerado primeiroBloco;
  preencheBlocoTeste(&primeiroBloco);
  
  


  unsigned char hash[SHA256_DIGEST_LENGTH];//vetor que armazenará o resultado do hash. Tamanho definidio pela libssl 

  //A funcao SHA256 requer tres parametros
  //1. o ponteiro para um vetor de unsigned char contendo o dado cujo hash você deseja calcular
  //2. o tamanho em bytes do dado cujo hash você deseja calcular
  //3. o ponteiro para um vetor de unsigned char que armazenará o resultado do hash calculado.
  SHA256((unsigned char *)&primeiroBloco, sizeof(BlocoNaoMinerado), hash);
  while(hash[0]!=0){
    primeiroBloco.nonce++;
    SHA256((unsigned char *)&primeiroBloco, sizeof(BlocoNaoMinerado), hash);
  }
  printf("Nonce: %d\n", primeiroBloco.nonce);
  printf("Minerador: %u\n", primeiroBloco.data[183]);
  printf("Hash: ");
  printHash(hash, SHA256_DIGEST_LENGTH);
  return 0;
}
