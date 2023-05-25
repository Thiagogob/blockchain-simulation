//Exemplo de uso da funcao SHA256 da biblioteca Open SSL
// Este código requer a instalação do pacote libssl-dev 
// Para compilar este código use gcc hash.c -lcrypto

#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h" //arquivo de definição necessário função SHA256
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"
#include <stdlib.h>

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

typedef struct listaBTC{
  unsigned int endereco;
  struct listaBTC *next;
}listaBTC;

listaBTC *criaNo(unsigned int endereco){
  listaBTC* novoEndereco = (listaBTC*)malloc(sizeof(listaBTC));
  if(novoEndereco == NULL){
    printf("Alocacao de memoria falhou\n");
    return NULL;
  }
  novoEndereco->endereco = endereco;
  novoEndereco->next = NULL;
  return novoEndereco;
}

void insereNoInicio(listaBTC** inicio, unsigned int endereco){
  listaBTC* novoEndereco = criaNo(endereco);
  if(inicio == NULL){
    *inicio = novoEndereco;
  }
  else{
    novoEndereco->next = *inicio;
    *inicio = novoEndereco;
  }
}

void mostraLista(listaBTC *inicio){
  listaBTC* aux = inicio;
  while(aux!=NULL){
    printf("%u - ", aux->endereco);
    aux = aux->next;
  }
  printf("\n");
}

void printHash(unsigned char hash[], int length)
{
  int i;
  
  for(i=0;i<length;++i)
    printf("%02x", hash[i]);

  printf("\n");
}

unsigned char gerarNumeroAleatorio(int valorMaximo){
    MTRand r = seedRand(1234567);
    unsigned char numeroAleatorio = genRandLong(&r)%valorMaximo;
    return numeroAleatorio;
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

BlocoMinerado minerarBlocoGenesis(BlocoNaoMinerado *blocoGenesis, unsigned int *carteira, listaBTC** enderecosComBTC){

  //vetor que armazenará o resultado do hash. Tamanho definidio pela libssl
  unsigned char hash[SHA256_DIGEST_LENGTH];

  //PROCESSO PARA DEFINIR NUMERO ALETORIO QUE REPRESENTA O MINERADOR DO BLOCO:
  unsigned char minerador = gerarNumeroAleatorio(256);
  blocoGenesis->data[183] = minerador;
  
  SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);

  while(hash[0]!=0){
    blocoGenesis->nonce++;
    SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);
  }

  //Agora que o bloco foi minerado
  //Vamos passar ele para a struct de bloco minerado
  BlocoMinerado* blocoGenesisMinerado = malloc(sizeof(BlocoMinerado));

  //os campos do bloco nao minerado sao enviados para o bloco minerado
  blocoGenesisMinerado->bloco = *blocoGenesis;

  //passando o hash que valida o bloco para a struct de bloco minerado
  for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
    blocoGenesisMinerado->hash[i] = hash[i];
  }

  /*
  printf("Hash: ");
  printHash(hash, SHA256_DIGEST_LENGTH);
  */

  //ATUALIZA CARTEIRA DO SISTEMA DEPOIS DA VALIDAÇÃO DO BLOCO
  carteira[minerador] = 50;

  //COLOCA PRIMEIRO MINERADOR NA LISTA DE ENDERECOS COM BTC
  insereNoInicio(enderecosComBTC, minerador);
  
  //mostraLista(*enderecosComBTC);

  return *blocoGenesisMinerado;
}

void inicializarBloco(unsigned char *hashAnterior, BlocoNaoMinerado *blocoN, int i){
  //preenchendo o campo do numero que representa
  blocoN->numero = i;

  //inicializando campo nonce
  blocoN->nonce = 0;

  //preenchendo o campo de hash anterior
  for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
    blocoN->hashAnterior[i] = hashAnterior[i];
  }

  //decidindo quem será o minerador do bloco
  unsigned char minerador = gerarNumeroAleatorio(256);
  blocoN->data[183] = minerador;

  //decidindo quantidade de transacoes no bloco
  unsigned char qtdTransacoes = gerarNumeroAleatorio(61);
   
  //definindo os elementos das transacoes
  for(int i=0; i<qtdTransacoes; i++){
    //definindo endereco de origem
    i++;

    //definindo endereco de destino
    i++;
    
    //definindo quantidade de BTC
    i++;

  }

}


int main(int argc, char *argv[])
{
 
  //instanciando e preenchendo bloco genesis
  //da maneira solicitada no enunciado
  BlocoNaoMinerado* blocoGenesis = malloc(sizeof(BlocoNaoMinerado));
  preencheBlocoGenesis(blocoGenesis);

  //inicializando carteira conforme pedido no enunciado
  unsigned int carteira[256];
  inicializaCarteira(carteira);

  //printf("CARTEIRA[122]: %u\n", carteira[122]);

  //cria lista dos enderecos com BTC
  listaBTC *enderecosComBTC = NULL;

  //cria vetor de 16 posições para armazenar blocos minerados
  BlocoMinerado vetorBlocosMinerados[16];

  //minerando o bloco genesis
  BlocoMinerado blocoGenesisMinerado = minerarBlocoGenesis(blocoGenesis, carteira, &enderecosComBTC);

  //Minera 30.000 blocos agora
  //BlocoMinerado blocoAnterior;
  /*
  for(int i = 0; i < 3; i++){

    //caso especial, onde o primeiro indice do vetor
    //eh o bloco genesis
    if(i==0){
      vetorBlocosMinerados[i] = blocoGenesisMinerado;
    }

    else{

       //alocando memoria para o bloco
       BlocoNaoMinerado* blocoN = malloc(sizeof(BlocoNaoMinerado));

       //chamando uma funcao para preencher o bloco
       //passando o hash anterior + o bloco a ser minerado + i
       inicializarBloco(vetorBlocosMinerados[(i-1)%16].hash, blocoN, i);


    }

  }
  */
  //unsigned char hashAnterior[SHA256_DIGEST_LENGTH];


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
  //printf("aleatorio teste: %u", gerarNumeroAleatorio(61));
  //printf("Hash: ");
  //printHash(hash, SHA256_DIGEST_LENGTH);
  //printf("Carteira[140]: %u", carteira[140]);
  //printf("hash bloco genesis minerado: ");
  //printHash(blocoGenesisMinerado.hash, SHA256_DIGEST_LENGTH);
  
  //mostraLista(enderecosComBTC);
  return 0;
}
