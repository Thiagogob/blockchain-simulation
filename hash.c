//Exemplo de uso da funcao SHA256 da biblioteca Open SSL
// Este código requer a instalação do pacote libssl-dev 
// Para compilar este código use gcc hash.c -lcrypto

#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h" //arquivo de definição necessário função SHA256
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"

typedef struct BlocoNaoMinerado{
  unsigned int numero;
  unsigned int nonce;
  unsigned char data[184];
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
}BlocoNaoMinerado;

typedef struct BlocoMinerado{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH];
}BlocoMinerado;


void printHash(unsigned char hash[], int length)
{
  int i;
  
  for(i=0;i<length;++i)
    printf("%02x", hash[i]);

  printf("\n");
}
 
void preencheBlocoGenesis(BlocoNaoMinerado *blocoGenesis){

  //PREENCHENDO VETOR DATA COM ZEROS
  for(int i =0; i<184;i++){
      blocoGenesis->data[i] = 0;
  }

  //PREENCHENDO VETOR DE HASH ANTERIOR COM ZEROS, POIS ESSE É O PRIMEIRO BLOCO
  for(int i=0; i<(SHA256_DIGEST_LENGTH); i++){
    blocoGenesis->hashAnterior[i] = 0;
  }

  //COLOCANDO A FRASE DO BLOCO GÊNESIS
  char* str = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
  strcpy(blocoGenesis->data, str);

  //DEFININDO NONCE COMO 0 E NUMERO DO BLOCO COMO 1, COMO DITO NO ENUNCIADO
  blocoGenesis->nonce = 0;
  blocoGenesis->numero = 1;
        
}

void inicializaCarteira(unsigned int * carteira){
  for (int i = 0; i < 256; i++)
  {
    carteira[i] = 0;
  }
  
}

BlocoMinerado minerarBlocoGenesis(BlocoNaoMinerado *blocoGenesis, unsigned int *carteira){

  //vetor que armazenará o resultado do hash. Tamanho definidio pela libssl
  unsigned char hash[SHA256_DIGEST_LENGTH];

  //PROCESSO PARA DEFINIR NUMERO ALETORIO QUE REPRESENTA O MINERADOR DO BLOCO:
  MTRand r = seedRand(1234567);
  unsigned int minerador = genRandLong(&r)%256;
  blocoGenesis->data[183] = minerador;
  
  SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);

  while(hash[0]!=0){
    blocoGenesis->nonce++;
    SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);
  }

  BlocoMinerado blocoGenesisMinerado;
  blocoGenesisMinerado.bloco = *blocoGenesis;

  for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
    blocoGenesisMinerado.hash[i] = hash[i];
  }

  /*
  printf("Hash: ");
  printHash(hash, SHA256_DIGEST_LENGTH);
  */

 //ATUALIZA CARTEIRA DO SISTEMA DEPOIS DA VALIDAÇÃO DO BLOCO
  carteira[minerador] = 50;

  return blocoGenesisMinerado;

}

int main(int argc, char *argv[])
{
 
  //instanciando e preenchendo bloco genesis
  //da maneira solicitada no enunciado
  BlocoNaoMinerado blocoGenesis;
  preencheBlocoGenesis(&blocoGenesis);

  //inicializando carteira conforme pedido no enunciado
  unsigned int carteira[256];
  inicializaCarteira(carteira);

  //printf("CARTEIRA[122]: %u\n", carteira[122]);

  //minerando o bloco genesis
  BlocoMinerado blocoGenesisMinerado = minerarBlocoGenesis(&blocoGenesis, carteira);

  //Minera 30.000 blocos agora
  //minerarBloco();
  /*
  //vetor que armazenará o resultado do hash. Tamanho definidio pela libssl
  unsigned char hash[SHA256_DIGEST_LENGTH]; 

  //A funcao SHA256 requer tres parametros
  //1. o ponteiro para um vetor de unsigned char contendo o dado cujo hash você deseja calcular
  //2. o tamanho em bytes do dado cujo hash você deseja calcular
  //3. o ponteiro para um vetor de unsigned char que armazenará o resultado do hash calculado.
  SHA256((unsigned char *)&blocoGenesis, sizeof(BlocoNaoMinerado), hash);

  while(hash[0]!=0){
    blocoGenesis.nonce++;
    SHA256((unsigned char *)&blocoGenesis, sizeof(BlocoNaoMinerado), hash);
  }
  */
 
  printf("Nonce: %d\n", blocoGenesisMinerado.bloco.nonce);
  printf("Minerador: %u\n", blocoGenesisMinerado.bloco.data[183]);
  //printf("Hash: ");
  //printHash(hash, SHA256_DIGEST_LENGTH);
  //printf("Carteira[140]: %u", carteira[140]);
  printf("hash bloco genesis minerado: ");
  printHash(blocoGenesisMinerado.hash, SHA256_DIGEST_LENGTH);

  return 0;
}
